#include "ServerLoginRejectedMessage.h"

#include <stdio.h>

#include "../MessageFactory.h"
#include "../PacketBase.h"

namespace itch
{

ServerLoginRejectedMessage::ServerLoginRejectedMessage()
        : MessageBase ( ServerLoginRejectedMessage::TYPE )
        , m_strReason ( "" )
{
}

bool ServerLoginRejectedMessage::Load ( PacketBase& packet )
{
    if ( !packet.ReadString ( m_strReason, 20 ) )
        return false;
    return MessageBase::Load(packet);
}

void ServerLoginRejectedMessage::LogMessage ( FILE* pLogFile ) const
{
    MessageBase::LogMessage ( pLogFile );
    fprintf ( pLogFile, SESSION_LOG_FIELD_DELIMITER "%s",
              m_strReason.c_str() );
}

static MessageBase *CREATE() {
    return new ServerLoginRejectedMessage();
}
static MessageFactoryRegistry __registry ( ServerLoginRejectedMessage::TYPE, CREATE );

}
