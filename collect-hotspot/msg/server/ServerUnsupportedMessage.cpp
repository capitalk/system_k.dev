#include "ServerUnsupportedMessage.h"

#include <stdio.h>

#include "../PacketBase.h"

namespace itch
{

ServerUnsupportedMessage::ServerUnsupportedMessage ( char cMessageType )
        : MessageBase ( cMessageType )
{
}

bool ServerUnsupportedMessage::Load ( PacketBase& packet )
{
    if ( !packet.ReadUntilTLF ( m_strBody ) )
        return false;
    return MessageBase::Load(packet);
}

void ServerUnsupportedMessage::LogMessage ( FILE* pLogFile ) const
{
    MessageBase::LogMessage ( pLogFile );
    fprintf ( pLogFile, SESSION_LOG_FIELD_DELIMITER "%s",
              m_strBody.c_str() );
}

}
