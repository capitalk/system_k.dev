CC=g++

COMMON=../../../common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)/utils
PROTO=$(COMMON)/proto

ORDERBOOK_DIR=../../order_book/order_book.v2
ORDERBOOK_INC=../../order_book


INCLUDE= -I$(ORDERBOOK_INC) \
		-I$(UTILS) \
		-I$(COMMON)

LIBS= -L$(ORDERBOOK_DIR)/ \
      	-lquickfix \
		-lboost_date_time \
		-lboost_program_options \
		-lboost_filesystem \
		-lorder_book2 \
		-lzmq \
		-lprotobuf \
		-lgtest

CFLAGS=-Wall -std=c++0x -pthread -O3 -ULOG -DNDEBUG
#LDFLAGS=-static

PROTOC=protoc

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g -DLOG# -DDEBUG_BOOK
endif

#LDFLAGS=-static


all: protos collect

include $(PROTO)/Makefile.inc

collect : Application.o  config_server.o jenkins_hash.o liborder_book2.a time_utils.o collect.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) config_server.o jenkins_hash.o time_utils.o Application.o collect.o $(PROTOBUF_OBJS) $(ORDERBOOK_DIR)/liborder_book2.a $(LIBS) -o $@ 

jenkins_hash.o: $(UTILS_SRC)/jenkins_hash.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

config_server.o: $(UTILS_SRC)/config_server.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

time_utils.o: $(UTILS_SRC)/time_utils.cpp
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
	rm -f *.pid
	cd $(PROTO) && make clean
	cd $(ORDERBOOK_DIR) && make clean

install :
	cp -r ./cfg /usr/local/capk
	cp ./collect /usr/local/capk
	cp *.sh /usr/local/capk

.PHONY : collect 


