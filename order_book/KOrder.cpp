#include "KOrder.h" 


KOrder::KOrder(uint32_t orderId, side_t buySell, double size, double price)
{
    _orderId = orderId;
    _buySell = buySell;
    _size = size;
    _price = price;
    //clock_gettime(CLOCK_MONOTONIC, &_eventTime);
    
}

KOrder:: ~KOrder()
{
}

std::ostream& 
operator<<(std::ostream& out, const KOrder& e) 
{
    out << "ORDER: " /*<< e._eventTime */<< ":" << e._orderId << ":" << e._buySell << ":" << e._size << ":" << e._price;
    return out;
}
