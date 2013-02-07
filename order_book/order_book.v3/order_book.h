//
//  order_book.h
//  OrderBook.v3
//
//  Created by Timir Karia on 12/22/12.
//  Copyright (c) 2012 Timir Karia. All rights reserved.
//

#ifndef __OrderBook_v3__order_book__
#define __OrderBook_v3__order_book__

#include <iostream>

const size_t MAX_ORDERS_PER_LEVEL = 10;

struct price_level
{
    price_level(double price):
    _price(price),
    _num_orders(0)
    {};
    
    int32_t _num_orders;
    double _price;
    double orders[MAX_ORDERS_PER_LEVEL];
};

class order_book
{
public:
    order_book(double init_price, double one_way_offset_pct, double min_tick_size);
    ~order_book();
    
private:
    double _init_price;
    double _min_tick_size;
    double _one_way_offset_pct;
    
    double* data;
    
};

#endif /* defined(__OrderBook_v3__order_book__) */
