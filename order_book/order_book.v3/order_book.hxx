//
//  order_book.hxx
//  OrderBook.v3
//
//  Created by Timir Karia on 2/17/13.
//  Copyright (c) 2013 Timir Karia. All rights reserved.
//

#ifndef OrderBook_v3_order_book_hxx
#define OrderBook_v3_order_book_hxx
#ifdef __cplusplus

extern "C" {
#include "order_book.h"
}
#endif // __cplusplus

#include <string>

namespace capk {

class order_book
{
public:
    order_book(const char* symbol, 
               double init_price, 
               uint32_t multiplier, 
               double one_way_offset_pct, 
               double min_tick_size);
    ~order_book();
    orderBook* getBook();
    void dump();
    double getBestBid() {return 0;};
    double getBestAsk() {return 0;};
    int addBid(double, double, std::string) {return 1;};
    int addAsk(double, double, std::string) {return 1;};
    
private:
    orderBook* _ob;
    double _init_price;
    double _one_way_offset_pct;
    double _min_tick_size;
    


    
};

} // namespace capk

#endif
