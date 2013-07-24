#ifndef MESSAGEBASE_H
#define MESSAGEBASE_H

#include "../Socket.h"

#ifndef SESSION_LOG_FIELD_DELIMITER
#define SESSION_LOG_FIELD_DELIMITER "|"
#endif

namespace itch
{

class PacketBase;

class MessageBase
{
public:
    virtual ~MessageBase();

    inline bool IsEndOfSession() const {
        return m_bEndOfSession;
    }
    inline bool IsHeartbeat() const {
        return m_bHeartbeat;
    }

    virtual bool Load ( PacketBase& packet );
    virtual bool Save ( PacketBase& packet ) const;

    virtual void LogMessage ( FILE* pLogFile ) const;

    inline char GetMessageType() const {
        return m_cMessageType;
    }

protected:
    const char m_cMessageType;
    bool m_bEndOfSession;
    bool m_bHeartbeat;

    MessageBase ( char cMessageType, bool bHeartbeat = false, bool bEndOfSession = false );
};

}

#endif // MESSAGEBASE_H
