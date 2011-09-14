CC=g++

INCLUDE=-I/usr/local/boost/ -I/usr/local/include/ -I/usr/local/src/quickfix/include -I../gtest/include -I../ -I./ -I../utils

LIBS=-L../utils --L/usr/local/src/quickfix/lib -L/usr/local/boost_1_45_0/stage/lib/ -lquickfix -lboost_date_time -lgtest -rpath=/usr/local/src/quickfix/lib

CFLAGS=-Wall  -std=c++0x -O3 # -g -D DEBUG


all: book_tests libOrderBook.a

libOrderBook.a: OrderBookEntry.o KOrderBook.o PriceDepthEntry.o KPriceDepthOrderBook.o 
	ar rcs libOrderBook.a $+

book_tests : libOrderBook.a book_tests.o PriceDepthEntry.o
	$(CC) book_tests.o -L. -lOrderBook $(LIBS) -o $@   

book_tests.o: book_tests.cpp PriceDepthEntry.o
	$(CC) -c $(INCLUDE) $(CFLAGS) book_tests.cpp

PriceDepthEntry.o: PriceDepthEntry.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) PriceDepthEntry.cpp 

KPriceDepthOrderBook.o: KPriceDepthOrderBook.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) KPriceDepthOrderBook.cpp

KOrderBook.o: KOrderBook.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) KOrderBook.cpp

OrderBookEntry.o:  OrderBookEntry.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) OrderBookEntry.cpp 

clean :
	rm -f *.a *.o book_tests


