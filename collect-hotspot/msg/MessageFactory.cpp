#include "MessageFactory.h"

#include "../Logging.h"
#include "server/ServerUnsupportedMessage.h"
#include "PacketBase.h"

namespace itch
{

std::map< std::string, std::map<char, CreateNew> > *MessageFactory::m_pMessageCreators;

MessageBase *MessageFactory::ReceiveMessage ( Socket* pSocket, int nTimeoutMilliseconds )
{
    if ( !pSocket->RecvMessage ( nTimeoutMilliseconds ) )
        return NULL;

    PacketBase packet;
    if (!pSocket->RecvPacket ( packet ) )
        return NULL;

    return ParseMessage ( MESSAGE_DEFAULT_NAMESPACE, packet );
}

MessageBase* MessageFactory::ParseMessage ( const char* pszNamespace, PacketBase& packet )
{
    char cMessageType;
    if (!packet.ReadChar(cMessageType))
        return NULL;

    MessageBase *pMessage = MessageFactory::CreateMessage ( pszNamespace, cMessageType );
    if ( pMessage == NULL )
    {
        LOG ( "message type '%c' is not supported", cMessageType );
        pMessage = new ServerUnsupportedMessage ( cMessageType );
    }

    if ( pMessage->Load ( packet ) )
        return pMessage;

    LOG ( "message parser returned an error" );
    if (dynamic_cast<ServerUnsupportedMessage *>(pMessage) == NULL)
    {
        // Let's try camming it into the unsupported message structure
        // rather than discarding it wholesale
        delete pMessage;

        LOG("trying to cram the packet into the unsupported message type");
        pMessage = new ServerUnsupportedMessage ( cMessageType );
        packet.ResetReadPosition ( 1 );
        if ( pMessage->Load ( packet ) )
            return pMessage;
    }

    // Otherwise, it appears that ServerUnsupportedMessage was already used
    // and did not like the packet contents at all.
    delete pMessage;
    return NULL;
}

void MessageFactory::RegisterMessageType ( const char *pszNamespace, char cMessageType, CreateNew callback )
{
    LOG ( "MessageFactory registered message type '%c'", cMessageType );
    GetMessageCreators()[std::string(pszNamespace)][cMessageType] = callback;
}

MessageBase *MessageFactory::CreateMessage ( const char *pszNamespace, char cMessageType )
{
    std::map< std::string, std::map<char, CreateNew> >::const_iterator nsIt =
        GetMessageCreators().find( std::string(pszNamespace) );
    if (nsIt == GetMessageCreators().end() )
        return NULL;

    std::map<char, CreateNew>::const_iterator it = nsIt->second.find ( cMessageType );
    if ( it == nsIt->second.end() )
        return NULL;

    return it->second();
}

std::map< std::string, std::map<char, CreateNew> >& MessageFactory::GetMessageCreators()
{
    // Unfortunately, this memory leak is a necessary evil. Static inter-module initialization
    // rears its ugly head when static instance used instead of the static pointer.
    if (m_pMessageCreators == NULL)
        m_pMessageCreators = new std::map< std::string, std::map<char, CreateNew> >();
    return *m_pMessageCreators;
}

}
