//
//  order_book.h
//  OrderBook.v3
//
//  Created by Timir Karia on 12/22/12.
//  Copyright (c) 2012 Timir Karia. All rights reserved.
//

#ifndef __OrderBook_v3__order_book__
#define __OrderBook_v3__order_book__

#ifdef _WIN32
#include <stdint.h>
#else
#include <inttypes.h>
#endif // _WIN32
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif //  __cplusplus

#define E_OUT_OF_BOUNDS -2
#define E_PRICE_NOT_MULTIPLE_OF_TICK -3

typedef uint32_t quantity_t;
typedef double price_t;
enum book_side {
  BID = 0,
  ASK = 1,
  NO_SIDE = 99
};
typedef enum book_side b_side;

const uint32_t MAX_ORDERS_PER_LEVEL = 100;
const uint32_t MAX_ORDER_ID_LEN = 32;
const char ORDER_ID_INIT_CHAR = 0xAA;

struct _order {
    quantity_t quantity;
    char order_id[MAX_ORDER_ID_LEN];
};
typedef struct _order order;

struct _priceLevel {
    price_t price;
    quantity_t aggregate_qty;
    uint32_t num_orders;
    order bids[MAX_ORDERS_PER_LEVEL];
    order asks[MAX_ORDERS_PER_LEVEL];
};
typedef struct _priceLevel priceLevel;

struct _orderBook {
    char* symbol;
    price_t init_price;
    uint32_t multiplier;
    uint32_t num_levels;
    double one_way_offset_pct;
    double tick_size;
    price_t lower_price_bound;
    price_t upper_price_bound;
    priceLevel* levels;
    priceLevel* best_bid;
    priceLevel* best_ask;
};
typedef struct _orderBook orderBook;

int init_order_book(orderBook* order_book);

orderBook* new_order_book(const char* symbol, 
                          price_t init_price, 
                          uint32_t multiplier,
                          double one_way_offset_pct, 
                          double tick_size);


int addOrder(orderBook* ob,
             const b_side side,
             const char* order_id,
             const price_t price,
             const quantity_t quantity); 

int delOrder(orderBook* ob,
             const b_side side,
             const char* order_id,
             const price_t price,
             const quantity_t quantity);

int modOrder(orderBook* ob,
             const b_side side,
             const char* order_id,
             const price_t new_price,
             const quantity_t new_quantity);


const priceLevel* best_bid(orderBook* ob);

int validate_init_price(orderBook* ob);
const priceLevel* best_ask(orderBook* ob);

void dump(orderBook* ob);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus__
#endif /* defined(__OrderBook_v3__order_book__) */
