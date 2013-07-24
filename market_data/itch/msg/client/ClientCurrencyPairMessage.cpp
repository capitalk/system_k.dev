#include "ClientCurrencyPairMessage.h"

#include <stdio.h>

#include "../PacketBase.h"

namespace itch
{

ClientCurrencyPairMessage::ClientCurrencyPairMessage ( char cMessageType, const char* pszCurrencyPair )
        : MessageBase ( cMessageType )
        , m_strCurrencyPair ( pszCurrencyPair )
{
}

bool ClientCurrencyPairMessage::Save ( PacketBase& packet ) const
{
    if ( !MessageBase::Save ( packet ) )
        return false;
    if ( !packet.WriteString ( m_strCurrencyPair, 7 ) )
        return false;
    return true;
}

void ClientCurrencyPairMessage::LogMessage ( FILE* pLogFile ) const
{
    MessageBase::LogMessage ( pLogFile );
    fprintf ( pLogFile, SESSION_LOG_FIELD_DELIMITER "%s",
              m_strCurrencyPair.c_str() );
}

}
