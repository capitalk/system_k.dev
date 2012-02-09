#include "ClientTickerSubscribeMessage.h"


namespace itch
{

ClientTickerSubscribeMessage::ClientTickerSubscribeMessage ( const char* pszCurrencyPair )
        : ClientCurrencyPairMessage ( TYPE, pszCurrencyPair )
{
}

}
