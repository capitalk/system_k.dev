#ifndef SERVERLOGINREJECTEDMESSAGE_H
#define SERVERLOGINREJECTEDMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class ServerLoginRejectedMessage : public MessageBase
{
public:
    static const char TYPE = 'J';

    ServerLoginRejectedMessage();

    bool Load ( PacketBase& packet );

    void LogMessage ( FILE *pLogFile ) const;

    const std::string& GetReason() const {
        return m_strReason;
    }

protected:
    std::string m_strReason;
};

}

#endif // SERVERLOGINREJECTEDMESSAGE_H
