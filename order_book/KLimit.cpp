#include "KLimit.h"


KLimit::KLimit(double price, uint32_t totalVolume)
{
    _price = price;
    _totalVolume = totalVolume;
}

KLimit::~KLimit()
{
}
    //KLimit* parent;
    //KLimit* leftChild;
    //KLimit* rightChild; 
    //KOrder* headOrder;
    //KOrder* tailOrder;
    
int 
KLimit::addOrder(pKOrder order)
{
    uint32_t orderId = order->getOrderId(); 
#ifdef DEBUG
        std::cerr << "KLimit::addOrder - Orders size @ " << _price << ": " << _orders.size() << std::endl;
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
        //_timeUpdated = order->getEventTime();
        return 1;
    }
    return 0;
}
/*
int 
KLimit::removeOrder(pKOrder order)
{
    if (order) {
        _removeOrder(order->getOrderId());
    }
    return 0;
}
*/

int
KLimit::removeOrder(uint32_t orderId)
{
    return _removeOrder(orderId); 
}

int 
KLimit::modifyOrder(uint32_t orderId, double size)
{
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
Orders::iterator 
KLimit::_findOrder(uint32_t orderId) 
{
    // KTK - TODO - fix evil linear search
    Orders::iterator it = _orders.begin();
    for (it = _orders.begin(); it != _orders.end(); it++) {
        if ((*it)->getOrderId() == orderId) {
            // KTK TODO - be careful of invalidated iterators here
            break;
        }
    } 
    return it;
}

int
KLimit::_removeOrder(uint32_t orderId)
{
    if (_orders.size() == 0) {
       return 1; 
    }
    Orders::iterator it = _findOrder(orderId);
    if (it != _orders.end()) {
            _removeVolume((*it)->getSize());
            _orders.erase(it);
            return 1;
    }
    else {
        std::cerr << "Can't find order: " << orderId << " at limit: " << _price << "\n"; 
    }
    return 0;
}

std::ostream&
operator << (std::ostream& out, const KLimit& limit) 
{
    out <<  limit._price << "," << limit._totalVolume << "," << limit.getOrderCount();
    return out;
}

