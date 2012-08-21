#ifndef CLIENTMARKETDATASUBSCRIBEMESSAGE_H
#define CLIENTMARKETDATASUBSCRIBEMESSAGE_H

#include "ClientCurrencyPairMessage.h"

namespace itch
{

class ClientMarketDataSubscribeMessage : public ClientCurrencyPairMessage
{
public:
    static const char TYPE = 'A';

    ClientMarketDataSubscribeMessage ( const char* pszCurrencyPair );
};

}

#endif // CLIENTMARKETDATASUBSCRIBEMESSAGE_H
