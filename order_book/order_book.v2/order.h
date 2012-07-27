#ifndef CAPK_ORDER
#define CAPK_ORDER

#include <string>
#include <map>

#include <time.h>

#include <boost/date_time/posix_time/posix_time.hpp> // for ptime

#include "utils/time_utils.h"
#include "limit.h"
#include "order_book.h"
#include "utils/types.h"
#include "book_types.h"

using namespace boost::posix_time;

//std::ostream& operator<<(std::ostream& out, const capk::KOrder& e);

namespace capk 
{ 


class KOrder
{
public:
    KOrder(uint32_t orderId, Side_t buySell, double size, double price);
    virtual ~KOrder();

    inline void setSize(double size) { _size = size; }
    inline void setPrice(double price) { _price = price; }
    inline void setOrderId(uint32_t orderId) { _orderId = orderId; }
    inline void setBuySell(Side_t buySell) { _buySell = buySell ; }
    //inline void setevtTime(const timespec eventTime) { _eventTime = eventTime; }
    
    inline double getSize() const { return _size; } 
    inline double getPrice() const { return _price; }
    inline orderId_t getOrderId() const { return _orderId; }
    inline Side_t getBuySell() const { return _buySell; }
    inline Side_t getSide() const { return _buySell; }
    //inline timespec getEventTime() const { return _eventTime; }
    //inline timespec getEntryTime() const { return _entryTime; }
    //OBE* nextOrder;
    //OBE* prevOrder;
    //Limit* limit; 

    
    bool operator<(const KOrder& rhs) const {
        return (this->getPrice() < rhs.getPrice());
    }

/* Do we need a compare for Orders? Or only limits? 
    bool operator>(const KOrder& rhs) const {
        return (this->price() > rhs.price());
    }
*/

    friend std::ostream& operator<<(std::ostream& out, const KOrder& e);

private:
    //timespec _entryTime;
    //timespec _evtTime;
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

} // namespace capk

#endif // CAPK_ORDER
