#ifndef CAPK_BOOK
#define CAPK_BOOK

#include <stdint.h>
#include <time.h>
#include <set>
#include <map>

#include "limit.h"
#include "order.h"
#include "book_types.h"
#include "utils/types.h"
#include "utils/time_utils.h"
#include "utils/constants.h"

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define OB_VERSION_STRING "V3"

//std::ostream& operator<<(std::ostream& out, const capk::KBook& e);

namespace capk 
{

    class KLimit;
    class KOrder;
    class KBook;
    class KLimitComp;
   
    struct KLimitComp : public std::binary_function<pKLimit, pKLimit, bool>
    {
            bool operator() (pKLimit const& a, pKLimit const& b);
    };


class KBook
{
public:
    KBook(const char* name, size_t depth);
    virtual ~KBook();
    virtual int add(uint32_t orderId, capk::Side_t buySell, double size, double price, timespec evtTime, timespec exchSendTime);
    //virtual int removeBid(uint32_t orderId);//, timespec event);
    //virtual int removeAsk(uint32_t orderId);//, timespec event);
    virtual int remove(uint32_t orderId, timespec evtTime, timespec exchSendTime);
    //virtual int modifyBid(uint32_t orderId, double size);//, timespec event);
    //virtual int modifyAsk(uint32_t orderId, double size);//, timespec event);
    virtual int modify(uint32_t orderId, double size, timespec evtTime, timespec exchSendTime);
    inline const char* getName() const { return _name; };
    inline uint32_t getDepth() const { return _depth; };
    virtual double bestPrice(capk::Side_t buySell);
    virtual double bestPriceVolume(capk::Side_t buySell);

    friend std::ostream& operator<<(std::ostream& out, const KBook& b);
    virtual uint32_t getOrderCountAtLimit(capk::Side_t buySell, double price);
    virtual uint32_t getTotalVolumeAtLimit(capk::Side_t buySell, double price);
    void printLevels(capk::Side_t buySell);

    virtual pKOrder getOrder(uint32_t orderId);
                                                
    const char* getOutputVersionString() { return OB_VERSION_STRING; } 
    const timespec getEventTime() { return _evtTime; }
    const timespec getExchangeSendTime() { return _exchSndTime; }
    void dbg();

private:
    virtual int addBid(KOrder* bid, timespec eventTime, timespec exchSendTime);
    virtual int addAsk(KOrder* ask, timespec eventTime, timespec exchSendTime);
    KOrderMap _orderMap;
    KTree _bidTree;
    KTree _askTree;
    KLimit* _bestBid;
    KLimit* _bestAsk; 
    char _name[128];
    timespec _evtTime;
    timespec _exchSndTime;
    uint32_t _depth;

    KTree::iterator _findLimit(KTree&  tree, double price);
    KOrderMap::iterator _findOrderId(uint32_t orderId);
    
};  

} // namespace capk

//std::ostream& ::operator<<(std::ostream& out, const capk::KBook& b);

#endif // CAPK_BOOK
