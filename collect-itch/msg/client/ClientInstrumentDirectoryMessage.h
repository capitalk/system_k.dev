#ifndef CLIENTINSTRUMENTDIRECTORYMESSAGE_H
#define CLIENTINSTRUMENTDIRECTORYMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class ClientInstrumentDirectoryMessage : public MessageBase
{
public:
    static const char TYPE = 'I';

    ClientInstrumentDirectoryMessage ();
};

}

#endif // CLIENTINSTRUMENTDIRECTORYMESSAGE_H
