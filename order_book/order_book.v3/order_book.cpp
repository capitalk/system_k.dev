//
//  order_book.cpp
//  OrderBook.v3
//
//  Created by Timir Karia on 12/22/12.
//  Copyright (c) 2012 Timir Karia. All rights reserved.
//

#include "order_book.hxx"
#include <assert.h>
#include <iostream>

order_book::order_book(double init_price, double one_way_offset_pct, double min_tick_size):
_init_price(init_price),
_one_way_offset_pct(one_way_offset_pct),
_min_tick_size(min_tick_size)
{
    assert(init_price >= 0);
    assert(one_way_offset_pct);
    double bound = init_price * one_way_offset_pct;
    double upper_bound = init_price + bound;
    double lower_bound = init_price - bound;
    double range = upper_bound - lower_bound;
    int32_t buckets = range / _min_tick_size;
#ifdef DEBUG
    std::cerr << "Initializing order book with: " \
    << "init_price: " << _init_price << "\n" \
    << "one_way_offset_pct: " << _one_way_offset_pct << "\n" \
    << "min_tick_size: " << _min_tick_size << "\n" \
    << "bound: " << bound << "\n" \
    << "upper_bound: " << upper_bound << "\n" \
    << "lower_bound: " << lower_bound << "\n" \
    << "range: " << range << "\n" \
    << "buckets: " << buckets << "\n" \
    << std::endl;
#endif
};

order_book::~order_book()
{
    
}
