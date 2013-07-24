#ifndef ORDER_BOOK_ORDER_BOOK_V2_ORDER_H_
#define ORDER_BOOK_ORDER_BOOK_V2_ORDER_H_

#include <time.h>


#include <string>
#include <map>

#include "limit.h"
#include "utils/time_utils.h"
#include "order_book.h"
#include "utils/types.h"
#include "book_types.h"

namespace capk {

class KOrder {
  public:
    KOrder(uint32_t orderId, Side_t buySell, double size, double price);

    ~KOrder();

    inline void setSize(double size) {
      _size = size;
    }

    inline void setPrice(double price) {
      _price = price;
    }

    inline void setOrderId(uint32_t orderId) {
      _orderId = orderId;
    }

    inline void setBuySell(Side_t buySell) {
      _buySell = buySell;
    }

    inline double getSize() const {
      return _size;
    }

    inline double getPrice() const {
      return _price;
    }

    inline uint32_t getOrderId() const {
      return _orderId;
    }

    inline Side_t getBuySell() const {
      return _buySell;
    }

    inline Side_t getSide() const {
      return _buySell;
    }

    bool operator<(const KOrder& rhs) const {
      return (this->getPrice() < rhs.getPrice());
    }

    friend std::ostream& operator<<(std::ostream& out, const KOrder& e);

  private:
    uint32_t _orderId;
    Side_t _buySell;
    double _size;
    double _price;
};

/*
struct KOrderComp : public std::binary_function<pKOrder, pKOrder, bool>
{
    bool operator() (pKOrder const& a, pKOrder const& b) {
        return (*a) < (*b);
    }
}
*/

}  // namespace capk

#endif  // ORDER_BOOK_ORDER_BOOK_V2_ORDER_H_
