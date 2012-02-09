CC=g++

INCLUDE=-I/usr/local/boost/ -I/usr/local/include/ \
        -I/usr/local/src/quickfix/include -I.. -I../orderBook2
LIBS= -L/usr/local/src/quickfix/lib/ -L/usr/local/lib \
	  -L../order_book/ \
      -lquickfix -lboost_date_time -lboost_program_options -lboost_filesystem #-lOrderBook2 -lzmq -lprotobuf

CFLAGS=-Wall -std=c++0x -pthread -O3
LDFLAGS=-static

PROTOC=protoc
PROTO=../proto

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g # -DDEBUG_BOOK
endif

LDFLAGS=-static

#ORDERBOOK_DIR=../order_book

all: validate_symbols

validate_symbols : validate_symbols.o Application.o 
	$(CC) $(CFLAGS) validate_symbols.o Application.o  $(LIBS) -o $@ 

validate_symbols.o: validate_symbols.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

Application.o: Application.cpp
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) Application.cpp  

clean :
	rm -f *.o validate_symbols

install :
	cp -r ./cfg /usr/local/capk
	cp ./validate_symbols /usr/local/capk
	cp *.sh /usr/local/capk
.PHONY : validate_symbols orderBook


