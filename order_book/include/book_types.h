#ifndef ORDER_BOOK_ORDER_BOOK_V2_BOOK_TYPES_H_
#define ORDER_BOOK_ORDER_BOOK_V2_BOOK_TYPES_H_

#include <boost/shared_ptr.hpp>

#include <map>
#include <set>
#include <list>

namespace capk {
    class level;
    class order;
    class order_book;
    class level_comparator;
    typedef boost::shared_ptr<level> plevel;
    typedef boost::shared_ptr<order> porder;
    typedef std::list<porder> Orders;
    typedef std::set<plevel, level_comparator> limit_tree;
    typedef std::map<uint32_t, porder> order_map;
}

#endif  // ORDER_BOOK_ORDER_BOOK_V2_BOOK_TYPES_H_
