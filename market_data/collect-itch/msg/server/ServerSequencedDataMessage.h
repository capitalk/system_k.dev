#ifndef SERVERDISCONNECTMESSAGE_H
#define SERVERDISCONNECTMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class PacketBase;

class ServerSequencedDataMessage : public MessageBase
{
public:
    static const char TYPE = 'S';

    ServerSequencedDataMessage();
    virtual ~ServerSequencedDataMessage();

    bool Load ( PacketBase& packet );

    void LogMessage ( FILE *pLogFile ) const;

    inline const std::string& GetTimestamp() const {
        return m_strTimestamp;
    }
    inline const std::string& GetPayload() const {
        return m_strPayload;
    }
    inline const MessageBase *GetPayloadMessage() const {
        return m_pPayloadMessage;
    }

protected:
    std::string m_strTimestamp;
    std::string m_strPayload;

    MessageBase *m_pPayloadMessage;
};

}

#endif // SERVERDISCONNECTMESSAGE_H
