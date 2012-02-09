#include "ServerLoginAcceptedMessage.h"

#include <stdio.h>

#include "../MessageFactory.h"
#include "../PacketBase.h"

namespace itch
{

ServerLoginAcceptedMessage::ServerLoginAcceptedMessage()
        : MessageBase ( ServerLoginAcceptedMessage::TYPE )
        , m_nSequenceNumber ( -1 )
{
}

bool ServerLoginAcceptedMessage::Load ( PacketBase& packet )
{
    if ( !packet.ReadInt ( m_nSequenceNumber, 10 ) )
        return false;
    return MessageBase::Load(packet);
}

void ServerLoginAcceptedMessage::LogMessage ( FILE* pLogFile ) const
{
    MessageBase::LogMessage ( pLogFile );
    fprintf ( pLogFile, SESSION_LOG_FIELD_DELIMITER "%d",
              m_nSequenceNumber );
}

static MessageBase *CREATE() {
    return new ServerLoginAcceptedMessage();
}
static MessageFactoryRegistry __registry ( ServerLoginAcceptedMessage::TYPE, CREATE );

}
