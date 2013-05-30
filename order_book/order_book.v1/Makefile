CC=g++

CAPK_ROOT=~/capitalk
COMMON=$(CAPK_ROOT)/common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)/utils
PROTO=$(COMMON)/proto

INCLUDE= -I $(COMMON) \
		-I $(UTILS)

LIBS= -lquickfix \
	 -lboost_date_time \
   -lboost_system \
	 -lgtest 

CFLAGS=-Wall  -std=c++0x -O3

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $(input) -o $(output)

ifeq ($(dbg),1)
	CFLAGS = -Wall  -std=c++0x -ggdb -g  -DDEBUG
endif

all: liborder_book.a

liborder_book.a: order_book_entry.o order_book.o price_depth_entry.o price_depth_order_book.o 
	ar rcs liborder_book.a $+

price_depth_entry.o: price_depth_entry.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) price_depth_entry.cpp 

price_depth_order_book.o: price_depth_order_book.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) price_depth_order_book.cpp

order_book.o: order_book.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) order_book.cpp

order_book_entry.o:  order_book_entry.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) order_book_entry.cpp 

test: liborder_book.a
	cd./test && make


clean :
	rm -f  *.o liborder_book.a 




