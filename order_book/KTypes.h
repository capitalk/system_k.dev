#ifndef CAPK_OB_TYPES
#define CAPK_OB_TYPES 1

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <set>
#include <map>
#include <list>


class KLimit;
class KOrder;
class KBook;
//#include "KLimit.h"
//#include "KOrder.h"
//#include "KBook.h"

struct KLimitComp;

//typedef uint32_t Volume;

typedef boost::shared_ptr<KLimit> pKLimit;

typedef std::set<pKLimit, KLimitComp> KTree;
                                            
typedef boost::shared_ptr<KOrder> pKOrder;

typedef std::list<pKOrder> Orders;

typedef std::map<uint32_t, pKOrder> KOrderMap;

typedef uint32_t orderId_t;

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

typedef Side side_t;

inline buy_sell_t sideConvert(char s) { if (s == '0') { return BUY; } else if(s == '1') { return SELL; }  }


#endif // CAPK_OB_TYPES
