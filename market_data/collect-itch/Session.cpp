#include "Session.h"

#include <stdio.h>
#include <time.h>
#include <string.h>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

#include "msg/MessageFactory.h"
#include "msg/PacketBase.h"
#include "Logging.h"

#include "msg/client/ClientHeartbeatMessage.h"
#include "msg/client/ClientLoginMessage.h"
#include "msg/client/ClientLogoutMessage.h"
#include "msg/client/ClientMarketSnapshotMessage.h"
#include "msg/client/ClientTickerSubscribeMessage.h"
#include "msg/client/ClientTickerUnsubscribeMessage.h"
#include "msg/client/ClientMarketDataSubscribeMessage.h"
#include "msg/client/ClientMarketDataUnsubscribeMessage.h"
#include "msg/client/ClientInstrumentDirectoryMessage.h"
#include "msg/server/ServerLoginAcceptedMessage.h"
#include "msg/server/ServerLoginRejectedMessage.h"
#include "msg/server/ServerSequencedDataMessage.h"
#include "msg/server/ServerErrorNotificationMessage.h"
#include "msg/server/ServerInstrumentDirectoryMessage.h"
#include "msg/hotspotfx/HotspotFxNewOrderMessage.h"
#include "msg/hotspotfx/HotspotFxModifyOrderMessage.h"
#include "msg/hotspotfx/HotspotFxCancelOrderMessage.h"
#include "msg/hotspotfx/HotspotFxMarketSnapshotMessage.h"
#include "msg/hotspotfx/HotspotFxTickerMessage.h"

namespace itch
{

Session::Session ( const char* pszHostname, int nPort, const char* pszSessionLogFile )
        : m_strHostname ( pszHostname )
        , m_nPort ( nPort )
        , m_strSessionLogFile ( pszSessionLogFile )
        , m_pLogFile ( NULL )
        , m_nLogFileEntries ( 0 )
        , m_nMessagesReceived ( 0 )
        , m_nMessagesSent ( 0 )
        , m_pSocket ( NULL )
        , m_serverWorker ( 0 )
        , m_clientWorker ( 0 )
        , m_bShutdown ( true )
        , m_nHeartbeatDelaySeconds ( HEARTBEAT_DELAY_SECONDS )
{
    pthread_mutex_init ( &m_clientSocketMutex, NULL );
    pthread_cond_init ( &m_clientSocketCond, NULL );
    pthread_mutex_init ( &m_dispatchMutex, NULL );
    pthread_cond_init ( &m_dispatchCond, NULL );

    if ( !m_strSessionLogFile.empty() )
        pthread_mutex_init ( &m_logFileMutex, NULL );

    memset ( m_dispatchPool, 0, sizeof ( m_dispatchPool ) );
}

Session::~Session()
{
    Stop();

    if ( !m_strSessionLogFile.empty() )
        pthread_mutex_destroy ( &m_logFileMutex );
    if ( m_pLogFile != NULL )
    {
        fclose(m_pLogFile);
        m_pLogFile = NULL;
    }

    pthread_cond_destroy ( &m_dispatchCond );
    pthread_mutex_destroy ( &m_dispatchMutex );
    pthread_cond_destroy ( &m_clientSocketCond );
    pthread_mutex_destroy ( &m_clientSocketMutex );
}

bool Session::Login ( const char* pszUsername, const char* pszPassword, bool bMarketDataUnsubscribe )
{
    return SendMessage ( ClientLoginMessage ( pszUsername, pszPassword, bMarketDataUnsubscribe ) );
}

bool Session::Logout()
{
    return SendMessage ( ClientLogoutMessage() );
}

bool Session::GetMarketSnapshot ( const char* pszCurrencyPair )
{
    return SendMessage ( ClientMarketSnapshotMessage ( pszCurrencyPair ) );
}

bool Session::SubscribeTicker ( const char* pszCurrencyPair )
{
    return SendMessage ( ClientTickerSubscribeMessage ( pszCurrencyPair ) );
}

bool Session::UnsubscribeTicker ( const char* pszCurrencyPair )
{
    return SendMessage ( ClientTickerUnsubscribeMessage ( pszCurrencyPair ) );
}

bool Session::SubscribeMarketData ( const char* pszCurrencyPair )
{
    return SendMessage ( ClientMarketDataSubscribeMessage ( pszCurrencyPair ) );
}

bool Session::UnsubscribeMarketData ( const char* pszCurrencyPair )
{
    return SendMessage ( ClientMarketDataUnsubscribeMessage ( pszCurrencyPair ) );
}

bool Session::GetInstrumentDirectory()
{
    return SendMessage ( ClientInstrumentDirectoryMessage() );
}

bool Session::WaitClose ( long nTimeoutMilliseconds )
{
    if ( !IsConnected() )
        return true;

    timespec timeout;
    clock_gettime ( CLOCK_REALTIME, &timeout );
    timeout.tv_sec += nTimeoutMilliseconds / 1000;
    timeout.tv_nsec += ( nTimeoutMilliseconds % 1000 ) * 1000000;

    pthread_mutex_lock ( &m_clientSocketMutex );
    pthread_cond_timedwait ( &m_clientSocketCond, &m_clientSocketMutex, &timeout );
    pthread_mutex_unlock ( &m_clientSocketMutex );

    return !IsConnected();
}

void Session::SetHeartbeatDelay ( int nDelaySeconds )
{
    if ( nDelaySeconds <= 0 )
        nDelaySeconds = HEARTBEAT_DELAY_SECONDS;
    if ( m_nHeartbeatDelaySeconds == nDelaySeconds )
        return;

    pthread_mutex_lock ( &m_clientSocketMutex );
    m_nHeartbeatDelaySeconds = nDelaySeconds;
    pthread_mutex_unlock ( &m_clientSocketMutex );
}

void Session::Start()
{
    // Don't try to re-start an already running session.
    if ( IsConnected() )
        return;

    // Establish a connection to the server. Make sure the connection is established
    // before proceeding to worker thread creation. The said workers expect the connection
    // to be up and running, otherwise they may experience untimely termination.
    // The socket's input buffer will accumulate any iconming data for the time it is
    // necessary to fire up a new thread.
    m_pSocket = new Socket ( m_strHostname.c_str(), m_nPort );
    if ( !m_pSocket->IsConnected() )
    {
        delete m_pSocket;
        m_pSocket = NULL;
    }
    else
    {
        m_bShutdown = false;

        m_nMessagesReceived = 0;
        m_nMessagesSent = 0;

        pthread_attr_t attr;
        pthread_attr_init ( &attr );
        pthread_attr_setdetachstate ( &attr, PTHREAD_CREATE_JOINABLE );

        // Fire up the dispatcher thread pool.
        for ( int index = 0; index < DISPATCH_THREAD_POOL_SIZE; index++ )
        {
            pthread_create ( &m_dispatchPool[index], &attr, DispatcherWorker, ( void * ) this );
        }

        // While not true -- we did not send any handshake messages here --
        // the server won't timeout at least for another 15 seconds. That's
        // why we are going to initilize the timer this way.
        clock_gettime ( CLOCK_REALTIME, &m_lastClientMessage );

        // Create worker threads
        pthread_create ( &m_serverWorker, &attr, ServerWorker, ( void * ) this );
        pthread_create ( &m_clientWorker, &attr, ClientWorker, ( void * ) this );

        pthread_attr_destroy ( &attr );
    }
}

void Session::Stop()
{
    m_bShutdown = true;

    // Shutdown client heartbeat worker thread
    if ( m_clientWorker > 0 )
    {
        pthread_mutex_lock ( &m_clientSocketMutex );
        // Speed up the shutdown process by making sure the client worker
        // thread knows about m_bShutdown=true; otherwise, we are facing
        // the timeout of up to m_nHeartbeatDelaySeconds seconds.
        pthread_cond_broadcast ( &m_clientSocketCond );
        pthread_mutex_unlock ( &m_clientSocketMutex );

        void *result;
        pthread_join ( m_clientWorker, &result );
        m_clientWorker = 0;
    }

    // Shutdown the server response worker thread
    if ( m_serverWorker > 0 )
    {
        void *result;
        pthread_join ( m_serverWorker, &result );
        m_serverWorker = 0;
    }

    // Shut down the dispatch thread pool. Note that the threads
    // won't shutdown until all server responses are successfully
    // processed (e.g. the queue is empty).
    pthread_mutex_lock ( &m_dispatchMutex );
    pthread_cond_broadcast ( &m_dispatchCond );
    pthread_mutex_unlock ( &m_dispatchMutex );
    for ( int index = 0; index < DISPATCH_THREAD_POOL_SIZE; index++ )
    {
        if ( m_dispatchPool[index] > 0 )
        {
            void *result;
            pthread_join ( m_dispatchPool[index], &result );
            m_dispatchPool[index] = 0;
        }
    }

    // Dispose of the socket instance
    if ( m_pSocket != NULL )
    {
        delete m_pSocket;
        m_pSocket = NULL;
    }

    if ( m_pLogFile != NULL )
    {
        fclose ( m_pLogFile );
        m_pLogFile = NULL;
    }
}

bool Session::SendMessage ( const MessageBase& msg )
{
    if ( !IsConnected() )
        return false;

    bool rc = false;

    pthread_mutex_lock ( &m_clientSocketMutex );
    if ( IsConnected() )
    {
        PacketBase packet;
        if ( msg.Save ( packet ) && m_pSocket->SendPacket( packet ) )
        {
            clock_gettime ( CLOCK_REALTIME, &m_lastClientMessage );
            rc = true;

            // Make sure the message is properly logged in the session file.
            LogMessage ( true, msg );
        }
    }
    pthread_cond_broadcast ( &m_clientSocketCond );
    pthread_mutex_unlock ( &m_clientSocketMutex );

    return rc;
}

void Session::DispatchServerMessage ( MessageBase *pMessage )
{
    // Make sure the message is properly logged in the session file.
    LogMessage ( false, *pMessage );

    if ( pMessage->IsHeartbeat() )
    {
        // This is just the server heartbeat message.
        delete pMessage;
    }
    else
    {
        if ( pMessage->IsEndOfSession() )
        {
            // The server wants to end the session. We
            // can close the socket at this point.
            pthread_mutex_lock ( &m_clientSocketMutex );
            m_pSocket->Close();
            pthread_cond_broadcast ( &m_clientSocketCond );
            pthread_mutex_unlock ( &m_clientSocketMutex );
        }

        // Enqueue the message for processing on a thread
        // pool thread. The calling thread (which is the
        // server response handler) must not wait.
        pthread_mutex_lock ( &m_dispatchMutex );
        m_dispatchQueue.push ( pMessage );
        pthread_cond_broadcast ( &m_dispatchCond );
        pthread_mutex_unlock ( &m_dispatchMutex );
    }
}

void Session::LogMessage ( bool bToServer, const MessageBase& msg )
{
    if ( !bToServer )
        ++m_nMessagesReceived;
    else
        ++m_nMessagesSent;

    if ( m_strSessionLogFile.empty() )
        return;

    pthread_mutex_lock ( &m_logFileMutex );

    struct timespec ts;
    clock_gettime ( CLOCK_REALTIME, &ts );

    struct tm tm;
    localtime_r(&ts.tv_sec, &tm);

    if ( m_strCurrentSessionLogFile.empty() || m_nLogLastDay != tm.tm_mday )
    {
        if ( m_pLogFile != NULL )
        {
            fclose ( m_pLogFile );
            m_pLogFile = NULL;
        }
        m_nLogLastDay = tm.tm_mday;

        char szBuffer[10];
        sprintf(szBuffer, "-%04d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

        boost::filesystem::path pathSessionLogFile = m_strSessionLogFile;
        m_strCurrentSessionLogFile = pathSessionLogFile.parent_path().string();

        // Need a path separator, but only when there's a path.
        if ( !m_strCurrentSessionLogFile.empty()
                && m_strCurrentSessionLogFile[m_strCurrentSessionLogFile.length() - 1] != '/')
        {
            m_strCurrentSessionLogFile += "/";
        }

        m_strCurrentSessionLogFile += pathSessionLogFile.stem() + szBuffer + pathSessionLogFile.extension();

        LOG ( "created new date-stamped session log file name: '%s'.",
              m_strCurrentSessionLogFile.c_str() );
    }

    if ( m_pLogFile == NULL )
    {
        m_pLogFile = fopen ( m_strCurrentSessionLogFile.c_str(), "a" );
        m_nLogFileEntries = 0;
    }
    if ( m_pLogFile != NULL )
    {
        // Timestamp messages in the log in format
        // YYYY-MM-DD HH:MM:SS.mmmuuu
        fprintf ( m_pLogFile, "%04d-%02d-%02d %02d:%02d:%02d.%06ld", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                  tm.tm_hour, tm.tm_min, tm.tm_sec, ts.tv_nsec / 1000);

        fprintf ( m_pLogFile, ( bToServer ? ">>>" : "<<<" ) );
        msg.LogMessage ( m_pLogFile );
        fprintf ( m_pLogFile, ".\n" );

        if ( ++m_nLogFileEntries >= 100 )
        {
            fclose ( m_pLogFile );
            m_pLogFile = NULL;
        }
    }

    pthread_mutex_unlock ( &m_logFileMutex );
}


void *Session::DispatcherWorker ( void *p )
{
    Session *pSession = ( Session * ) p;

    while ( true )
    {
        MessageBase *pMessage = NULL;

        pthread_mutex_lock ( &pSession->m_dispatchMutex );

        while ( !pSession->m_bShutdown && pSession->m_dispatchQueue.empty() )
            pthread_cond_wait ( &pSession->m_dispatchCond, &pSession->m_dispatchMutex );

        if ( !pSession->m_dispatchQueue.empty() )
        {
            pMessage = pSession->m_dispatchQueue.front();
            pSession->m_dispatchQueue.pop();
        }

        pthread_mutex_unlock ( &pSession->m_dispatchMutex );

        if ( pMessage == NULL )
            break;

        // Route the message to the appropriate handler
        switch ( pMessage->GetMessageType() )
        {
        case ServerLoginAcceptedMessage::TYPE:
            pSession->OnLoginAccepted ( * ( ServerLoginAcceptedMessage * ) pMessage );
            break;
        case ServerLoginRejectedMessage::TYPE:
            pSession->OnLoginRejected ( * ( ServerLoginRejectedMessage * ) pMessage );
            break;
        case ServerSequencedDataMessage::TYPE:
            if ( pMessage->IsEndOfSession() )
                pSession->OnSessionEnd ( * ( ServerSequencedDataMessage * ) pMessage );
            else
            {
                ServerSequencedDataMessage *pSeqMessage = (ServerSequencedDataMessage *)pMessage;
                switch (pSeqMessage->GetPayloadMessage()->GetMessageType())
                {
                case HotspotFxNewOrderMessage::TYPE:
                    pSession->OnNewOrder(*pSeqMessage);
                    break;
                case HotspotFxModifyOrderMessage::TYPE:
                    pSession->OnModifyOrder(*pSeqMessage);
                    break;
                case HotspotFxCancelOrderMessage::TYPE:
                    pSession->OnCancelOrder(*pSeqMessage);
                    break;
                case HotspotFxMarketSnapshotMessage::TYPE:
                    pSession->OnMarketSnapshot(*pSeqMessage);
                    break;
                case HotspotFxTickerMessage::TYPE:
                    pSession->OnTicker(*pSeqMessage);
                    break;
                }
            }
            break;
        case ServerErrorNotificationMessage::TYPE:
            pSession->OnErrorNotification ( * ( ServerErrorNotificationMessage * ) pMessage );
            break;
        case ServerInstrumentDirectoryMessage::TYPE:
            pSession->OnInstrumentDirectory ( * ( ServerInstrumentDirectoryMessage * ) pMessage );
            break;
        }

        // Don't forget to dispose of the message. While cross-thread heap
        // operations are generally discouraged, they work just fine and usually
        // incur an insignificant heap arena synchronization overhead.
        delete pMessage;
    }

    return NULL;
}

void *Session::ServerWorker ( void *p )
{
    LOG ( "ServerWorker starting up..." );

    Session *pSession = ( Session * ) p;
    timespec tsLastMessageSeen;
    clock_gettime(CLOCK_REALTIME, &tsLastMessageSeen);

    while ( !pSession->m_bShutdown )
    {
        MessageBase *pResponseMessage = MessageFactory::ReceiveMessage ( pSession->m_pSocket, 500 );
        if ( pResponseMessage == NULL )
        {
            if ( pSession->m_bShutdown )
                break;

            // One of the things we might want to check is the server timeout.
            // Sometimes, the server may abruptly stop sending messages and
            // won't close/shutdown the socket. We want to identify this condition
            // and force socket closure.
            if (pSession->m_pSocket->IsConnected())
            {
                timespec tsCurrentTime;
                clock_gettime(CLOCK_REALTIME, &tsCurrentTime);
                if (tsCurrentTime.tv_sec - tsLastMessageSeen.tv_sec >= pSession->m_nServerTimeoutSeconds)
                {
                    LOG ( "No server messages seen for more than %d second(s) -- timeout", pSession->m_nServerTimeoutSeconds );
                    pSession->m_pSocket->Close();
                }
            }


            if ( !pSession->m_pSocket->IsConnected() )
            {
                // This might be the first time we've noticed that the socket is
                // closed. Let's notify the client thread and whomever might be
                // waiting for this socket closure to occur.
                pthread_mutex_lock ( &pSession->m_clientSocketMutex );
                pthread_cond_broadcast ( &pSession->m_clientSocketCond );
                pthread_mutex_unlock ( &pSession->m_clientSocketMutex );
                break;
            }
        }
        else
        {
            pSession->DispatchServerMessage ( pResponseMessage );
            clock_gettime(CLOCK_REALTIME, &tsLastMessageSeen);
        }
    }

    LOG ( "ServerWorker shutting down..." );
    return NULL;
}

void *Session::ClientWorker ( void *p )
{
    LOG ( "ClientWorker starting up..." );

    Session *pSession = ( Session * ) p;
    ClientHeartbeatMessage heartbeatMessage;
    PacketBase heartbeatMessagePacket;
    if (!heartbeatMessage.Save( heartbeatMessagePacket ))
    {
        LOG ( "Unable to pre-create a heartbeat message packet." );
        LOG ( "ClientWorker shutting down..." );
        return NULL;
    }

    while ( !pSession->m_bShutdown && pSession->IsConnected() )
    {
        pthread_mutex_lock ( &pSession->m_clientSocketMutex );

        timespec timeout;
        memcpy ( &timeout, &pSession->m_lastClientMessage, sizeof ( timespec ) );
        timeout.tv_sec += pSession->m_nHeartbeatDelaySeconds;

        pthread_cond_timedwait ( &pSession->m_clientSocketCond, &pSession->m_clientSocketMutex, &timeout );

        // While we were waiting, a request to shutdown may have arrived, or the socket was closed.
        if ( !pSession->m_bShutdown && pSession->IsConnected() )
        {
            // Whether or not pthread_cond_timedwait() has inidicated a timeout condition, we still
            // want to check whether the m_lastClientMessage is in the past for no less than
            // m_nHeartbeatDelaySeconds.
            clock_gettime ( CLOCK_REALTIME, &timeout );
            if ( timeout.tv_sec - pSession->m_lastClientMessage.tv_sec >= pSession->m_nHeartbeatDelaySeconds )
            {
                if ( !pSession->m_pSocket->SendPacket ( heartbeatMessagePacket ) )
                {
                    // If unable to send the heartbeat message, make sure to close the socket.
                    LOG ( "Unable to send the heartbeat message -- closing the socket" );
                    pSession->m_pSocket->Close();
                }
                else
                {
                    pSession->LogMessage ( true, heartbeatMessage );
                }

                // Remember the date/time stamp of this send, so the next timeout
                // will be calculated against this value.
                memcpy ( &pSession->m_lastClientMessage, &timeout, sizeof ( timespec ) );
            }
        }

        pthread_mutex_unlock ( &pSession->m_clientSocketMutex );
    }

    LOG ( "ClientWorker shutting down..." );
    return NULL;
}

}
