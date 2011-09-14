
#include <string>
#include <list>
#include <tr1/unordered_set>
#include <ostream>
#include <assert.h>

#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>

#include "KOrderBook.h"
#include "PriceDepthEntry.h"
 
#ifndef CAPITAL_K_PRICE_LIMIT_ORDERBOOK_H
#define CAPITAL_K_PRICE_LIMIT_ORDERBOOK_H

namespace capitalk
{


typedef std::map<std::string, PriceDepthEntry*> IdMap; 
// use reverse sort orders since Bid side needs to be decreasing, but 
// Offer side needs to be increasing 
typedef std::map<double, PriceDepthEntry*, std::less<double> > PriceMapAsc; 
typedef std::map<double, PriceDepthEntry*, std::greater<double> > PriceMapDesc;    

class PriceDepthOrderBook : public OrderBook
{
public: 
	PriceDepthOrderBook(const char * symbol, const unsigned int depth);
	PriceDepthOrderBook(const std::string& symbol, const unsigned int depth);

	virtual ~PriceDepthOrderBook();

    virtual const FIX::UtcTimeStamp& lastUpdateTime() const { 
        return _lastUpdateTime; 
    } 

	virtual bool add(PriceDepthEntry*);
	virtual bool remove(const std::string&, const FIX::UtcTimeStamp&, bool);
    virtual bool replace(const std::string&, PriceDepthEntry*); 
	virtual bool changeSize(const std::string&, unsigned int, const FIX::UtcTimeStamp&);

    virtual PriceDepthEntry* findBidById(const std::string&);  
    virtual PriceDepthEntry* findOfferById(const std::string&);  
    virtual PriceDepthEntry* findEntryById(const std::string&);  

    virtual PriceDepthEntry* walkToBidLevel(const unsigned int level);
    virtual PriceDepthEntry* walkToOfferLevel(const unsigned int level);

    virtual PriceDepthEntry* walkToBidPrice(double price);
    virtual PriceDepthEntry* walkToOfferPrice(double price); 



	friend std::ostream& 
        operator<<(std::ostream& out, const PriceDepthOrderBook&  b); 

	friend std::ostream& 
        operator<<(std::ostream& out, const PriceDepthOrderBook* pb); 

    /* these are going to be ascending order, which is backwards 
       from what we want. For now the caller must remember to 
       use a reverse iterator, but later we should change the 
       interface to hide this detail. 
    */ 
	const IdMap& getBidIds() const { return _bidIds; }
	const PriceMapDesc& getBidPrices() const { return _bidPrices; } 

	const IdMap& getOfferIds() const { return _offerIds;}
	const PriceMapAsc& getOfferPrices() const { return _offerPrices; } 

	int numBids() const { 
        assert(_bidIds.size() == _bidPrices.size()); 
        return _bidIds.size(); 
    } 

	int numOffers() const { 
        assert(_offerIds.size() == _offerPrices.size());         
        return _offerIds.size(); 
    }  	

    bool hasBidPrice(double price); 
    bool hasOfferPrice(double price); 
    bool hasPrice(double price); 

    bool hasBidId(const std::string&); 
    bool hasOfferId(const std::string&); 
    bool hasId(const std::string&); 

	void print (std::ostream& out) const;
	
protected:
	IdMap _bidIds;
	PriceMapDesc _bidPrices; 
	
	IdMap _offerIds; 
	PriceMapAsc _offerPrices; 

    FIX::UtcTimeStamp _lastUpdateTime; 
private: 
	PriceMapDesc::iterator walkToBidLevelIter(const unsigned int level);
	PriceMapDesc::iterator walkToBidPriceIter(double price);
	
	PriceMapAsc::iterator walkToOfferLevelIter(const unsigned int level);
    PriceMapAsc::iterator walkToOfferPriceIter(double price); 

    template <typename Iter>
    void print_iter_range(std::ostream& out, Iter start, Iter end, capitalk::Side side) const; 
};

}

#endif //CAPITAL_K_PRICE_LIMIT_ORDERBOOK_H
