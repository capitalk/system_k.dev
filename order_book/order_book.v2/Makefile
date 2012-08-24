CC=g++

COMMON=../../../common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)/utils
PROTO=$(COMMON)/proto


INCLUDE=-I/usr/local/boost/ \
		-I/usr/local/include/ \
		-I/usr/local/src/quickfix/include \
		-I../gtest/include \
		-I..\
	   	-I. \
		-I../.. \
		-I$(COMMON)\
		-I$(UTILS)

LIBS=-L../utils \
	 -L/usr/local/src/quickfix/lib/ \
	 -L/usr/local/boost_1_45_0/stage/lib/ \
	 -L../gtest/lib/.libs \
	 -Wl,-rpath,../gtest/lib/.libs \
	 -lquickfix \
	 -lboost_date_time \
	 -lgtest 

CFLAGS=-Wall  -std=c++0x -O3 # -g -D DEBUG

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $(input) -o $(output)

ifeq ($(dbg),1)
	CFLAGS = -Wall  -std=c++0x -ggdb -g  -DDEBUG
endif

all: liborder_book2.a

liborder_book2.a: order.o order_book.o limit.o time_utils.o fix_convertors.o
	ar rcs liborder_book2.a $+

order.o: order.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) order.cpp 

order_book.o: order_book.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) order_book.cpp

limit.o: limit.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) limit.cpp

time_utils.o: $(UTILS_SRC)/time_utils.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $(UTILS_SRC)/time_utils.cpp

fix_convertors.o: $(UTILS_SRC)/fix_convertors.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $(UTILS_SRC)/fix_convertors.cpp

clean :
	rm -f  *.o liborder_book2.a 




