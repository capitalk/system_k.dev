//
//  order_book.c
//  OrderBook.v3
//
//  Created by Timir Karia on 2/17/13.
//  Copyright (c) 2013 Timir Karia. All rights reserved.
//

#include "order_book.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

  /** 
   * Give the book a name and starting price for the market.
   * init_price may come from market snapshot
   * multiplier should be 10^x where x is least significant decimal in price
   * one_way_offset_pct maximum single direction move in percent
   * tick_size actual tick size of market - will be multiplied by multiplier
   *  when constructing book
   */
orderBook* new_order_book(const char* symbol, 
         price_t init_price,
         uint32_t multiplier, 
         double one_way_offset_pct,
         double tick_size) {

  if (symbol == NULL) {
    fprintf(stderr, "Symbol may not be NULL to initialize orderbook\n");
    return NULL;
  }
  // TODO make sure that init_price conforms to tick or round it 
  uint32_t big_init_price = multiplier * init_price;
  uint32_t big_tick_increment = (uint32_t)(multiplier * tick_size);

  double offset = one_way_offset_pct * init_price;
  uint32_t big_offset = multiplier * offset;

  uint32_t big_upper_bound = big_init_price + big_offset;
  uint32_t big_lower_bound = big_init_price - big_offset;

  uint32_t range = big_upper_bound - big_lower_bound;

  
  // Check for wrong multiplier or tick size
  // Should we check tick_size * multiplier - big_tick_increment? 
  // double remainder  = (ob->multiplier * ob->tick_size) - big_tick_increment;
  if (big_tick_increment < 1) {
    fprintf(stderr, "Tick size * multiplier should be >= 1\n");
    return NULL;
  }

  uint32_t num_levels = range / big_tick_increment;

  
  // Sanity check   
  assert(big_upper_bound > big_lower_bound);
  
  priceLevel* levels = (priceLevel*) malloc(num_levels * sizeof(priceLevel));
  if (levels == NULL) {
    fprintf(stderr, "Can't allocate levels\n");
    return NULL;
  }
  orderBook* book = (orderBook*) malloc(sizeof(orderBook));
  if (book == NULL) {
    fprintf(stderr, "Can't allocate orderbook\n");
    return NULL;
  }
  
  
  memset(book, 0, sizeof(orderBook));
  memset(levels, 0, num_levels * sizeof(priceLevel));
  
  book->symbol = (char*) malloc(strlen(symbol)+1);
  strncpy(book->symbol, symbol, strlen(symbol));
  book->symbol[strlen(symbol)] = 0x0;

  book->init_price = init_price;
  book->multiplier = multiplier;
  book->num_levels = num_levels;
  book->one_way_offset_pct = one_way_offset_pct;
  book->tick_size = tick_size;
  book->lower_price_bound = big_lower_bound / multiplier;
  book->upper_price_bound = big_upper_bound / multiplier;
  book->levels = levels;
  
  init_order_book(book);
  return (orderBook*)book;
};

int validate_init_price(orderBook* ob) {
  uint32_t big_init_price = ob->multiplier * ob->init_price;
  uint32_t big_tick_increment = ob->multiplier * ob->tick_size;

  if (big_init_price % big_tick_increment != 0) {
    fprintf(stderr, "Init price (%f) must be multiple of tick size (%f)\n", 
        ob->init_price, ob->tick_size);
    return -1;
  }
  return 0;
}

int init_order_book(orderBook* ob) {
  if (!ob) {
    return -1;
  }

  uint32_t big_init_price = ob->multiplier * ob->init_price;
  uint32_t big_tick_increment = ob->multiplier * ob->tick_size;
  double offset = ob->one_way_offset_pct * ob->init_price;
  uint32_t big_offset = ob->multiplier * offset;
  //uint32_t big_upper_bound = big_init_price + big_offset;
  uint32_t big_lower_bound = big_init_price - big_offset;

  uint32_t i = 0;
  for (i = 0; i<ob->num_levels; i++) {
    // initialize price levels
    ob->levels[i].price = 
      (big_lower_bound + (i * big_tick_increment))/ob->multiplier;
    ob->levels[i].aggregate_qty = 0;
    ob->levels[i].num_orders = 0;
    for (uint32_t j = 0; j< MAX_ORDERS_PER_LEVEL; j++) {
      ob->levels[i].bids[j].quantity = 0;
      memset(ob->levels[i].bids[j].order_id, 
              ORDER_ID_INIT_CHAR, 
              MAX_ORDER_ID_LEN);

      ob->levels[i].asks[j].quantity = 0;
      memset(ob->levels[i].bids[j].order_id, 
              ORDER_ID_INIT_CHAR, 
              MAX_ORDER_ID_LEN);
    }
  }
  assert(i >= 0);
  //assert((uint32_t)(ob->levels[i].price * ob->multiplier) == big_upper_bound);
  ob->best_bid = NULL;
  ob->best_ask = NULL;

  return 0;
};

int addOrder(orderBook* ob,
    const b_side side, 
    const char* order_id, 
    const price_t price, 
    const quantity_t quantity) {
  assert(ob);
  if (price > ob->upper_price_bound || price < ob->lower_price_bound) {
    return E_OUT_OF_BOUNDS;
  }
  return 0;
};

int delOrder(orderBook* ob,
    const b_side side, 
    const char* order_id, 
    const price_t price, 
    const quantity_t quantity) {
  return 0;
};

int modOrder(orderBook* ob,
    const b_side side, 
    const char* order_id, 
    const price_t new_price, 
    const quantity_t new_quantity) {
  return 0;
};

void dump(orderBook* ob) {
  if (ob) {
    fprintf(stderr, "symbol:              %s\n", ob->symbol); 
    fprintf(stderr, "init_price:          %f\n", ob->init_price);
    fprintf(stderr, "num_levels:          %ui\n", ob->num_levels);
    fprintf(stderr, "multiplier:          %d\n", ob->multiplier);
    fprintf(stderr, "one_way_offset_pct:  %f\n", ob->one_way_offset_pct);
    fprintf(stderr, "tick_size:           %f\n", ob->tick_size);
    fprintf(stderr, "lower_price_bound:   %f\n", ob->lower_price_bound);
    fprintf(stderr, "upper_price_bound:   %f\n", ob->upper_price_bound);
    fprintf(stderr, "InitPrice:           %f\n", ob->init_price);
    fprintf(stderr, "Price at [0]:        %f\n", ob->levels[0].price);
    fprintf(stderr, "Price at [%d]:       %f\n", ob->num_levels, ob->init_price);
  }

}

const priceLevel* best_bid(orderBook* ob) {
  return NULL;
};

const priceLevel* best_ask(orderBook* ob) {
  return NULL;
};

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
