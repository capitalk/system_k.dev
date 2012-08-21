#ifndef CLIENTMARKETSNAPSHOTMESSAGE_H
#define CLIENTMARKETSNAPSHOTMESSAGE_H

#include "ClientCurrencyPairMessage.h"

namespace itch
{

class ClientMarketSnapshotMessage : public ClientCurrencyPairMessage
{
public:
    static const char TYPE = 'M';

    ClientMarketSnapshotMessage ( const char *pszCurrencyPair );
};

}

#endif // CLIENTMARKETSNAPSHOTMESSAGE_H
