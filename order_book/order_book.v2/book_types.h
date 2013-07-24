#ifndef ORDER_BOOK_ORDER_BOOK_V2_BOOK_TYPES_H_
#define ORDER_BOOK_ORDER_BOOK_V2_BOOK_TYPES_H_

#include <boost/shared_ptr.hpp>

#include <map>
#include <set>
#include <list>

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

#endif  // ORDER_BOOK_ORDER_BOOK_V2_BOOK_TYPES_H_
