#include "ServerSequencedDataMessage.h"

#include <stdio.h>

#include "../MessageFactory.h"
#include "../PacketBase.h"

namespace itch
{

ServerSequencedDataMessage::ServerSequencedDataMessage()
        : MessageBase ( TYPE )
        , m_strTimestamp ( "" )
        , m_strPayload ( "" )
        , m_pPayloadMessage ( NULL )
{
}

ServerSequencedDataMessage::~ServerSequencedDataMessage()
{
    if (m_pPayloadMessage != NULL)
    {
        delete m_pPayloadMessage;
        m_pPayloadMessage = NULL;
    }
}


bool ServerSequencedDataMessage::Load ( PacketBase& packet )
{
    char chMarker = packet.PeekChar();
    if ( chMarker == TLF )
    {
        // This is a "depleted" message, which is used by the server
        // to identify session termination.
        m_bEndOfSession = true;
        return true;
    }

    // Great, this is not the session terminator message. Let's read
    // the rest of it.
    if ( !packet.ReadString ( m_strTimestamp, 9 ) )
        return false;

    // The remainder of the message is the payload.
    if ( !packet.ReadUntilTLF ( m_strPayload ) )
        return false;

    PacketBase payloadPacket = m_strPayload;
    m_pPayloadMessage = MessageFactory::ParseMessage ( MESSAGE_HOTSPOTFX_NAMESPACE, payloadPacket );
    if (m_pPayloadMessage == NULL)
        return false;

    return MessageBase::Load(packet);
}

void ServerSequencedDataMessage::LogMessage ( FILE* pLogFile ) const
{
    MessageBase::LogMessage ( pLogFile );
    if ( !m_bEndOfSession )
    {
        fprintf ( pLogFile, SESSION_LOG_FIELD_DELIMITER "%s"
                  SESSION_LOG_FIELD_DELIMITER,
                  m_strTimestamp.c_str() );

        if (m_pPayloadMessage != NULL)
            m_pPayloadMessage->LogMessage(pLogFile);
        else
            fprintf(pLogFile, "%s", m_strPayload.c_str());
    }
}

static MessageBase *CREATE() {
    return new ServerSequencedDataMessage();
}
static MessageFactoryRegistry __registry ( ServerSequencedDataMessage::TYPE, CREATE );

}
