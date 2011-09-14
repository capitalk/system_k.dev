#include <array>
#include <string>

#include "quickfix/Values.h"
#include "quickfix/Application.h"

#include "quickfix/FieldConvertors.h"
#include "quickfix/FieldTypes.h"

#include "OrderBookEntry.h" 

#ifndef CAPITAL_K_ORDERBOOK_H
#define CAPITAL_K_ORDERBOOK_H

namespace capitalk {

enum TickType {
		QUOTE = 'Q',
		DEAL  = 'D'
};

enum Side {
		BID = 0,
		OFFER = 1
};


class OrderBook
{
public:
	OrderBook(const std::string& symbol, const int depth);	

	virtual ~OrderBook();
	/*
        Order books don't seem to share common functionality at level 
        below PricedepthOrderBook, so I'm not positive why this
        class exists
    */ 
	inline const std::string& getSymbol() const { return _symbol;}
    inline int getDepth() const { return _depth;}
    virtual const FIX::UtcTimeStamp& lastUpdateTime() const=0; 

protected: 
	std::string _symbol;
	unsigned int _depth;
	
};

};

#endif //CAPITAL_K_ORDERBOOK_H
