#ifndef MESSAGEFACTORY_H
#define MESSAGEFACTORY_H

#include <map>

#include "../Socket.h"
#include "MessageBase.h"

#define MESSAGE_ITCH_NAMESPACE "ITCH"
#define MESSAGE_HOTSPOTFX_NAMESPACE "HotspotFX"
#define MESSAGE_DEFAULT_NAMESPACE MESSAGE_ITCH_NAMESPACE

namespace itch
{

typedef MessageBase * ( * CreateNew ) ();

class MessageFactory
{
public:
    static MessageBase *ReceiveMessage ( Socket *pSocket, int nTimeoutMilliseconds );
    static MessageBase *ParseMessage( const char *pszNamespace, PacketBase& packet );

    static void RegisterMessageType ( const char *pszNamespace, char cMessageType, CreateNew callback );

private:
    static std::map< std::string, std::map<char, CreateNew> > *m_pMessageCreators;

    MessageFactory() {}
    MessageFactory(const MessageFactory& other) {}

    static MessageBase *CreateMessage ( const char *pszNamespace, char cMessageType );
    static std::map< std::string, std::map<char, CreateNew> >& GetMessageCreators();
};

class MessageFactoryRegistry
{
public:
    inline MessageFactoryRegistry ( const char *pszNamespace, char cMessageType, CreateNew callback )
    {
        MessageFactory::RegisterMessageType ( pszNamespace, cMessageType, callback );
    }

    inline MessageFactoryRegistry ( char cMessageType, CreateNew callback )
    {
        MessageFactory::RegisterMessageType ( MESSAGE_DEFAULT_NAMESPACE, cMessageType, callback );
    }
};

}

#endif // MESSAGEFACTORY_H
