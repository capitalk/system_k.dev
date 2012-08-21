#include "ClientMarketDataUnsubscribeMessage.h"

namespace itch
{

ClientMarketDataUnsubscribeMessage::ClientMarketDataUnsubscribeMessage ( const char* pszCurrencyPair )
        : ClientCurrencyPairMessage ( TYPE, pszCurrencyPair )
{
}

}
