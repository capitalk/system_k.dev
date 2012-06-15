CC=g++

ORDERBOOK=../order_book/order_book.v2

ORDERBOOK_DIR=../order_book/order_book.v2

UTILS_DIR=../utils

INCLUDE=-I/usr/local/boost/ \
		-I/usr/local/include/ \
        -I/usr/local/src/quickfix/include \
		-I.. \
		-I/usr/local/include/gtest/ \
		-I$(UTILS_DIR)

LIBS= -L/usr/local/src/quickfix/lib/ \
	  -L/usr/local/lib \
      -L/usr/local/boost_1_45_0/stage/lib/ \
      -lquickfix \
	  -lboost_date_time \
	  -lboost_program_options \
	  -lboost_thread \
	  -lgtest

CFLAGS=-Wall -std=c++0x -pthread

LDFLAGS=-static

ifeq ($(dbg),1)
		status = '*** BUILDING IN DEBUG ***'
		CFLAGS += -DDEBUG -ggdb -g # -DDEBUG_BOOK
endif


LDFLAGS=-static


all: status exchange 

exchange : executor.o Application.o msg_pump.o msg_dispatcher.o fix_convertors.o session_info.o libOrderBook.a 
	$(CC) $(CFLAGS) executor.o Application.o msg_pump.o msg_dispatcher.o session_info.o fix_convertors.o $(ORDERBOOK_DIR)/liborder_book2.a $(LIBS) -o $@ 


session_info.o: session_info.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $?

fix_convertors.o: $(UTILS_DIR)/fix_convertors.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $? 

msg_dispatcher.o: msg_dispatcher.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $?

executor.o: executor.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

Application.o: Application.cpp
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $?  

libOrderBook.a:
	cd $(ORDERBOOK_DIR) && make

msg_pump.o: msg_pump.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $?

session_info: session_info.o session_info.o msg_pump.o msg_dispatcher.o fix_convertors.o
	$(CC)  session_info.o session_info.o msg_pump.o msg_dispatcher.o fix_convertors.o $(LIBS) -o $@ 


clean :
	rm -f *.o exchange 

status: 
	@echo $(status)



