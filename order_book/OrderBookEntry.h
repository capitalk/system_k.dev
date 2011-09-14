#include <string>

#include "quickfix/FieldConvertors.h"
#include "quickfix/FieldTypes.h"

#ifndef CAPITAL_K_ORDERBOOK_ENTRY_H
#define CAPITAL_K_ORDERBOOK_ENTRY_H

namespace capitalk { 

class OrderBookEntry
{
public:
    virtual FIX::UtcTimeStamp created() const=0;
    virtual FIX::UtcTimeStamp modified() const=0; 

    virtual char type() const=0;
    virtual const std::string& id() const=0;
    virtual double price() const=0;
    virtual unsigned int size() const=0;

    bool operator<(const OrderBookEntry& rhs) const {
        return (this->price() < rhs.price());
    }

    bool operator>(const OrderBookEntry& rhs) const {
        return (this->price() > rhs.price());
    }

    friend std::ostream& operator<<(std::ostream& out, const OrderBookEntry& e);
};  

}

#endif
