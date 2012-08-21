#ifndef CAPK_LIMIT
#define CAPK_LIMIT

#include <time.h>
#include <stdint.h>
#include <list>
#include <set>
#include <iostream>

#include "book_types.h"
#include "order.h"
#include "order_book.h"
#include "utils/types.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>

//std::ostream& operator<<(std::ostream& out, const capk::KBook& b);
//std::ostream& operator<<(std::ostream& out, const capk::KOrder& b);
//std::ostream& operator<<(std::ostream& out, const capk::KLimit& e);

namespace capk 
{

class KBook;

class KLimit
{
public:
    KLimit(double price, uint32_t totalVolume = 0);
    virtual ~KLimit();
    //KLimit* parent;
    //KLimit* leftChild;
    //KLimit* rightChild; 
    //KOrder* headOrder;
    //KOrder* tailOrder;
    
    int addOrder(pKOrder order);
    //int removeOrder(pKOrder order);
    int removeOrder(uint32_t orderId);
    int modifyOrder(uint32_t orderId, double size);
    inline uint32_t getTotalVolume() const { return _totalVolume; } 
    inline double getPrice() const { return _price; }
    inline uint32_t getOrderCount() const { return _orders.size(); }
    inline timespec getUpdateTime() const { return _timeUpdated; }
    inline void setUpdateTime(const timespec updateTime) { _timeUpdated = updateTime; }
    inline timespec getMsgSentTime() const { return _msgSentTime; }
    inline void setMsgSentTime(const timespec sentTime) { _msgSentTime = sentTime; }
    //inline FIX::UTCTimeStamp getFIXSentTime() const { return _FIXSentTime; }
    //int setFIXSentTime(const FIX::UTCTimeStamp FIXUTCTimeStamp) { _FIXSentTime = FIXUTCTimeStamp;}

    //void printAllOrders();

    bool operator<(KLimit& rhs) const {
        return (this->_price < rhs.getPrice());
    }

    bool operator>(KLimit& rhs) const {
        return (this->_price > rhs.getPrice());
    }

    friend std::ostream& operator<<(std::ostream& out, const capk::KLimit& e);
    friend std::ostream& operator<<(std::ostream& out, const capk::KBook& b);

private:
    Orders _orders;
    double _price;
    uint32_t _totalVolume;
    timespec _timeUpdated;    
    timespec _msgSentTime;
    //FIX::UTCTimeStamp _FIXSentTime;
    inline uint32_t _addVolume(uint32_t vol) { 
        _totalVolume += vol; 
#ifdef DEBUG
        std::cerr << "TOTAL VOLUME(@" << _price << ") NOW (+ " << vol << "):" << _totalVolume << std::endl;
#endif
        return _totalVolume; }

    inline uint32_t _removeVolume(uint32_t vol) { 
        _totalVolume -= vol; 
#ifdef DEBUG
        std::cerr << "TOTAL VOLUME(@" << _price << ") NOW (- " << vol << "):" << _totalVolume << std::endl;
#endif
        return _totalVolume; }

    Orders::iterator _findOrder(uint32_t orderId);
    int _removeOrder(uint32_t orderId);
    
};  

/*
class KLimitComp : public std::binary_function<pKLimit, pKLimit, bool>
{
    bool operator() (pKLimit const& a, pKLimit const& b) {
        return (*a) < (*b);
    }
};
*/

/*
struct KLimitComp : public std::binary_function<pKLimit, pKLimit, bool>
{
    bool operator() (pKLimit const& a, pKLimit const& b) {
        return (*a) < (*b);
    }
};
*/
} // namespace capk


#endif // CAPK_LIMIT
