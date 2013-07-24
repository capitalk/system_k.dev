#ifndef CLIENTTICKERUNSUBSCRIBEMESSAGE_H
#define CLIENTTICKERUNSUBSCRIBEMESSAGE_H

#include "ClientCurrencyPairMessage.h"

namespace itch
{

class ClientTickerUnsubscribeMessage : public ClientCurrencyPairMessage
{
public:
    static const char TYPE = 'U';

    ClientTickerUnsubscribeMessage ( const char *pszCurrencyPair );
};

}

#endif // CLIENTTICKERUNSUBSCRIBEMESSAGE_H
