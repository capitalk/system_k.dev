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

extern "C" {

struct orderBook* new_order_book(double init_price,
        double one_way_offset_pct,
        double min_tick_size) {
    // TODO make sure that init_price conforms to tick or round it 
    double abs_price_range = init_price * one_way_offset_pct;
    
    size_t one_way_levels = 
        ((init_price + abs_price_range)/min_tick_size);

    size_t levels_size = 
        one_way_levels * sizeof(priceLevel);
    
    double lower_price_bound = 
        init_price - (one_way_levels * min_tick_size);

    double upper_price_bound = 
        init_price + (one_way_levels * min_tick_size);
    
    priceLevel* levels = (priceLevel*)malloc(levels_size);
    orderBook* book = (orderBook*)malloc(sizeof(orderBook));
    
    memset(book, 0, sizeof(orderBook));
    memset(levels, 0, levels_size);
    
    book->init_price = init_price;
    book->one_way_offset_pct = one_way_offset_pct;
    book->min_tick_size = min_tick_size;
    book->lower_price_bound = lower_price_bound;
    book->upper_price_bound = upper_price_bound;
    book->levels = levels;
    
    if (book == NULL || levels == NULL) {
        // error
    }
    else {
        init_order_book(book);
    }
    return (orderBook*)book;
    
    
};

int init_order_book(orderBook* order_book) {
    for (int i = 0; i<order_book->num_levels; i++) {
        order_book->levels[i]._price = order_book->lower_price_bound + (i * order_book->min_tick_size);
    }
    return 0;
};

int add_order(double price, double quantity, const char* order_id) {
    return 0;
    
};

int del_order(double price, double quantity, const char* order_id) {
    return 0;
    
};


struct price_level* best_bid() {
    return NULL;
};

struct price_level* best_ask() {
    return NULL;
};

} // extern "C"
