#ifndef CLIENTLOGOUTMESSAGE_H
#define CLIENTLOGOUTMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class ClientLogoutMessage : public MessageBase
{
public:
    static const char TYPE = 'O';

    ClientLogoutMessage();
};

}

#endif // CLIENTLOGOUTMESSAGE_H
