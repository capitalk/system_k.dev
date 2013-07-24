#include "limit.h"

namespace capk {

std::ostream& operator<<(std::ostream& out, const capk::KLimit& limit) {
  out <<  limit._price
      << ","
      << limit._totalVolume
      << ","
      << limit.getOrderCount();
  return out;
}

KLimit::KLimit(double price, uint32_t totalVolume) {
  _price = price;
  _totalVolume = totalVolume;
}

KLimit::~KLimit() {
}

int KLimit::addOrder(pKOrder order) {
  uint32_t orderId = order->getOrderId();
#ifdef DEBUG
  std::cerr << "KLimit::addOrder - Orders size @ "
      << _price
      << ": "
      << _orders.size() << std::endl;
#endif
  Orders::iterator it = _findOrder(orderId);
  if (it != _orders.end()) {
#ifdef DEBUG
    std::cerr << "Add order called for duplicate order" << std::endl;
#endif
    return 0;
  }
  if (order) {
    _orders.push_back(pKOrder(order));
    _addVolume(order->getSize());
    return 1;
  }
  return 0;
}

int KLimit::removeOrder(uint32_t orderId) {
  return _removeOrder(orderId);
}

int KLimit::modifyOrder(uint32_t orderId, double size) {
  Orders::iterator it = _findOrder(orderId);
  if (it != _orders.end()) {
    // Modify total volume
    _removeVolume((*it)->getSize());
    (*it)->setSize(size);
    _addVolume(size);
    return 1;
  }
  return 0;
}

/*
void
KLimit::printAllOrders()
{
    Orders::iterator it = _orders.begin();
    std::cerr << "LIMIT:" << _price << std::endl;
    for (it = _orders.begin(); it != _orders.end(); it++) {
        std::cerr << "\tORDER:" << *(*it) << std::endl;
    }
}
*/

Orders::iterator KLimit::_findOrder(uint32_t orderId) {
  // TODO(tkaria@capitalkpartners.com) linear search
  Orders::iterator it = _orders.begin();
  for (it = _orders.begin(); it != _orders.end(); it++) {
    if ((*it)->getOrderId() == orderId) {
      // TODO(tkaria@capitalkpartners.com) invalidated iterators if MT
      break;
    }
  }
  return it;
}

int KLimit::_removeOrder(uint32_t orderId) {
  if (_orders.size() == 0) {
    return 1;
  }
  Orders::iterator it = _findOrder(orderId);
  if (it != _orders.end()) {
    _removeVolume((*it)->getSize());
    _orders.erase(it);
    return 1;
  } else {
    std::cerr << "Can't find order: "
        << orderId
        << " at limit: "
        << _price
        << "\n";
  }
  return 0;
}

bool KLimitComp::operator() (pKLimit const& a, pKLimit const& b) {
  return (*a) < (*b);
}
}  // namesapce capk

