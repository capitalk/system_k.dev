#include "ServerHeartbeatMessage.h"

#include "../MessageFactory.h"

namespace itch
{

ServerHeartbeatMessage::ServerHeartbeatMessage()
        : MessageBase ( TYPE, true )
{
}

static MessageBase *CREATE() {
    return new ServerHeartbeatMessage();
}
static MessageFactoryRegistry __registry ( ServerHeartbeatMessage::TYPE, CREATE );

}
