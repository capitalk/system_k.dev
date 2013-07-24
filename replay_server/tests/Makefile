CC=g++

ORDERBOOK=../../order_book/order_book.v2
ORDERBOOK_DIR=../../order_book/order_book.v2

UTILS_DIR=../../utils
SRC_DIR=..

INCLUDE=-I/usr/local/boost/ \
		-I/usr/local/include/ \
        -I/usr/local/src/quickfix/include \
		-I.. \
		-I../.. \
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


all: test_msg_pump test_session_info test_fix_convertors


SessionInfo.o: SessionInfo.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) SessionInfo.cpp

FIXConvertors.o: $(UTILS_DIR)/FIXConvertors.cpp $(UTILS_DIR)/FIXConvertors.h
	cd $(UTILS_DIR) && $(CC) -c $(INCLUDE) $(CFLAGS) $(UTILS_DIR)/FIXConvertors.cpp $(UTILS_DIR)/FIXConvertors.h

msg_dispatcher.o: $(SRC_DIR)/msg_dispatcher.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $?

session_info.o: $(SRC_DIR)/session_info.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $?

libOrderBook.a:
	cd $(ORDERBOOK_DIR) && make

msg_pump.o: $(SRC_DIR)/msg_pump.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $(SRC_DIR)/msg_pump.cpp

test_msg_pump: test_msg_pump.o msg_pump.o
	$(CC) test_msg_pump.o msg_pump.o $(LIBS) -o $@

test_msg_pump.o: test_msg_pump.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $? 

test_session_info: test_session_info.o session_info.o msg_pump.o msg_dispatcher.o $(UTILS_DIR)/FIXConvertors.o
	$(CC)  test_session_info.o session_info.o msg_pump.o msg_dispatcher.o $(LIBS) $(UTILS_DIR)/FIXConvertors.o -o $@ 

test_session_info.o: test_session_info.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $? 

test_fix_convertors: test_fix_convertors.o 
	$(CC)  test_fix_convertors.o $(LIBS) -o $@

test_fix_convertors.o: test_fix_convertors.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $? 

clean :
	rm -f *.o test_msg_pump test_session_info test_fix_convertors

status: 
	@echo $(status)



