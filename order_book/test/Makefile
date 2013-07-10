CC=g++

SYSK_ROOT=../..
COMMON=$(SYSK_ROOT)/common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)
PROTO=$(COMMON)/proto
BOOST=/opt/src/boost
OB_INCLUDE=../include
OB_LIBDIR=../.libs

SRC=..

INCLUDE= -I $(SRC) \
    -I $(COMMON) \
		-I $(UTILS) \
		-I $(OB_INCLUDE) \
    -I $(BOOST)

LIBS=-L$(SRC) \
	 -L$(UTILS) \
	 -L$(OB_LIBDIR) \
	 -lquickfix \
	 -lboost_date_time \
   -lboost_system \
	 -lgtest 

CFLAGS=-Wall  -std=c++0x -ggdb -DDEBUG

all: orderbook-0.0.0.a unit_test perf_test

orderbook-0.0.0.a: 
	cd .. && make 

unit_test : orderbook-0.0.0.a unit_test.o 
	$(CC) unit_test.o  $(LIBS) -lorderbook-0.0.0 -o $@   

unit_test.o: unit_test.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) unit_test.cpp

perf_test : orderbook-0.0.0.a perf_test.o 
	$(CC) perf_test.o  $(LIBS) -lorderbook-0.0.0 -o $@   

perf_test.o: perf_test.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) perf_test.cpp


clean :
	rm -f  *.o perf_test unit_test

.PHONY: clean 


