#!/bin/bash
g++ -std=c++0x -O3 hashId.cpp ../utils/FIXConvertors.cpp ../utils/KTimeUtils.cpp -I../ -I../utils -I/usr/local/boost -L ../utils/ -L/usr/local/src/quickfix/lib -L/usr/local/boost/stage/lib  -lquickfix -lrt  -lboost_date_time -o hashId
