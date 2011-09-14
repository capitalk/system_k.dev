#ifndef SERVERINSTRUMENTDIRECTORYMESSAGE_H
#define SERVERINSTRUMENTDIRECTORYMESSAGE_H

#include <list>

#include "../MessageBase.h"

namespace itch
{

class ServerInstrumentDirectoryMessage : public MessageBase
{
public:
    static const char TYPE = 'R';

    ServerInstrumentDirectoryMessage ();

    bool Load ( PacketBase& packet );

    void LogMessage ( FILE *pLogFile ) const;

protected:
    std::list<std::string> m_lstCurrencyPairs;
};

}

#endif // SERVERINSTRUMENTDIRECTORYMESSAGE_H
