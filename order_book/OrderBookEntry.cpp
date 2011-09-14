#include "OrderBookEntry.h" 

namespace capitalk 
{ 

std::ostream& operator<< (std::ostream &out, const OrderBookEntry& e)
{
	out << FIX::UtcTimeStampConvertor::convert(e.modified(), true) << ","
		<< e.type() << ","
		<< e.id() << ","
		<< e.price() << ","
		<< std::fixed << e.size();
	return out;
}

}
