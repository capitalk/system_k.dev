#ifndef CAPK_ORDER
#define CAPK_ORDER

#include <string>

#include <time.h>

#include <map>

#include <boost/date_time/posix_time/posix_time.hpp> // for ptime

#include "KTypes.h"
#include "utils/KTimeUtils.h"

/*
typedef uint32_t orderId_t;

typedef enum {
    BUY=0,
    SELL=1
} buy_sell_t;
*/
using namespace boost::posix_time;

//namespace capitalk { 


class KOrder
{
public:
    KOrder(uint32_t orderId, buy_sell_t buySell, double size, double price);
    virtual ~KOrder();

    inline void setSize(double size) { _size = size; }
    inline void setPrice(double price) { _price = price; }
    inline void setOrderId(uint32_t orderId) { _orderId = orderId; }
    inline void setBuySell(buy_sell_t buySell) { _buySell = buySell ; }
    //inline void setevtTime(const timespec eventTime) { _eventTime = eventTime; }
    
    inline double getSize() const { return _size; } 
    inline double getPrice() const { return _price; }
    inline orderId_t getOrderId() const { return _orderId; }
    inline buy_sell_t getBuySell() const { return _buySell; }
    inline buy_sell_t getSide() const { return _buySell; }
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
    buy_sell_t _buySell; 
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

//}

#endif // CAPK_ORDER
