/*
 * (C) Capital K Partners 2011
 * @author Timir Karia
 * See the fix document describing orderbook best practices for 
 * the naming and ideas behind this code
 * http://www.fixprotocol.org/documents/4419/MDOWG_Book_Mgt_Final_draft.doc
 */

#include <iostream> 
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include "price_depth_order_book.h"


// for functions that return false on failure, use this macro 
#define CHECK(condition)\
{\
  if(!(condition))\
  {\
    std::cerr << "Check failed at " << __FILE__ << ":" << __LINE__;\
    std::cerr << " inside " << __FUNCTION__ << "\n" ;\
	std::cerr << "Condition: " << #condition << "\n" ;\
    return false;\
  }\
}

// for functions that return false on failure, use this macro 
#define CHECKMSG(condition, msg)\
{\
  if(!(condition))\
  {\
    std::cerr << "Check failed at " << __FILE__ << ":" << __LINE__;\
    std::cerr << " inside " << __FUNCTION__ << "\n" ;\
	std::cerr << "Condition: " << #condition << "\n" ;\
    std::cerr << "Msg: " << msg << "\n" ;\
    return false;\
  }\
}

namespace capk
{

PriceDepthOrderBook::PriceDepthOrderBook(const char* symbol,
		const unsigned int depth) : OrderBook(symbol,depth), _lastUpdateTime(0,0,0)
{
}

PriceDepthOrderBook::PriceDepthOrderBook(const std::string& symbol,
		const unsigned int depth) : OrderBook(symbol.c_str(),depth), _lastUpdateTime(0,0,0)
{
}

PriceDepthOrderBook::~PriceDepthOrderBook()
{

}


PriceDepthEntry* PriceDepthOrderBook::findBidById(const std::string& id) 
{ 
    IdMap::iterator iter = _bidIds.find(id); 
    return (iter == _bidIds.end()) ? 0 : iter->second; 
}

PriceDepthEntry* PriceDepthOrderBook::findOfferById(const std::string& id) 
{ 
    IdMap::iterator iter = _offerIds.find(id); 
    return (iter == _offerIds.end()) ? 0 : iter->second; 
}

PriceDepthEntry* PriceDepthOrderBook::findEntryById(const std::string& id) 
{ 
    PriceDepthEntry* e = this->findBidById(id); 
    return (e == 0) ? this->findOfferById(id) : e; 
}

PriceDepthEntry* PriceDepthOrderBook::walkToBidLevel(const unsigned int level) 
{ 
    return walkToBidLevelIter(level)->second; 
} 

PriceDepthEntry* PriceDepthOrderBook::walkToOfferLevel(const unsigned int level)
{ 
    return walkToOfferLevelIter(level)->second; 
}

PriceDepthEntry* PriceDepthOrderBook::walkToBidPrice(double price)
{
    return walkToBidPriceIter(price)->second; 
}

PriceDepthEntry* PriceDepthOrderBook::walkToOfferPrice(double price)
{
    return walkToOfferPriceIter(price)->second; 
}

// trim the price map down to desired depth and extra entries from 
// both the price->entry map and the id->entry map
template <typename T1, typename T2>
void trimMap(T1& priceMap, T2& idMap, unsigned int depth, bool deleteValue) { 
    typename T1::iterator iter = priceMap.end(); 
    unsigned int size = priceMap.size(); 
    while (size > depth) { 
        iter--; 
        idMap.erase(iter->second->id());
        if (deleteValue) { delete iter->second; } 
        priceMap.erase(iter); 
        size--; 
    }    
}


bool PriceDepthOrderBook::hasBidPrice(double price) { 
    return this->_bidPrices.find(price) != this->_bidPrices.end();
}  
bool PriceDepthOrderBook::hasOfferPrice(double price) {
    return this->_offerPrices.find(price) != this->_offerPrices.end(); 
}
bool PriceDepthOrderBook::hasPrice(double price) { 
    return this->hasBidPrice(price) || this->hasOfferPrice(price); 
}

bool PriceDepthOrderBook::hasBidId(const std::string& id) { 
    return this->_bidIds.find(id) != this->_bidIds.end();
}
bool PriceDepthOrderBook::hasOfferId(const std::string& id) {
    return this->_offerIds.find(id) != this->_offerIds.end();
}
bool PriceDepthOrderBook::hasId(const std::string& id) {
    return this->hasBidId(id) || this->hasOfferId(id); 
}


// remove any entries which have duplicate ID or price as new entry  
template <typename PRICE_MAP, typename ID_MAP> 
void remove_duplicate_entries(const std::string& id, ID_MAP& ids, double price, PRICE_MAP& prices) { 
  typename PRICE_MAP::iterator priceIter = prices.find(price); 
  if (priceIter != prices.end()) { 
    ids.erase(priceIter->second->id()); 
    delete priceIter->second;
    prices.erase(priceIter); 
  }
  typename ID_MAP::iterator idIter = ids.find(id); 
  if (idIter != ids.end()) { 
    prices.erase(idIter->second->price()); 
    delete idIter->second; 
    ids.erase(idIter); 
  } 
} 
/*
 * According to fix SPEC MDOWG_Book_Mgt_Final_draft.doc (from fixprotocol.org)
 * "The vendor must maintain the Price-Depth view in the following entries
 * NEW - to create /insert a new price level
 * DELETE - to remove a price level
 * CHANGE - to update quantity at a price level
 */

bool PriceDepthOrderBook::add(PriceDepthEntry* entry)
{
    CHECK(entry != 0);
    if (this->_lastUpdateTime < entry->_modified) { 
        this->_lastUpdateTime = entry->_modified; 
    }
    double price = entry->price(); 
    const std::string& id = entry->id(); 
    char side = entry->type(); 

    if (side == FIX::MDEntryType_BID) {

        // if this is a double-insertion, delete the old entry 
        // KTK TODO - remvoed check for duplicates
        //remove_duplicate_entries(id, _bidIds, price, _bidPrices);
        _bidIds[id] = entry;
        _bidPrices[price] = entry;         
        // shrink bid maps down to maximum depth 
        trimMap(_bidPrices, _bidIds, _depth, true); 
    } else {

    	CHECK(side == FIX::MDEntryType_OFFER);
        // KTK TODO - remvoed check for duplicates
        //remove_duplicate_entries(id, _offerIds, price, _offerPrices);
        _offerIds[id] = entry; 
        _offerPrices[price] = entry; 
        /* shrink offer maps down to maximum depth */ 	
        trimMap(_offerPrices, _offerIds, _depth, true); 
    }	
	return true; 
}

bool PriceDepthOrderBook::remove(const std::string& id, const FIX::UtcTimeStamp& time, bool deleteEntry)
{    
    IdMap::iterator idIter = _bidIds.find(id);
    if (idIter != _bidIds.end()) { 
	    PriceDepthEntry* entry = (*idIter).second; 
        PriceMapDesc::iterator priceIter = _bidPrices.find(entry->price()); 
        if (priceIter == _bidPrices.end()) {
            return false;
        }
        CHECK(priceIter != _bidPrices.end()); 

	    _bidPrices.erase(priceIter); 
        _bidIds.erase(idIter);
        if (this->_lastUpdateTime < time) { this->_lastUpdateTime = time; }
        // WARNING: only safe if this orderbook owns all the entries 
        if (deleteEntry) { delete entry; }
        return true; 
    }
     
    idIter = _offerIds.find(id);
    if(idIter != _offerIds.end()) {
        CHECK(idIter != _offerIds.end()); 
        PriceDepthEntry* entry = (*idIter).second; 
        PriceMapAsc::iterator priceIter = _offerPrices.find(entry->price()); 
        if (priceIter == _offerPrices.end()) {
            return false;
        }
        CHECK(priceIter != _offerPrices.end()); 
	
        _offerPrices.erase(priceIter); 
        _offerIds.erase(idIter);
        if (this->_lastUpdateTime < time) { this->_lastUpdateTime = time; }
        if (deleteEntry) { delete entry; }

        return true; 
    }
    return false; 
}

bool PriceDepthOrderBook::changeSize(const std::string& id, unsigned int size, const FIX::UtcTimeStamp& time)
{
    if (this->_lastUpdateTime < time) { _lastUpdateTime = time; }
	PriceDepthEntry* entry = NULL;
    entry = findEntryById(id); 
    if (0 == entry) {
        return false; 
    }
	CHECKMSG(entry != 0, "Depth may be outside book range" ); 
    if (this->_lastUpdateTime < time) { _lastUpdateTime = time; }
	entry->_size = size; 
    entry->_modified = time; 
	return true; 
}

bool PriceDepthOrderBook::replace(const std::string& id, PriceDepthEntry* entry)
{ 
    CHECK(entry != 0); 
    if (this->_lastUpdateTime < entry->_modified) { 
        _lastUpdateTime = entry->_modified; 
    }

    IdMap::iterator idIter = _bidIds.find(id);
    if (idIter != _bidIds.end()) { 
        _bidIds[id] = entry; 
        _bidPrices[entry->price()] = entry; 
    	return true; 
    } else { 
        idIter = _offerIds.find(id); 
        CHECK(idIter != _offerIds.end()); 
        _offerIds[id] = entry; 
        _offerPrices[entry->price()] = entry; 
    	return true; 
    } 
    return false;
} 


// to actually find a given level in an order book, use code which 
// doesn't care whether iterators come from ascending or descending maps 
template <typename Iter> 
Iter walkToLevel(Iter start, Iter end, unsigned int level) 
{ 
    unsigned int i = 0;
    while (i < level && start != end) { 
        start++; 
        i++;  
    }
    return start;
} 

PriceMapDesc::iterator
PriceDepthOrderBook::walkToBidLevelIter(unsigned int level) 
{ 
    return walkToLevel(_bidPrices.begin(), _bidPrices.end(), level); 
} 

PriceMapAsc::iterator
PriceDepthOrderBook::walkToOfferLevelIter(unsigned int level) 
{ 
    return walkToLevel(_offerPrices.begin(), _offerPrices.end(), level); 
} 


PriceMapDesc::iterator
PriceDepthOrderBook::walkToBidPriceIter(double price)
{ 
    PriceMapDesc::iterator iter = _bidPrices.begin(); 
    while((iter != _bidPrices.end()) && (price > iter->second->price())) {
        iter++;
    };
    return iter; 
}

PriceMapAsc::iterator
PriceDepthOrderBook::walkToOfferPriceIter(double price)
{ 
    PriceMapAsc::iterator iter = _offerPrices.begin(); 
	while((iter != _offerPrices.end()) && (price < iter->second->price())) {
	    iter++;
    };
    return iter; 
}
 

template <typename Iter>
void PriceDepthOrderBook::print_iter_range(std::ostream& out, Iter start, 
    Iter end, capk::Side_t side) const 
{ 
    int i = 1; 
    PriceDepthEntry* entry; 
    while (start != end) { 
        entry = start->second; 

        out << FIX::UtcTimeStampConvertor::convert(entry->modified(), true) 
            << ","
            << getSymbol() 
            << ","
			<< static_cast<char>(QUOTE) << ","
            << side << ","
			<< i 
            << ","
            << entry->price() 
            << ","
            << entry->size() 
            << ","
            << "1";
        out << "\n";
        start++; 
        i++;
    } 
}

void PriceDepthOrderBook::print(std::ostream& out) const
{
    out << "ORDERBOOK,";
    out <<  FIX::UtcTimeStampConvertor::convert(this->_lastUpdateTime, true);
    out << "\n"; 
    print_iter_range(out, _bidPrices.begin(), _bidPrices.end(), capk::BID); 
    print_iter_range(out, _offerPrices.begin(), _offerPrices.end(), capk::ASK);     
    //std::flush(out);
}

std::ostream&
operator<< (std::ostream &out, const PriceDepthOrderBook* pBook) {
	
	pBook->print(out);
	return out;
}

std::ostream&
operator<< (std::ostream &out, const PriceDepthOrderBook& b) {
	
	b.print(out);
	return out;
}

}
