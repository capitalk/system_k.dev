//
//  main.cpp
//  OrderBook.v3
//
//  Created by Timir Karia on 12/22/12.
//  Copyright (c) 2012 Timir Karia. All rights reserved.
//

#include <iostream>
#include "order_book.h"

int main(int argc, const char * argv[])
{

    // insert code here...
    std::cout << "Creating order_book" << std::endl;
    order_book(1.000, 0.01, 0.00001);
    return 0;
}

