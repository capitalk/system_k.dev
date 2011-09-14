#ifndef CLIENTTICKERSUBSCRIBEMESSAGE_H
#define CLIENTTICKERSUBSCRIBEMESSAGE_H

#include "ClientCurrencyPairMessage.h"

namespace itch
{

class ClientTickerSubscribeMessage : public ClientCurrencyPairMessage
{
public:
    static const char TYPE = 'T';

    ClientTickerSubscribeMessage ( const char *pszCurrencyPair );
};

}

#endif // CLIENTTICKERSUBSCRIBEMESSAGE_H
