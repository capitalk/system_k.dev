#ifndef SESSION_H
#define SESSION_H

#include <pthread.h>
#include <queue>
#include <list>

#include "Socket.h"
#include "msg/MessageBase.h"

namespace itch
  {

  class ServerInstrumentDirectoryMessage;
  class ServerUnsupportedMessage;
  class ServerErrorNotificationMessage;
  class ServerSequencedDataMessage;
  class ServerLoginRejectedMessage;
  class ServerLoginAcceptedMessage;

// Override the default with -DDISPATCH_THREAD_POOL_SIZE=## compiler switch
#ifndef DISPATCH_THREAD_POOL_SIZE
#define DISPATCH_THREAD_POOL_SIZE 8
#endif

// Override the default with -DHEARTBEAT_DELAY_SECONDS=## compiler switch
// NOTE: the server expects client heartbeats AT MOST 15 seconds apart.
#ifndef HEARTBEAT_DELAY_SECONDS
#define HEARTBEAT_DELAY_SECONDS 1
#endif

#ifndef SERVER_TIMEOUT_SECONDS
#define SERVER_TIMEOUT_SECONDS 2
#endif

  class Session
    {
    public:
      Session ( const char *pszHostname, int nPort, const char *pszSessionLogFile = NULL );
      virtual ~Session();

      bool Login ( const char *pszUsername, const char *pszPassword, bool bMarketDataUnsubscribe );
      bool Logout();
      bool GetMarketSnapshot ( const char *pszCurrencyPair );
      bool SubscribeTicker ( const char *pszCurrencyPair );
      bool UnsubscribeTicker ( const char *pszCurrencyPair );
      bool SubscribeMarketData ( const char *pszCurrencyPair );
      bool UnsubscribeMarketData ( const char *pszCurrencyPair );
      bool GetInstrumentDirectory();

      // Returns true if the connection is closed, or false if the connection
      // was not closed within the specified timeout period.
      bool WaitClose ( long nTimeoutMilliseconds );

      inline bool IsConnected() const { return m_pSocket != NULL && m_pSocket->IsConnected(); }
      inline time_t GetUptimeSeconds() const { return m_pSocket == NULL ? 0 : m_pSocket->GetUptimeSeconds(); }
      inline long GetMessagesSentCount() const { return m_nMessagesSent; }
      inline long GetMessagesReceivedCount() const { return m_nMessagesReceived; }

      inline int GetHeartbeatDelay() const { return m_nHeartbeatDelaySeconds; }
      void SetHeartbeatDelay ( int nDelaySeconds = HEARTBEAT_DELAY_SECONDS );

      inline int GetServerTimeout() const { return m_nServerTimeoutSeconds; }
      inline void SetServerTimeout( int nTimeoutSeconds = SERVER_TIMEOUT_SECONDS ) { m_nServerTimeoutSeconds = nTimeoutSeconds; }

      void Start();
      void Stop();

    protected:
      virtual void OnLoginAccepted ( const ServerLoginAcceptedMessage& msg ) {}
      virtual void OnLoginRejected ( const ServerLoginRejectedMessage& msg ) {}
      virtual void OnSessionEnd ( const ServerSequencedDataMessage& msg ) {}
      virtual void OnErrorNotification ( const ServerErrorNotificationMessage& msg ) {}
      virtual void OnInstrumentDirectory( const ServerInstrumentDirectoryMessage& msg) {}
      virtual void OnNewOrder( const ServerSequencedDataMessage& msg ) {}
      virtual void OnModifyOrder( const ServerSequencedDataMessage& msg ) {}
      virtual void OnCancelOrder( const ServerSequencedDataMessage& msg ) {}
      virtual void OnMarketSnapshot( const ServerSequencedDataMessage& msg ) {}
      virtual void OnTicker( const ServerSequencedDataMessage& msg ) {}
      virtual void OnUnsupportedMessage(const ServerUnsupportedMessage& msg) {}

    private:
      const std::string m_strHostname;
      const int m_nPort;
      const std::string m_strSessionLogFile;
      pthread_mutex_t m_logFileMutex;
      FILE *m_pLogFile;
      int m_nLogFileEntries;
      std::string m_strCurrentSessionLogFile;
      int m_nLogLastDay;
      
      long m_nMessagesReceived;
      long m_nMessagesSent;

      Socket *m_pSocket;

      bool m_bShutdown;

      pthread_t m_serverWorker;
      pthread_t m_clientWorker;

      timespec m_lastClientMessage;
      int m_nHeartbeatDelaySeconds;
      int m_nServerTimeoutSeconds;

      pthread_mutex_t m_clientSocketMutex;
      pthread_cond_t m_clientSocketCond;

      pthread_t m_dispatchPool[DISPATCH_THREAD_POOL_SIZE];
      pthread_mutex_t m_dispatchMutex;
      pthread_cond_t m_dispatchCond;
      std::queue<MessageBase *> m_dispatchQueue;

      bool SendMessage ( const MessageBase& msg );
      void DispatchServerMessage ( MessageBase *pMessage );

      void LogMessage ( bool bToServer, const MessageBase& msg );

      static void *DispatcherWorker ( void *p );
      static void *ServerWorker ( void *p );
      static void *ClientWorker ( void *p );
    };

}

#endif // SESSION_H
