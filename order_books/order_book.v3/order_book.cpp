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
#include <stdexcept>

namespace capk {

order_book::order_book(const char* symbol,
                       price_t init_price,
                       uint32_t multiplier,
                       double one_way_offset_pct,
                       double min_tick_size):
_init_price(init_price),
_one_way_offset_pct(one_way_offset_pct),
_min_tick_size(min_tick_size)
{
    assert(init_price >= 0);
    assert(one_way_offset_pct);
    /*
    double bound = init_price * one_way_offset_pct;
    double upper_bound = init_price + bound;
    double lower_bound = init_price - bound;
    double range = upper_bound - lower_bound;
    int32_t buckets = range / _min_tick_size;
    */

    _ob = new_order_book(symbol,
                         init_price,
                         multiplier,
                         one_way_offset_pct,
                         min_tick_size);
    if (!_ob) {
      throw std::runtime_error("Can't initialize order book");
    }
};

orderBook* order_book::getBook() {
  return _ob;
}

void order_book::dump()
{
  ::dump(_ob);
}

order_book::~order_book()
{
    
}

} // namespace capk
