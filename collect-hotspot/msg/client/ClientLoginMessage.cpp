#include "ClientLoginMessage.h"

#include <stdio.h>

#include "../PacketBase.h"

namespace itch
{

ClientLoginMessage::ClientLoginMessage ( const char* pszUsername, const char* pszPassword, bool bMarketDataUnsubscribe )
        : MessageBase ( TYPE )
        , m_strUsername ( pszUsername )
        , m_strPassword ( pszPassword )
        , m_bMarketDataUnsubscribe ( bMarketDataUnsubscribe )
{
}

bool ClientLoginMessage::Save ( PacketBase& packet ) const
{
    if (!MessageBase::Save(packet))
        return false;
    if ( !packet.WriteString ( m_strUsername, 40 ) )
        return false;
    if ( !packet.WriteString ( m_strPassword, 40 ) )
        return false;
    if ( !packet.WriteChar ( ( m_bMarketDataUnsubscribe ? 'T' : 'F' ) ) )
        return false;
    if ( !packet.WriteLong ( 0, 9 ) )                // Reserved
        return false;
    return true;
}

void ClientLoginMessage::LogMessage ( FILE* pLogFile ) const
{
    MessageBase::LogMessage ( pLogFile );
    fprintf ( pLogFile, SESSION_LOG_FIELD_DELIMITER "%s"
              SESSION_LOG_FIELD_DELIMITER "%s"
              SESSION_LOG_FIELD_DELIMITER "%c"
              SESSION_LOG_FIELD_DELIMITER "0",
              m_strUsername.c_str(),
              m_strPassword.c_str(),
              ( m_bMarketDataUnsubscribe ? 'T' : 'F' ) );
}

}
