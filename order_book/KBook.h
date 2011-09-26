#ifndef CAPK_BOOK
#define CAPK_BOOK

#include "KTypes.h"
#include "KLimit.h"
#include "KOrder.h"
#include "utils/KTimeUtils.h"

#include <stdint.h>
#include <time.h>
#include <set>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

//namespace capitalk { 

//typedef boost::shared_ptr<KLimit> pKLimit;
//typedef std::set<pKLimit, KLimitComp> KTree;
//typedef std::map<uint32_t, pKOrder> KOrderMap;
//


class KBook
{
public:
    KBook(const char* name, size_t depth);
    virtual ~KBook();
    virtual int add(uint32_t orderId, buy_sell_t buySell, double size, double price, timespec evtTime, timespec exchSendTime);
    //virtual int removeBid(uint32_t orderId);//, timespec event);
    //virtual int removeAsk(uint32_t orderId);//, timespec event);
    virtual int remove(uint32_t orderId, timespec evtTime, timespec exchSendTime);
    //virtual int modifyBid(uint32_t orderId, double size);//, timespec event);
    //virtual int modifyAsk(uint32_t orderId, double size);//, timespec event);
    virtual int modify(uint32_t orderId, double size, timespec evtTime, timespec exchSendTime);
    inline const char* getName() const { return _name; };
    inline uint32_t getDepth() const { return _depth; };
    virtual double bestPrice(buy_sell_t buySell);
    virtual double bestPriceVolume(buy_sell_t buySell);

    friend std::ostream& operator<<(std::ostream& out, const KBook& b);
    virtual uint32_t getOrderCountAtLimit(buy_sell_t buySell, double price);
    virtual uint32_t getTotalVolumeAtLimit(buy_sell_t buySell, double price);
    void printLevels(buy_sell_t buySell);

    virtual pKOrder getOrder(uint32_t orderId) { return (_findOrderId(orderId)->second); }
    const char* getOutputVersionString() { return "V3"; } 
    const timespec getEventTime() { return _evtTime; }
    const timespec getExchangeSendTime() { return _exchSndTime; }

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


//}

#endif // CAPK_BOOK
