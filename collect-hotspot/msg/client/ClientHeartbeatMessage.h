#ifndef CLIENTHEARTBEATMESSAGE_H
#define CLIENTHEARTBEATMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class ClientHeartbeatMessage : public MessageBase
{
public:
    static const char TYPE = 'R';

    ClientHeartbeatMessage();
};

}

#endif // CLIENTHEARTBEATMESSAGE_H
