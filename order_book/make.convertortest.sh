#!/bin/bash
g++ -std=c++0x -O3 convertortest.cpp ../utils/FIXConvertors.cpp ../utils/KTimeUtils.cpp -I../ -I../utils -I/usr/local/boost -I../gtest/include/gtest -L ../utils/ -L/usr/local/src/quickfix/lib -L/usr/local/boost/stage/lib  -lquickfix -lrt -lgtest -lboost_date_time -o convertortest
