//
//  order_book.c
//  OrderBook.v3
//
//  Created by Timir Karia on 2/17/13.
//  Copyright (c) 2013 Timir Karia. All rights reserved.
//

#include <stdio.h>
#include "order_book.h"
#include <assert.h>
#include <stdlib.h>
#include <memory.h>

extern "C" {

struct orderBook* new_order_book(double init_price,
         double one_way_offset_pct,
         double min_tick_size) {
  // TODO make sure that init_price conforms to tick or round it 
  double abs_price_range = init_price * one_way_offset_pct;
  
  size_t one_way_levels = 
    ((init_price + abs_price_range) / min_tick_size);

  size_t levels_size = 
    one_way_levels * sizeof(priceLevel);
  
  double lower_price_bound = 
    init_price - (one_way_levels * min_tick_size);

  double upper_price_bound = 
    init_price + (one_way_levels * min_tick_size);
  
  priceLevel* bid_levels = (priceLevel*) malloc(levels_size);
  priceLevel* ask_levels = (priceLevel*) malloc(levels_size);
  orderBook* book = (orderBook*) malloc(sizeof(orderBook));
  
  memset(book, 0, sizeof(orderBook));
  memset(levels, 0, levels_size);
  
  book->init_price = init_price;
  book->one_way_offset_pct = one_way_offset_pct;
  book->min_tick_size = min_tick_size;
  book->lower_price_bound = lower_price_bound;
  book->upper_price_bound = upper_price_bound;
  book->bid_levels = bid_levels;
  book->ask_levels = ask_levels;
  
  if (book == NULL || levels == NULL) {
    // error
    return NULL;
  }
  else {
    init_order_book(book);
  }
  return (orderBook*)book;
};

int init_order_book(orderBook* order_book) {
  if (!order_book) {
    return -1;
  }
  for (int i = 0; i<order_book->num_levels; i++) {
    order_book->levels[i].price = 
      order_book->lower_price_bound + (i * order_book->min_tick_size);
    for (int j = 0; j< MAX_ORDERS_PER_LEVEL; j++) {
      order_book->levels[i].orders[j].quantity = capk::INIT_SIZE;
      memset(order_book->levels[i].orders[j].order_id, 0xAA, MAX_ORDER_ID_LEN);
    }
  }
  return 0;
};

int addOrder(orderBook* ob,
    const capk::side_t side, 
    const char* order_id, 
    const double price, 
    const double quantity) {
  return 0;
};

int delOrder(orderBook* ob,
    const capk::side_t side, 
    const char* order_id, 
    const double price, 
    const double quantity) {
  return 0;
};

int modOrder(orderBook* ob,
    const capk::side_t side, 
    const char* order_id, 
    const double new_price, 
    const double new_quantity) {
  return 0;
};



struct price_level* best_bid() {
  return NULL;
};

struct price_level* best_ask() {
  return NULL;
};

} // extern "C"
