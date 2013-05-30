#include "order.h"

namespace capk {

std::ostream& operator<<(std::ostream& out, const capk::KOrder& e) {
  out << "ORDER: "
      << ":"
      << e._orderId
      << ":"
      << e._buySell
      << ":"
      << e._size
      << ":"
      << e._price;
  return out;
}

KOrder::KOrder(uint32_t orderId,
               capk::Side_t buySell,
               double size,
               double price) {
  _orderId = orderId;
  _buySell = buySell;
  _size = size;
  _price = price;
}

KOrder:: ~KOrder() {
}

}  // namespace capk

