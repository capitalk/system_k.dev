#include "order_book.h"

namespace capk {

std::ostream& operator<<(std::ostream& out, const capk::order_book& b) {
  capk::limit_tree::reverse_iterator bit = b._bidTree.rbegin();
  capk::limit_tree::iterator ait = b._askTree.begin();
  int i = 1;
  while (bit != b._bidTree.rend()) {
    out.unsetf(std::ios::fixed);
    out << static_cast<char>(capk::QUOTE) << ","
        << (*bit)->getUpdateTime() << ","
        << capk::BID << ","
        << i << ","
        << std::setiosflags(std::ios_base::fixed)
        << std::setprecision(5)
        << (*bit)->getPrice() << ","
        << std::setprecision(0)
        << (*bit)->getTotalVolume() << ",";
    capk::Orders::iterator it = (*bit)->_orders.begin();
    while (it != (*bit)->_orders.end()) {
      out.setf(std::ios::fixed);
      out.precision(0);
      out << (*it)->getSize();
      it++;

      if (it != (*bit)->_orders.end()) {
        out << ";";
      }
    }
    out << "\n";
    i++;
    bit++;
  }

  i = 1;
  while (ait != b._askTree.end()) {
    out.unsetf(std::ios::fixed);
    out << static_cast<char>(capk::QUOTE) << ","
        << (*ait)->getUpdateTime() << ","
        << capk::ASK << ","
        << i << ","
        << std::setiosflags(std::ios_base::fixed)
        << std::setprecision(5)
        << (*ait)->getPrice() << ","
        << std::setprecision(0)
        << (*ait)->getTotalVolume() << ",";
    capk::Orders::iterator it = (*ait)->_orders.begin();
    while (it != (*ait)->_orders.end()) {
      out.setf(std::ios::fixed);
      out.precision(0);
      out << (*it)->getSize();
      it++;
      if (it != (*ait)->_orders.end()) {
        out << ";";
      }
    }
    out << "\n";
    i++;
    ait++;
  }
  out.unsetf(std::ios::fixed);
  return out;
}

order_book::order_book(const char* name, size_t depth):_name({0}), _depth(depth) {
  if (name) {
    strncpy(_name, name, OB_NAME_LEN-1);
    _name[OB_NAME_LEN-1] = '\0';
  }
}

order_book::~order_book() {
}

porder order_book::getOrder(uint32_t orderId) {
  order_map::iterator it = _findOrderId(orderId);
  if (it == _orderMap.end()) {
    return porder();
  } else {
    return it->second;
  }
}

int order_book::add(uint32_t orderId,
               Side_t buySell,
               double size,
               double price,
               timespec evtTime,
               timespec exchSndTime) {
  _exchSndTime = exchSndTime;
  order* pOrd = new order(orderId, buySell, size, price);
  if (buySell == BID) {
    return addBid(pOrd, evtTime, exchSndTime);
  }
  if (buySell == ASK) {
    return addAsk(pOrd, evtTime, exchSndTime);
  }
  return 0;
}

int order_book::addBid(order* bid, timespec evtTime, timespec exchSndTime) {
  if (bid) {
    porder pBid(bid);
    limit_tree::iterator it = _findLimit(_bidTree, pBid->getPrice());
    if (it != _bidTree.end()) {
      (*it)->addOrder(pBid);
      (*it)->setUpdateTime(evtTime);
      this->_evtTime = evtTime;
    } else {
      level* newLimit = new level(pBid->getPrice());
      newLimit->setUpdateTime(evtTime);
      newLimit->addOrder(pBid);
      this->_evtTime = evtTime;
      _bidTree.insert(plevel(newLimit));
    }
    uint32_t oid = bid->getOrderId();
    _orderMap[oid] = pBid;
    return 1;
  }
  return 0;
}

int order_book::addAsk(order* ask, timespec evtTime, timespec exchSndTime) {
  if (ask) {
    porder pAsk(ask);
    limit_tree::iterator it = _findLimit(_askTree, ask->getPrice());
    if (it != _askTree.end()) {
      (*it)->addOrder(pAsk);
      (*it)->setUpdateTime(evtTime);
      _evtTime = evtTime;
    } else {
      level* newLimit = new level(ask->getPrice());
      newLimit->setUpdateTime(evtTime);
      newLimit->addOrder(pAsk);
      _evtTime = evtTime;
      _askTree.insert(plevel(newLimit));
    }
    uint32_t oid = ask->getOrderId();
    _orderMap[oid] = pAsk;
    return 1;
  }
  return 0;
}

void order_book::clear() {
  _orderMap.clear();
  _bidTree.clear();
  _askTree.clear();
}

int order_book::remove(uint32_t orderId, timespec evtTime, timespec exchSndTime) {
  double limit;
  Side_t bidAsk;
  _exchSndTime = exchSndTime;
  order_map::iterator oit =  _findOrderId(orderId);
  if (oit != _orderMap.end()) {
    limit = oit->second->getPrice();
    bidAsk = oit->second->getBuySell();

    if (bidAsk == BID) {
      limit_tree::iterator bit = _findLimit(_bidTree, limit);
      if (bit != _bidTree.end()) {
        _orderMap.erase(orderId);
        int removeOk = (*bit)->removeOrder(orderId);
        assert(removeOk == 1);
        _evtTime = evtTime;
        (*bit)->setUpdateTime(_evtTime);
        // TODO(tkaria@capitalkpartners.com) remove the limit ONLY if qty = 0
        //  OK - but should just ignore in the get best method - maybe faster?
        if ((*bit)->getOrderCount() == 0) {
          _bidTree.erase(bit);
        }
        return 1;
      }
      return 0;
    }
    if (bidAsk == ASK) {
      limit_tree::iterator ait = _findLimit(_askTree, limit);
      if (ait != _bidTree.end()) {
        _orderMap.erase(orderId);
        int removeOk = (*ait)->removeOrder(orderId);
        assert(removeOk == 1);
        _evtTime = evtTime;
        (*ait)->setUpdateTime(_evtTime);
        // TODO(tkaria@capitalkpartners.com) remove the limit ONLY if qty = 0
        //  OK - but should just ignore in the get best method - maybe faster?
        if ((*ait)->getOrderCount() == 0) {
          _askTree.erase(ait);
        }
        return 1;
      }
      return 0;
    }
  }
  return 0;
}

int order_book::modify(uint32_t orderId,
                  double size,
                  timespec evtTime,
                  timespec exchSndTime) {
// KTK - this is inefficient since we need to use the orderId in
// the map to get to the limit price. Then use the limit price
// to search for the order in the tree
  _exchSndTime = exchSndTime;
  order_map::iterator it = _findOrderId(orderId);
  if (it != _orderMap.end()) {
// TODO(tkaria@capitalkpartners.com) Modify size directly in order
// and have order notify its parent
// TODO(tkaria@capitalkpartners.com) if price is modified does
// queue position get lost? - KTK - irrelevant since price is not a param!!!
// TODO(tkaria@capitalkpartners.com) ask Hotspot
    double price = it->second->getPrice();
    Side_t bs = it->second->getBuySell();
    limit_tree::iterator lit;
    if (bs == BID) {
      lit  = _findLimit(_bidTree, price);
      if (lit == _bidTree.end()) {
        return 0;
      }
    }
    if (bs == ASK) {
      lit  = _findLimit(_askTree, price);
      if (lit == _askTree.end()) {
        return 0;
      }
    }
    (*lit)->modifyOrder(orderId, size);
    (*lit)->setUpdateTime(_evtTime);
    _evtTime = evtTime;
    return 1;
  }
  return 0;
}

uint32_t order_book::getTotalVolumeAtLimit(Side_t buySell, double price) {
  limit_tree::iterator it;
  if (buySell == BID) {
    it = _findLimit(_bidTree, price);
    if (it != _bidTree.end()) {
#ifdef DEBUG
      std::cerr << "BID VOLUME @ "
          << price
          << ": "
          << (*it)->getTotalVolume()
          << std::endl;
#endif
      return (*it)->getTotalVolume();
    }
  }
  if (buySell == ASK) {
    it = _findLimit(_askTree, price);
    if (it != _askTree.end()) {
#ifdef DEBUG
      std::cerr << "ASK VOLUME @ "
          << price
          << ": "
          << (*it)->getTotalVolume()
          << std::endl;
#endif
      return (*it)->getTotalVolume();
    }
  }
#ifdef DEBUG
  std::cerr << "NO LIMIT: " << buySell << " @ " << price << std::endl;
#endif
  return 0;
}

uint32_t order_book::getOrderCountAtLimit(Side_t buySell, double price) {
  limit_tree::iterator it;
  if (buySell == BID) {
    it = _findLimit(_bidTree, price);
    if (it != _bidTree.end()) {
#ifdef DEBUG
      std::cerr << "BIDS @ "
          << price
          << ": "
          << (*it)->getOrderCount()
          << std::endl;
#endif
      return (*it)->getOrderCount();
    }
  }
  if (buySell == ASK) {
    it = _findLimit(_askTree, price);
    if (it != _askTree.end()) {
#ifdef DEBUG
      std::cerr << "ASKS @ "
          << price
          << ": "
          << (*it)->getOrderCount()
          << std::endl;
#endif
      return (*it)->getOrderCount();
    }
  }
  return 0;
}

double order_book::bestPriceVolume(Side_t buySell) {
  double p = bestPrice(buySell);
  return getTotalVolumeAtLimit(buySell, p);
}

double order_book::bestPrice(Side_t buySell) {
  // printLevels(buySell);
  if (buySell == BID) {
    //  TODO(tkaria@capitalkpartners.com) - NOTE this does not support negative prices
    limit_tree::reverse_iterator bit = _bidTree.rbegin();
    if (bit == _bidTree.rend()) {
      return NO_BID;
    }
    return (*bit)->getPrice();
  }
  if (buySell == ASK) {
    //  TODO(tkaria@capitalkpartners.com) - NOTE this does not support negative prices
    limit_tree::iterator ait = _askTree.begin();
    if (ait == _askTree.end()) {
      return NO_ASK;
    }
    return (*ait)->getPrice();
  }
  //  This should never happen
  return (-999999999);
}

void order_book::printLevels(Side_t buySell) {
  if (buySell == BID) {
    limit_tree::reverse_iterator bit = _bidTree.rbegin();
    while (bit != _bidTree.rend()) {
      std::cout << "BID: " << *(*bit) << "\n";
      bit++;
    }
  }
  if (buySell == ASK) {
    limit_tree::reverse_iterator ait = _askTree.rbegin();
    while (ait != _askTree.rend()) {
      std::cout << "ASK: " << *(*ait) << "\n";
      ait++;
    }
  }
}

limit_tree::iterator order_book::_findLimit(limit_tree& tree, double price) {
  limit_tree::iterator it;
  level* tmpLimit = new level(price);
  it = tree.find(plevel(tmpLimit));
  return it;
}

order_map::iterator order_book::_findOrderId(uint32_t orderId) {
  order_map::iterator it = _orderMap.find(orderId);
  // assert(it != _orderMap.end());
  return it;
}

void order_book::dbg() {
  order_map::iterator it = _orderMap.begin();
  std::cerr << "ORDER MAP\n";
  while (it != _orderMap.end()) {
    std::cerr << it->first << " : " << *(it->second) << "\n";
    it++;
  }
}
}  // namespace capk

