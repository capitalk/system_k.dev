#include "MessageBase.h"

#include <stdio.h>

#include "../Logging.h"
#include "PacketBase.h"

namespace itch
{

MessageBase::MessageBase ( char cMessageType, bool bHeartbeat, bool bEndOfSession )
        : m_bEndOfSession ( bEndOfSession )
        , m_bHeartbeat ( bHeartbeat )
        , m_cMessageType ( cMessageType )
{
}

MessageBase::~MessageBase()
{
}

bool MessageBase::Save ( PacketBase& packet ) const
{
    // Note that this Save() function implementation does
    // NOT add TLF character at the end of the packet. This
    // is due to the present design of class hierarchy. To
    // accomodate the protocol requirements, the TLF
    // character is provided automatically by Socket.Send()
    // routine.
    if (!packet.WriteChar(m_cMessageType))
        return false;
    return true;
}

bool MessageBase::Load ( PacketBase& packet )
{
    // Unlike Save(), the base Load() function is called
    // *after* the derived class(es) have processed the
    // packet contents. Therefore, all that remains is to
    // check whether the packet is properly terminated with
    // TLF. However, because this is a base class for all
    // packets, even those which are not terminated with
    // TLF characters (such as hotspot payloads), and we
    // are pretty sure Socket::RecvPacket() would not
    // consider anything a packet unless it has the TLF
    // character at the end of it, we'll simply ignore
    // whatever remains in the packet. For now.
    return true;
}

void MessageBase::LogMessage(FILE* pLogFile) const
{
    fprintf ( pLogFile, "%c", m_cMessageType );
}

}
