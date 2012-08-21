
#include "price_depth_entry.h" 

namespace capk { 

PriceDepthEntry::PriceDepthEntry(
    const FIX::UtcTimeStamp& created, 
    const FIX::UtcTimeStamp& modified,  
    char type, 
    const std::string& id, 
    double px, 
    unsigned int size) :
        _created(created),
        _modified(modified),
		_type(type),
        _id(id),
        _price(px),
        _size(size)
{

}

}
