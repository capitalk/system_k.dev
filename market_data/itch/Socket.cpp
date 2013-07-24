#include "Socket.h"

#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#include "Logging.h"
#include "msg/PacketBase.h"

namespace itch
{

Socket::Socket ( const char *pszHostname, int nPort )
        : m_nSocket ( -1 )
        , m_timeConnected ( 0 )
        , m_timeDisconnected ( 0 )
#if defined(DONT_USE_RING_BUFFER)
        , m_pcRecvBuffer ( NULL )
        , m_nRecvBufferSize ( 0 )
        , m_nRecvBufferPos ( 0 )
#endif
{
    if ( !pszHostname )
    {
        LOG ( "the specified hostname is a null pointer" );
        return;
    }
    if ( nPort <= 0 )
    {
        LOG ( "the specified port number is out of range: %d", nPort );
        return;
    }

    char szPort[16];
    sprintf ( szPort, "%d", nPort );

    struct addrinfo *pAddress = NULL;
    struct addrinfo hints = {0};
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int rc = getaddrinfo ( pszHostname, szPort, &hints, &pAddress );
    if ( rc != 0 )
    {
        LOG ( "getaddrinfo error (%d): %s", rc, gai_strerror ( rc ) );
        return;
    }

    if ( pAddress == NULL )
    {
        LOG ( "getaddrinfo failed to resolve '%s' into a network address", pszHostname );
        return;
    }

    for ( struct addrinfo *pEntry = pAddress; pEntry; pEntry = pEntry->ai_next )
    {
        m_nSocket = socket ( pEntry->ai_family, pEntry->ai_socktype, pEntry->ai_protocol );
        if ( m_nSocket < 0 )
        {
            LOG ( "socket error (%d): %s", errno, strerror ( errno ) );
            continue;
        }

        rc = connect ( m_nSocket, pAddress->ai_addr, pAddress->ai_addrlen );
        if ( rc != 0 )
        {
            LOG ( "connect error (%d): %s", errno, strerror ( errno ) );
            close ( m_nSocket );
            m_nSocket = -1;
        }

        break;
    }

    freeaddrinfo ( pAddress );

    if ( m_nSocket < 0 )
        return;

    time ( &m_timeConnected );
    LOG ( "success: connected to %s:%d", pszHostname, nPort );
}

Socket::~Socket()
{
    Close();
}

void Socket::Close()
{
#if defined(DONT_USE_RING_BUFFER)
    if ( m_pcRecvBuffer != NULL )
    {
        free ( m_pcRecvBuffer );
        m_pcRecvBuffer = NULL;

        m_nRecvBufferPos = 0;
        m_nRecvBufferSize = 0;
    }
#endif

    time ( &m_timeDisconnected );

    if ( m_nSocket < 0 )
        return;

    shutdown ( m_nSocket, SHUT_RDWR );
    close ( m_nSocket );
    m_nSocket = -1;
}

bool Socket::SendPacket ( const PacketBase& packet )
{
    if ( m_nSocket < 0 )
        return false;

    const std::string& strBuffer = packet.GetBuffer();
    size_t nBytes = strBuffer.size();

    if ( nBytes <= 0 )
        return true;

    const char *pcBuffer = strBuffer.c_str();

    int nSentBytes;
    while ( ( nSentBytes = send ( m_nSocket, pcBuffer, nBytes, 0 ) ) >= 0 )
    {
        nBytes -= nSentBytes;
        pcBuffer += nSentBytes;

        if ( nBytes <= 0 )
            break;

        if ( nSentBytes == 0 )
        {
            // Nothing was sent here, meaning the outbound buffer has overflown
            // and will accept no more data. We'll wait for a second and close
            // down the connection if no buffer space becomes available.
            
            timeval timeout = {0};
            timeout.tv_sec = 1;
            
            fd_set rfds;
            FD_ZERO ( &rfds );
            FD_SET ( m_nSocket, &rfds );

            int rc = select ( m_nSocket + 1, NULL, &rfds, &rfds, &timeout );
            if ( rc < 0 )
            {
                LOG ( "select error (%d): %s", errno, strerror ( errno ) );
                Close();
                return false;
            }
            else if ( rc == 0 )
            {
                // The operation has timed out, and no buffer space has become
                // available for us.
                Close();
                return false;
            }

            // Buffer space has become available for us to use. Let's send more
            // data.
        }
    }

    if ( nSentBytes < 0 )
    {
        LOG ( "send error (%d): %s", errno, strerror ( errno ) );
        Close();
        return false;
    }

    // Presently, packets may not be properly terminated by MessageBase.Save()
    // code, therefore as we understand ITCH protocol well at this point, we
    // shall add TLF automatically (if it is not present in the packet).
    if (strBuffer[strBuffer.size() - 1] != TLF)
    {
        char c = TLF;
        nSentBytes = send(m_nSocket, &c, 1, 0);
        if (nSentBytes < 0)
        {
            LOG("send error (%d): %s", errno, strerror(errno));
            Close();
            return false;
        }
    }

    return true;
}



bool Socket::RecvMessage ( int nTimeoutMilliseconds )
{
    if ( m_nSocket < 0 )
        return false;

#if defined(DONT_USE_RING_BUFFER)
    // Check to see if the buffer already contains TLF character
    // which is the well-known message boundary.
    if ( m_pcRecvBuffer != NULL && m_nRecvBufferPos < m_nRecvBufferSize - 1 )
    {
        if ( memchr ( m_pcRecvBuffer + m_nRecvBufferPos, TLF, m_nRecvBufferSize - m_nRecvBufferPos ) != NULL )
            return true;
    }
#else
    if (m_buffer.ContainsByte(TLF))
        return true;
#endif

    // Wait until the remote sends us the TLF character, or the specified timeout expires
    timeval timeout = {0};
    timeout.tv_sec = nTimeoutMilliseconds / 1000;
    timeout.tv_usec = ( nTimeoutMilliseconds % 1000 ) * 1000000;

    do
    {
        fd_set rfds;
        FD_ZERO ( &rfds );
        FD_SET ( m_nSocket, &rfds );

        int rc = select ( m_nSocket + 1, &rfds, NULL, &rfds, &timeout );
        if ( rc < 0 )
        {
            LOG ( "select error (%d): %s", errno, strerror ( errno ) );
            Close();
            return false;
        }
        else if ( rc == 0 )
        {
            // Timeout
            return false;
        }

        // else rc > 0
        bool bHasTLF;
        if ( !RecvIntoBuffer ( bHasTLF ) )
            return false;

        if ( bHasTLF )
            return true;
    }
    while ( timeout.tv_sec >= 0 || timeout.tv_usec >= 0 );

    // Timeout expired
    return false;
}

bool Socket::RecvIntoBuffer ( bool& bHasTLF )
{
    char tempBuffer[2048];

    int nRecvSize = recv ( m_nSocket, tempBuffer, sizeof ( tempBuffer ), 0 );
    if ( nRecvSize < 0 )
    {
        LOG ( "recv error (%d): %s", errno, strerror ( errno ) );
        Close();
        return false;
    }
    if ( nRecvSize == 0 )
    {
        LOG ( "the peer has performed an orderly shutdown" );
        Close();
        return false;
    }

    // Determine if the received buffer has the well-known message boundary character
    bHasTLF = memchr ( tempBuffer, TLF, nRecvSize );

#if defined(DONT_USE_RING_BUFFER)
    // Place the contents of tempBuffer into the m_pRecvBuffer
    if ( m_pcRecvBuffer == NULL || m_nRecvBufferPos < nRecvSize )
    {
        // There's not enough space in the existing buffer to accommodate the
        // recently received data (or there's no buffer currently exists), therefore
        // we must (re-)allocate the existing buffer and expand its size.
        LOG ( "expanding receive buffer from %d to %d", m_nRecvBufferSize, m_nRecvBufferSize + nRecvSize );
        m_pcRecvBuffer = ( char * ) realloc ( m_pcRecvBuffer, m_nRecvBufferSize + nRecvSize );
        memcpy ( m_pcRecvBuffer + m_nRecvBufferSize, tempBuffer, nRecvSize );
        m_nRecvBufferSize += nRecvSize;
    }
    else
    {
        // The existing buffer has enough free space to accommodate the recently
        // received data. We have to move the data within the existing buffer so
        // the recent data can be copied to the existing buffer tail.
        if ( m_nRecvBufferPos < m_nRecvBufferSize )
            memmove ( m_pcRecvBuffer + m_nRecvBufferPos - nRecvSize, m_pcRecvBuffer + m_nRecvBufferPos, nRecvSize );
        memcpy ( m_pcRecvBuffer + m_nRecvBufferSize - nRecvSize, tempBuffer, nRecvSize );
        m_nRecvBufferPos -= nRecvSize;
    }
#else
    m_buffer.WriteData(tempBuffer, nRecvSize);
#endif

    return true;
}

bool Socket::RecvPacket ( PacketBase& packet )
{
    std::string str;

    while ( true )
    {
#if defined(DONT_USE_RING_BUFFER)
        // Check to see if the buffer already contains TLF character.
        if ( m_pcRecvBuffer != NULL && m_nRecvBufferPos < m_nRecvBufferSize - 1 )
        {
            char *pcTLF = ( char * ) memchr ( m_pcRecvBuffer + m_nRecvBufferPos, TLF, m_nRecvBufferSize - m_nRecvBufferPos );
            if ( pcTLF )
            {
                // Good news, the whole packet is int the buffer.
                int nLength = pcTLF - ( m_pcRecvBuffer + m_nRecvBufferPos ) + 1;
                str.append ( m_pcRecvBuffer + m_nRecvBufferPos, nLength );
                m_nRecvBufferPos += nLength;
                packet.Assign ( str );
                return true;
            }
            else
            {
                // The buffer exists, but its contents do not contain the whole packet.
                // Copy the current buffer contents into the packet.
                str.append ( m_pcRecvBuffer + m_nRecvBufferPos, m_nRecvBufferSize - m_nRecvBufferPos );
                m_nRecvBufferPos = m_nRecvBufferSize;
            }
        }
#else
        char szBuffer[2048];
        int nLength = m_buffer.ReadDataUtilByte(szBuffer, sizeof(szBuffer), TLF);
        if (nLength > 0)
        {
            str.append(szBuffer, nLength);
            if (szBuffer[nLength - 1] == TLF)
            {
                packet.Assign(str);
                return true;
            }
        }
#endif
        RecvIntoBuffer();
    }
}

time_t Socket::GetUptimeSeconds() const
{
    // If the socket was never connected, its runtime is zero
    if ( m_timeConnected == 0 )
        return 0;

    // If the socket was never disconnected, its runtime extends
    // to the current time.
    time_t endTime = m_timeDisconnected;
    if ( endTime == 0 )
        time ( &endTime );

    return endTime - m_timeConnected;
}


}
