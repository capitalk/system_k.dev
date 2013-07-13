CC=g++

SYSK_ROOT=../..
COMMON=$(SYSK_ROOT)/common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)/utils
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

CFLAGS=-Wall -std=c++0x -ggdb -DDEBUG

all: unit_test perf_test

unit_test : unit_test.o 
	$(CC) unit_test.o  $(LIBS) -Bstatic -lorderbook-0.0.0 -o $@   

unit_test.o: unit_test.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) unit_test.cpp

perf_test : perf_test.o  fix_convertors.o
	$(CC) perf_test.o fix_convertors.o $(LIBS) -lorderbook-0.0.0 -o $@   

fix_convertors.o: $(UTILS_SRC)/fix_convertors.cpp 
	$(CC) $(CFLAGS) -c $(INCLUDE) $(UTILS_SRC)/fix_convertors.cpp

perf_test.o: perf_test.cpp 
	$(CC) $(CFLAGS) -c $(INCLUDE) perf_test.cpp


clean :
	rm -f  *.o perf_test unit_test

.PHONY: clean 


