CC=g++

CAPK_ROOT=~/capitalk
COMMON=$(CAPK_ROOT)/common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)/utils
PROTO=$(COMMON)/proto


INCLUDE= -I $(COMMON)\
		-I $(UTILS)

LIBS= -lquickfix \
	 -lboost_date_time \
	 -lgtest 

CFLAGS=-Wall  -std=c++0x -O3 

ifeq ($(dbg),1)
	CFLAGS = -Wall  -std=c++0x -ggdb -g  -DDEBUG
endif

all: liborder_book2.a

liborder_book2.a: order.o order_book.o limit.o time_utils.o fix_convertors.o
	ar rcs liborder_book2.a $+

order.o: order.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $^ 

order_book.o: order_book.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $^

limit.o: limit.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $^

time_utils.o: $(UTILS_SRC)/time_utils.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $^

fix_convertors.o: $(UTILS_SRC)/fix_convertors.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $^

clean :
	rm -f  *.o liborder_book2.a 




