
#include "KOrderBook.h"

namespace capitalk {

OrderBook::OrderBook(const std::string& symbol, const int depth) 
    : _symbol(symbol), _depth(depth) 
{


}

OrderBook::~OrderBook()
{

}

}
