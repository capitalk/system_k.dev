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


#define OB_VERSION_STRING "V3"

#define OB_NAME_LEN 128

namespace capk {

class KLimit;
class KOrder;
class KBook;
class KLimitComp;

struct KLimitComp : public std::binary_function<pKLimit, pKLimit, bool> {
    bool operator() (pKLimit const& a, pKLimit const& b);
};


class KBook {
  public:
    KBook(const char* name, size_t depth);

    ~KBook();

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

    friend std::ostream& operator<<(std::ostream& out, const KBook& b);

    uint32_t getOrderCountAtLimit(capk::Side_t buySell, double price);

    uint32_t getTotalVolumeAtLimit(capk::Side_t buySell, double price);

    void printLevels(capk::Side_t buySell);

    pKOrder getOrder(uint32_t orderId);

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
    int addBid(KOrder* bid, timespec eventTime, timespec exchSendTime);
    int addAsk(KOrder* ask, timespec eventTime, timespec exchSendTime);
    KOrderMap _orderMap;
    KTree _bidTree;
    KTree _askTree;
    char _name[OB_NAME_LEN];
    timespec _evtTime;
    timespec _exchSndTime;
    uint32_t _depth;

    KTree::iterator _findLimit(KTree&  tree, double price);
    KOrderMap::iterator _findOrderId(uint32_t orderId);
};

}  // namespace capk

#endif  // ORDER_BOOK_ORDER_BOOK_V2_ORDER_BOOK_H_
