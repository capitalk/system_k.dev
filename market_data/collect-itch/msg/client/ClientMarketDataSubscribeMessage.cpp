#include "ClientMarketDataSubscribeMessage.h"

namespace itch
{

ClientMarketDataSubscribeMessage::ClientMarketDataSubscribeMessage ( const char* pszCurrencyPair )
        : ClientCurrencyPairMessage ( TYPE, pszCurrencyPair )
{
}

}
