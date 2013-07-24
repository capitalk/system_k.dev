#ifndef SERVERHEARTBEATMESSAGE_H
#define SERVERHEARTBEATMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class ServerHeartbeatMessage : public MessageBase
{
public:
    static const char TYPE = 'H';

    ServerHeartbeatMessage();
};

}

#endif // SERVERHEARTBEATMESSAGE_H
