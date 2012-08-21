#ifndef SERVERERRORNOTIFICATIONMESSAGE_H
#define SERVERERRORNOTIFICATIONMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class ServerErrorNotificationMessage : public MessageBase
{
public:
    static const char TYPE = 'E';

    ServerErrorNotificationMessage();

    bool Load ( PacketBase& packet );

    void LogMessage ( FILE *pLogFile ) const;

    inline const std::string& GetExplanation() const {
        return m_strExplanation;
    }

protected:
    std::string m_strExplanation;
};

}

#endif // SERVERERRORNOTIFICATIONMESSAGE_H
