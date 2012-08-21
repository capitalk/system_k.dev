#include "ClientMarketSnapshotMessage.h"

namespace itch
{
ClientMarketSnapshotMessage::ClientMarketSnapshotMessage ( const char* pszCurrencyPair )
        : ClientCurrencyPairMessage ( TYPE, pszCurrencyPair )
{
}

}
