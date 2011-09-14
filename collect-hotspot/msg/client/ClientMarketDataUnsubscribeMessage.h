#ifndef MARKETDATAUNSUBSCRIBEMESSAGE_H
#define MARKETDATAUNSUBSCRIBEMESSAGE_H

#include "ClientCurrencyPairMessage.h"

namespace itch
{

class ClientMarketDataUnsubscribeMessage : public ClientCurrencyPairMessage
{
public:
    static const char TYPE = 'B';

    ClientMarketDataUnsubscribeMessage ( const char* pszCurrencyPair );
};

}

#endif // MARKETDATAUNSUBSCRIBEMESSAGE_H
