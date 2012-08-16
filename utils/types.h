#ifndef CAPK_OB_TYPEDEFS
#define CAPK_OB_TYPEDEFS

#include <stdint.h>

namespace capk
{
typedef enum {
    BUY=0,
    SELL=1
} buy_sell_t;


enum TickType {
    QUOTE = 'Q',
    TRADE  = 'T'
};

enum Side {
    BID = 0,
    ASK = 1
};
typedef Side Side_t;

typedef uint32_t orderId_t;

typedef uint32_t msg_t;
typedef uint32_t venue_id_t;


}
/*
namespace capk {
    class KLimit;
    class KOrder;
    class KBook;
    class KLimitComp;
    typedef boost::shared_ptr<KLimit> pKLimit;
    typedef boost::shared_ptr<KOrder> pKOrder;
    typedef std::list<pKOrder> Orders;
    typedef std::set<pKLimit, KLimitComp> KTree;
    typedef std::map<uint32_t, pKOrder> KOrderMap;
}
*/
#endif // CAPK_OB_TYPEDEFS
