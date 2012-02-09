#include "ClientTickerUnsubscribeMessage.h"

namespace itch
{

ClientTickerUnsubscribeMessage::ClientTickerUnsubscribeMessage ( const char* pszCurrencyPair )
        : ClientCurrencyPairMessage ( TYPE, pszCurrencyPair )
{
}

}
