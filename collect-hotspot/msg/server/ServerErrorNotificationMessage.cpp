#include "ServerErrorNotificationMessage.h"

#include <stdio.h>

#include "../MessageFactory.h"
#include "../PacketBase.h"

namespace itch
{

ServerErrorNotificationMessage::ServerErrorNotificationMessage()
        : MessageBase ( TYPE )
        , m_strExplanation ( "" )
{
}

bool ServerErrorNotificationMessage::Load ( PacketBase& packet )
{
    if ( !packet.ReadString ( m_strExplanation, 100 ) )
        return false;
    return MessageBase::Load(packet);
}

void ServerErrorNotificationMessage::LogMessage ( FILE* pLogFile ) const
{
    MessageBase::LogMessage ( pLogFile );
    fprintf ( pLogFile, SESSION_LOG_FIELD_DELIMITER "%s",
              m_strExplanation.c_str() );
}

static MessageBase *CREATE() {
    return new ServerErrorNotificationMessage();
}
static MessageFactoryRegistry __registry ( ServerErrorNotificationMessage::TYPE, CREATE );

}
