//
//  order_book.h
//  OrderBook.v3
//
//  Created by Timir Karia on 12/22/12.
//  Copyright (c) 2012 Timir Karia. All rights reserved.
//

#ifndef __OrderBook_v3__order_book__
#define __OrderBook_v3__order_book__

#include <inttypes.h>

const uint32_t MAX_ORDERS_PER_LEVEL = 10;
const uint32_t MAX_ORDER_ID_LEN = 32;


struct _order {
    double quantity;
    char order_id[MAX_ORDER_ID_LEN];
};
typedef struct _order order;

struct _priceLevel {
    double _aggregate_qty;
    uint32_t _num_orders;
    double _price;
    order orders[MAX_ORDERS_PER_LEVEL];
};
typedef struct _priceLevel priceLevel;

struct orderBook {
    double init_price;
    int32_t num_levels;
    double one_way_offset_pct;
    double min_tick_size;
    double lower_price_bound;
    double upper_price_bound;
    priceLevel* levels;
    priceLevel* best_bid;
    priceLevel* best_ask;
    
};
typedef struct orderBook orderBook;

int init_order_book(orderBook* order_book);

orderBook* new_order_book(double init_price,
               double one_way_offset_pct,
               double min_tick_size);


int addOrder(orderBook* ob,
         const char* order_id,
         double price,
         double quantity);

int delOrder(orderBook* ob,
         const char* order_id,
         double price,
         double quantity);

int modOrder(orderBook* ob,
         const char* order_id,
         double new_price,
         double new_quantity);


const priceLevel* bestBid(orderBook* ob) {
    return NULL;
}

const priceLevel* bestOffer(orderBook* ob) {
    return NULL;
}



#endif /* defined(__OrderBook_v3__order_book__) */
