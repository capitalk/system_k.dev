#include <string> 
#include "OrderBookEntry.h" 


#ifndef CAPITAL_K_PRICE_DEPTH_ENTRY_H
#define CAPITAL_K_PRICE_DEPTH_ENTRY_H

namespace capitalk { 

class PriceDepthEntry : public OrderBookEntry 
{
public:
	FIX::UtcTimeStamp _created;
	FIX::UtcTimeStamp _modified; 
    char _type;
    std::string _id;
    double _price;
    unsigned int _size;
 
	PriceDepthEntry(
        const FIX::UtcTimeStamp&, 
        const FIX::UtcTimeStamp&, 
        char, 
        const std::string&, 
        double, 
        unsigned int); 

    virtual FIX::UtcTimeStamp created() const { return _created; }  

    virtual FIX::UtcTimeStamp modified() const  { return _modified; } 

    virtual char type() const  { return _type; } 

    virtual const std::string& id() const  { return _id; } 

    virtual double price() const  { return _price; } 

    virtual unsigned int size() const { return _size; } 

	virtual ~PriceDepthEntry() {};
   
};

} 

#endif
