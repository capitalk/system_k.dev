CC=g++

CAPK_ROOT=~/capitalk
COMMON=$(CAPK_ROOT)/common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)
PROTO=$(COMMON)/proto
BOOST=/opt/src/boost

SRC=..

INCLUDE= -I $(SRC) \
    -I $(COMMON) \
		-I $(UTILS) \
    -I $(BOOST)

LIBS=-L$(SRC) \
	 -L$(UTILS) \
	 -lquickfix \
	 -lboost_date_time \
   -lboost_system \
	 -lgtest 

CFLAGS=-Wall  -std=c++0x -ggdb -DDEBUG

all: liborder_book2.a perf_test unit_test

liborder_book2.a: 
	cd .. && make 

unit_test : liborder_book2.a unit_test.o 
	$(CC) unit_test.o  $(LIBS) -lorder_book2 -o $@   

unit_test.o: unit_test.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) unit_test.cpp

perf_test : liborder_book2.a perf_test.o 
	$(CC) perf_test.o  $(LIBS) -lorder_book2 -o $@   

perf_test.o: perf_test.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) perf_test.cpp


clean :
	rm -f  *.o perf_test unit_test

.PHONY: clean 


