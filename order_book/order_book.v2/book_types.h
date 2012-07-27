#ifndef CAPK_BOOK_TYPES
#define CAPK_BOOK_TYPES

#include <map>
#include <set>
#include <list>
#include <boost/shared_ptr.hpp>

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

#endif //CAPK_BOOK_TYPES
