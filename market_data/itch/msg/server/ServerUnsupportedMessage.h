#ifndef SERVERUNSUPPORTEDMESSAGE_H
#define SERVERUNSUPPORTEDMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class ServerUnsupportedMessage : public MessageBase
{
public:
    ServerUnsupportedMessage ( char cMessageType );

    bool Load ( PacketBase& packet );

    void LogMessage ( FILE *pLogFile ) const;

protected:
    std::string m_strBody;
};

}

#endif // SERVERUNSUPPORTEDMESSAGE_H
