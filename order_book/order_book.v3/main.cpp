//
//  main.cpp
//  OrderBook.v3
//
//  Created by Timir Karia on 12/22/12.
//  Copyright (c) 2012 Timir Karia. All rights reserved.
//

#include <iostream>
#include "order_book.h"
#include "order_book.hxx"

int main(int argc, const char * argv[])
{
    
    // insert code here...
    std::cout << "Creating order_book" << std::endl;
    //order_book ob(center_bid, percent_price_bound, tick_size)
    order_book ob(1.000, 0.01, 0.00001);
    // allocate space - validate boundaries
    
    ob.addBid(1.01, 998929, "abc");
    ob.addAsk(1.011, 12345, "cde");
    ob.getBestBid();
    ob.getBestAsk();
    return 0;
}

