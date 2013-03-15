//
//  order_book.hxx
//  OrderBook.v3
//
//  Created by Timir Karia on 2/17/13.
//  Copyright (c) 2013 Timir Karia. All rights reserved.
//

#ifndef OrderBook_v3_order_book_hxx
#define OrderBook_v3_order_book_hxx

#include <string>

class order_book
{
public:
    order_book(double init_price, double one_way_offset_pct, double min_tick_size);
    ~order_book();
    double getBestBid() {return 0;};
    double getBestAsk() {return 0;};
    int addBid(double, double, std::string) {return 1;};
    int addAsk(double, double, std::string) {return 1;};
    
private:
    double _init_price;
    double _min_tick_size;
    double _one_way_offset_pct;
    
    double* data;
    
};


#endif
