#include "KBook.h"


KBook::KBook(const char* name, size_t depth):_depth(depth)
{
    if (name) {
        strncpy(_name, name, 127);
    }
}

KBook::~KBook()
{
}

int
KBook::add(uint32_t orderId, buy_sell_t buySell, double size, double price, timespec evtTime, timespec exchSndTime)//, timespec evtTime*/)
{
    _exchSndTime = exchSndTime;    
    KOrder* pOrd = new KOrder(orderId, buySell, size, price);
    if (buySell == BUY) {
        return addBid(pOrd, evtTime, exchSndTime);
    }
    if (buySell == SELL) {
        return addAsk(pOrd, evtTime, exchSndTime);
    }
    return 0;
}

int
KBook::addBid(KOrder* bid, timespec evtTime, timespec exchSndTime)
{
    if (bid) {
        pKOrder pBid(bid);
        KTree::iterator it = _findLimit(_bidTree, pBid->getPrice());
        if (it != _bidTree.end()) {
            (*it)->addOrder(pBid);
            (*it)->setUpdateTime(evtTime);
            this->_evtTime = evtTime;//(*it)->getUpdateTime();
        } 
        else {
#ifdef DEBUG
            std::cerr << "Adding new limit: " << pBid->getPrice() << std::endl;
#endif
            KLimit* newLimit = new KLimit(pBid->getPrice());
            newLimit->setUpdateTime(evtTime);
            newLimit->addOrder(pBid); 
            this->_evtTime = evtTime;//newLimit->getUpdateTime();
            _bidTree.insert(pKLimit(newLimit));
        }
        _orderMap[bid->getOrderId()] = pBid;
#ifdef DEBUG
        KOrderMap::iterator xxx = _findOrderId(bid->getOrderId());
        std::cerr << "MAP ENTRY: " << xxx->second->getOrderId() << std::endl;
#endif
        return 1;
    }
    return 0;
}

int
KBook::addAsk(KOrder* ask, timespec evtTime, timespec exchSndTime)
{
    if (ask) {
        pKOrder pAsk(ask);
        KTree::iterator it = _findLimit(_askTree, ask->getPrice());
        if (it != _askTree.end()) {
            (*it)->addOrder(pAsk);
            (*it)->setUpdateTime(evtTime);
            _evtTime = evtTime;//(*it)->getUpdateTime();
        } 
        else {
            KLimit* newLimit = new KLimit(ask->getPrice());
            newLimit->setUpdateTime(evtTime);
            newLimit->addOrder(pAsk); 
            _evtTime = evtTime;//newLimit->getUpdateTime();
            _askTree.insert(pKLimit(newLimit));
        }
        _orderMap[ask->getOrderId()] = pAsk;
        return 1; 
    }
    return 0;
}

int
KBook::remove(uint32_t orderId, timespec evtTime, timespec exchSndTime) 
{
    double limit;
    buy_sell_t bidAsk;
    _exchSndTime = exchSndTime;
    KOrderMap::iterator oit =  _findOrderId(orderId);
    if (oit != _orderMap.end()) {
        limit = oit->second->getPrice();    
        bidAsk = oit->second->getBuySell();

        if (bidAsk == BUY) {
            KTree::iterator bit = _findLimit(_bidTree, limit);    
            if (bit != _bidTree.end()) {
                _orderMap.erase(orderId);  
                int removeOk = (*bit)->removeOrder(orderId);
                assert(removeOk == 1);
                _evtTime = evtTime; 
                (*bit)->setUpdateTime(_evtTime);
                // KTK !!!! TODO - remove the limit ONLY if qty = 0
                // OK - but should just ignore in the get best method - maybe faster? 
                if ((*bit)->getOrderCount() == 0) {
                    _bidTree.erase(bit);            
                }
                return 1;
            }
            return 0;
        }
        if (bidAsk == SELL) {
            KTree::iterator ait = _findLimit(_askTree, limit);
            if (ait != _bidTree.end()) {
                _orderMap.erase(orderId);
                int removeOk = (*ait)->removeOrder(orderId);
                assert(removeOk == 1);
                _evtTime = evtTime;
                (*ait)->setUpdateTime(_evtTime);
                // KTK !!!! TODO - remove the limit ONLY if qty = 0
                // OK - but should just ignore in the get best method - maybe faster? 
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

int
KBook::modify(uint32_t orderId, double size, timespec evtTime, timespec exchSndTime)
{

// KTK - this is inefficient since we need to use the orderId in 
// the map to get to the limit price. Then use the limit price to search for the order
// in the tree
    _exchSndTime = exchSndTime;
    KOrderMap::iterator it = _findOrderId(orderId);
    if (it != _orderMap.end()) {
//TODO - Modify size directly in order and have order notify its parent
//TODO - if price is modified does queue position get lost? - KTK - irrelevant since price is not a param!!!
//TODO - ask Hotspot
        // Get side
        double price = it->second->getPrice();
        buy_sell_t bs = it->second->getBuySell();
        KTree::iterator lit;
        if (bs == BUY) {
            lit  = _findLimit(_bidTree, price);
            if (lit == _bidTree.end()) { return 0;}
        }
        if (bs == SELL) {
            lit  = _findLimit(_askTree, price);
            if (lit == _askTree.end()) { return 0;}
        }
        (*lit)->modifyOrder(orderId, size);
        (*lit)->setUpdateTime(_evtTime);
        _evtTime = evtTime;//(*lit)->getUpdateTime();
        return 1;
    }
    return 0;
}

uint32_t
KBook::getTotalVolumeAtLimit(buy_sell_t buySell, double price) 
{
    KTree::iterator it;
    if (buySell == BUY) {
        it = _findLimit(_bidTree, price);
        if (it != _bidTree.end()) {
#ifdef DEBUG
            std::cerr << "BID VOLUME @ " << price << ": " << (*it)->getTotalVolume() << std::endl;
#endif 
            return (*it)->getTotalVolume(); 
        }
    }
    if (buySell == SELL) {
        it = _findLimit(_askTree, price);
        if (it != _askTree.end()) {
#ifdef DEBUG
            std::cerr << "ASK VOLUME @ " << price << ": " << (*it)->getTotalVolume() << std::endl;
#endif
            return (*it)->getTotalVolume(); 
        }
    } 
#ifdef DEBUG 
    std::cerr << "NO LIMIT: " << buySell << " @ " << price << std::endl;
#endif 
    //return -1;
    return 0;
}

uint32_t
KBook::getOrderCountAtLimit(buy_sell_t buySell, double price) 
{
    KTree::iterator it;
    if (buySell == BUY) {
        it = _findLimit(_bidTree, price);
        if (it != _bidTree.end()) {
#ifdef DEBUG
            std::cerr << "BIDS @ " << price << ": " << (*it)->getOrderCount() << std::endl;
#endif 
            return (*it)->getOrderCount(); 
        }
    }
    if (buySell == SELL) {
        it = _findLimit(_askTree, price);
        if (it != _askTree.end()) {
#ifdef DEBUG
            std::cerr << "ASKS @ " << price << ": " << (*it)->getOrderCount() << std::endl;
#endif
            return (*it)->getOrderCount(); 
        }
    } 
    //return -1;
    return 0;
}
double
KBook::bestPriceVolume(buy_sell_t buySell)
{
    double p = bestPrice(buySell);
    return getTotalVolumeAtLimit(buySell, p);
}

double
KBook::bestPrice(buy_sell_t buySell)
{
    //printLevels(buySell);
    if (buySell == BUY) {
        // KTK TODO - NOTE this does not support negative prices
        KTree::reverse_iterator bit = _bidTree.rbegin();
        if (bit == _bidTree.rend()) { 
            return -1;
        }
        return (*bit)->getPrice();
        
    }
    if (buySell == SELL) {
        // KTK TODO - NOTE this does not support negative prices
        KTree::iterator ait = _askTree.begin();
        if (ait == _askTree.end()) { 
            return -1;
        }
        return (*ait)->getPrice();
    }
    return (-1);
}

void
KBook::printLevels(buy_sell_t buySell)
{
    if (buySell == BUY) {
        KTree::reverse_iterator bit = _bidTree.rbegin();
        while (bit != _bidTree.rend()) {
            std::cout << "BID: " << *(*bit) << "\n";
            bit++;
        }
        
    }
    if (buySell == SELL) {
        KTree::reverse_iterator ait = _askTree.rbegin();
        while (ait != _askTree.rend()) {
            std::cout << "ASK: " << *(*ait) << "\n";
            ait++;
        }
    }
}


KTree::iterator
KBook::_findLimit(KTree& tree, double price) 
{
    KTree::iterator it;
    KLimit* tmpLimit = new KLimit(price); 
    it = tree.find(pKLimit(tmpLimit)); 
    return it;
}

KOrderMap::iterator
KBook::_findOrderId(uint32_t orderId)
{
    return _orderMap.find(orderId);        
}

std::ostream& 
operator<<(std::ostream& out, const KBook& b)
{
    out << "ORDERBOOK,";
    //out << FIX::UtcTimeStampConvertor::convert(_evtTime, true) << "," << _name << "\n";
    out << b._name << "," << b._evtTime << "," << b._exchSndTime << "\n";
    KTree::reverse_iterator bit = b._bidTree.rbegin();
    KTree::iterator ait = b._askTree.begin();
    int i = 1;
    while (bit != b._bidTree.rend()) {
        int nOrders = (*bit)->getOrderCount();
        //if (nOrders == 0) { bit++; continue; }
        out << (*bit)->getUpdateTime() << "," 
            //<< b._name << ","
            << (char)QUOTE << ","
            << BID << ","
            << i << ","
            //<< *(*bit) << ",";
            << (*bit)->getPrice() << ","
            << (*bit)->getTotalVolume() << ",";
            //<< (*bit)->getOrderCount() << ",";
            Orders::iterator it = (*bit)->_orders.begin();
            while (it != (*bit)->_orders.end()) {
                out << "[" << (*it)->getOrderId() << "]" << (*it)->getSize();
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
        int nOrders = (*ait)->getOrderCount();
        //if (nOrders == 0) { ait++; continue;}
        out << (*ait)->getUpdateTime() << "," 
            //<< b._name << ","
            << (char)QUOTE << ","
            << ASK << ","
            << i << ","
            //<< *(*ait) << ",";
            << (*ait)->getPrice() << ","
            << (*ait)->getTotalVolume() << ",";
            //<< (*bit)->getOrderCount() << ",";
            Orders::iterator it = (*ait)->_orders.begin();
            while (it != (*ait)->_orders.end()) {
                out << "[" << (*it)->getOrderId() << "]" << (*it)->getSize();
                it++; 
                if (it != (*ait)->_orders.end()) {
                    out << ";";
                }
            } 
        out << "\n";
            i++;
            ait++;
    }
    return out;

}

