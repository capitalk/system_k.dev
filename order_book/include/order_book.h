#ifndef ORDER_BOOK_ORDER_BOOK_V2_ORDER_BOOK_H_
#define ORDER_BOOK_ORDER_BOOK_V2_ORDER_BOOK_H_

#include <stdint.h>
#include <time.h>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <set>
#include <map>
#include <functional>

#include "./limit.h"
#include "./order.h"
#include "./book_types.h"
#include "utils/types.h"
#include "utils/time_utils.h"
#include "utils/constants.h"


#define OB_VERSION_STRING "0.0.1"

#define OB_NAME_LEN 128

namespace capk {

class level;
class order;
class order_book;
class level_comparator;

struct level_comparator : public std::binary_function<plevel, plevel, bool> {
    bool operator() (const plevel& a, const plevel& b);
};


class order_book {
  public:
    order_book(const char* name, size_t depth);

    ~order_book();

    int add(uint32_t orderId,
                    capk::Side_t buySell,
                    double size,
                    double price,
                    timespec evtTime,
                    timespec exchSendTime);

    int remove(uint32_t orderId,
                       timespec evtTime,
                       timespec exchSendTime);

    int modify(uint32_t orderId,
                       double size,
                       timespec evtTime,
                       timespec exchSendTime);

    inline const char* getName() const {
        return _name;
    };

    inline uint32_t getDepth() const {
        return _depth;
    };

    double bestPrice(capk::Side_t buySell);

    double bestPriceVolume(capk::Side_t buySell);

    friend std::ostream& operator<<(std::ostream& out, const order_book& b);

    uint32_t getOrderCountAtLimit(capk::Side_t buySell, double price);

    uint32_t getTotalVolumeAtLimit(capk::Side_t buySell, double price);

    void printLevels(capk::Side_t buySell);

    porder getOrder(uint32_t orderId);

    const char* getOutputVersionString() {
        return OB_VERSION_STRING;
    }
    const timespec getEventTime() {
        return _evtTime;
    }
    const timespec getExchangeSendTime() {
        return _exchSndTime;
    }
    void dbg();
    void clear();

  private:
    int addBid(order* bid, timespec eventTime, timespec exchSendTime);
    int addAsk(order* ask, timespec eventTime, timespec exchSendTime);
    order_map _orderMap;
    limit_tree _bidTree;
    limit_tree _askTree;
    char _name[OB_NAME_LEN];
    timespec _evtTime;
    timespec _exchSndTime;
    uint32_t _depth;

    limit_tree::iterator _findLimit(limit_tree&  tree, double price);
    order_map::iterator _findOrderId(uint32_t orderId);
};

}  // namespace capk

#endif  // ORDER_BOOK_ORDER_BOOK_V2_ORDER_BOOK_H_
