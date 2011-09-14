#ifndef SERVERLOGINACCEPTEDMESSAGE_H
#define SERVERLOGINACCEPTEDMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class ServerLoginAcceptedMessage : public MessageBase
{
public:
    static const char TYPE = 'A';

    ServerLoginAcceptedMessage();

    bool Load ( PacketBase& packet );

    void LogMessage ( FILE *pLogFile ) const;

    int GetSequenceNumber() const {
        return m_nSequenceNumber;
    }

protected:
    int m_nSequenceNumber;
};

}

#endif // SERVERLOGINACCEPTEDMESSAGE_H
