CC=g++

PROTO=../proto

ORDERBOOK_DIR=../order_book/order_book.v2
ORDERBOOK_INC=../order_book

UTILS_DIR=../utils

INCLUDE=-I/usr/local/boost/ \
		-I/usr/local/include/ \
        -I/usr/local/src/quickfix/include \
		-I.. \
		-I../.. \
		-I$(ORDERBOOK_INC) \
		-I$(UTILS) \

LIBS= -L/usr/local/src/quickfix/lib/ \
		-L/usr/local/lib \
		-L$(ORDERBOOK_DIR)/ \
      	-lquickfix \
		-lboost_date_time \
		-lboost_program_options \
		-lboost_filesystem \
		-lorder_book2 \
		-lzmq \
		-lprotobuf \
		-lgtest

CFLAGS=-Wall -std=c++0x -pthread -O3 -ULOG -DNDEBUG
LDFLAGS=-static

PROTOC=protoc

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g -DLOG# -DDEBUG_BOOK
endif

LDFLAGS=-static


all: protos collect

include $(PROTO)/Makefile.inc

collect : Application.o  venue_utils.o jenkins_hash.o liborder_book2.a time_utils.o collect.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) venue_utils.o jenkins_hash.o time_utils.o Application.o collect.o $(PROTOBUF_OBJS) $(ORDERBOOK_DIR)/liborder_book2.a $(LIBS) -o $@ 

jenkins_hash.o: $(UTILS_DIR)/jenkins_hash.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

venue_utils.o: $(UTILS_DIR)/venue_utils.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

time_utils.o: $(UTILS_DIR)/time_utils.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

collect.o: collect.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

Application.o: Application.cpp
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) Application.cpp  


liborder_book2.a:
	cd $(ORDERBOOK_DIR) && make

protos: 
	cd $(PROTO) && make

clean :
	rm -f *.o collect 
	cd $(PROTO) && make clean

install :
	cp -r ./cfg /usr/local/capk
	cp ./collect /usr/local/capk
	cp *.sh /usr/local/capk

.PHONY : collect orderBook


