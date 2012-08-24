CC=g++

UTILS=../utils

INCLUDE=-I/usr/local/boost/ -I/usr/local/include/ -I/usr/local/src/quickfix/include -I../gtest/include -I../ -I./ -I$(UTILS)

LIBS=-L../utils -L/usr/local/src/quickfix/lib/ -L/usr/local/boost_1_45_0/stage/lib/ -L../gtest/lib/.libs -Wl,-rpath,../gtest/lib/.libs -lquickfix -lboost_date_time -lgtest 

CFLAGS=-Wall  -std=c++0x -O3 # -g -D DEBUG

OB1=./order_book.v1
OB2=./order_book.v2
TESTS=./tests

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $(input) -o $(output)


all: libOrderBook2.a libOrderBook.a test

tests: book_tests book_tests2 hashId convertorTest ts2fix

libOrderBook.a: 
	cd $(OB1) && make 

libOrderBook2.a: 
	cd $(OB2) && make

test: 
	cd $(TESTS) && make

testclean:
	cd $(TESTS) && make clean	

clean :
	cd $(TESTS) && make clean
	cd $(OB1) && make clean
	cd $(OB2) && make clean


.PHONY: all tests test testclean clean


