#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <time.h>

#if !defined(DONT_USE_RING_BUFFER)
#include "ExpandableRingBuffer.h"
#endif

#define TLF (char)0x0A

namespace itch
{

class PacketBase;

class Socket
{
public:
    Socket ( const char *pszHostname, int nPort );
    virtual ~Socket();

    void Close();

    // Send the specified packet over the wire. Return false if the send() function
    // complains and true otherwise.
    bool SendPacket ( const PacketBase& packet );

    // Ensure that the message is fully buffered before returning (return: true),
    // or the specified timeout expires (return: false). Note that specifying
    // nTimeoutMilliseconds will instruct the function to check whatever data is
    // currently in the receive buffer and return immediately without waiting.
    bool RecvMessage ( int nTimeoutMilliseconds );

    // Retrieve a full packet (up to and including TLF character). Returns false
    // if the socket closes before the the packet is fully received and true
    // otherwise. Please note that the function WILL BLOCK if the packet is not
    // already in the internal buffer (see RecvMessage(timeout) function).
    bool RecvPacket ( PacketBase& packet );

    inline bool IsConnected() {
        return ( m_nSocket >= 0 );
    }

    time_t GetUptimeSeconds() const;

private:
    int m_nSocket;
    time_t m_timeConnected;
    time_t m_timeDisconnected;

#if defined(DONT_USE_RING_BUFFER)
    char *m_pcRecvBuffer;
    int m_nRecvBufferSize;
    int m_nRecvBufferPos;
#else
    ExpandableRingBuffer m_buffer;
#endif

    inline bool RecvIntoBuffer() {
        bool bHasTLF;
        return RecvIntoBuffer ( bHasTLF );
    }
    bool RecvIntoBuffer ( bool& bHasTLF );
};

}

#endif // SOCKET_H
