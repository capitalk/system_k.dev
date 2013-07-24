#include "ServerInstrumentDirectoryMessage.h"

#include <stdio.h>

#include "../MessageFactory.h"
#include "../PacketBase.h"

namespace itch
{

ServerInstrumentDirectoryMessage::ServerInstrumentDirectoryMessage()
        : MessageBase ( TYPE )
{
}

bool ServerInstrumentDirectoryMessage::Load ( PacketBase& packet )
{
    m_lstCurrencyPairs.clear();

    int nCount ;
    if ( !packet.ReadInt ( nCount, 4 ) )
        return false;

    while ( --nCount >= 0 )
    {
        std::string strCurrencyPair;
        if ( !packet.ReadString ( strCurrencyPair, 7 ) )
            return false;
        m_lstCurrencyPairs.push_back ( strCurrencyPair );
    }

    return MessageBase::Load(packet);
}

void ServerInstrumentDirectoryMessage::LogMessage ( FILE* pLogFile ) const
{
    MessageBase::LogMessage ( pLogFile );
    fprintf ( pLogFile, SESSION_LOG_FIELD_DELIMITER "%ld",
              m_lstCurrencyPairs.size() );
    for ( std::list<std::string>::const_iterator it = m_lstCurrencyPairs.begin(); it != m_lstCurrencyPairs.end(); it++ )
    {
        fprintf ( pLogFile, SESSION_LOG_FIELD_DELIMITER "%s",
                  ( *it ).c_str() );
    }
}

static MessageBase *CREATE() {
    return new ServerInstrumentDirectoryMessage();
}
static MessageFactoryRegistry __registry ( ServerInstrumentDirectoryMessage::TYPE, CREATE );
}
