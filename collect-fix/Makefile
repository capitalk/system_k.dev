CC=g++

INCLUDE=-I/usr/local/boost/ -I/usr/local/include/ \
        -I/usr/local/src/quickfix/include -I.. -I../orderBook2
LIBS= -L/usr/local/src/quickfix/lib/ -L/usr/local/lib \
	  -L../order_book/ \
      -lquickfix -lboost_date_time -lboost_program_options -lboost_filesystem -lOrderBook2 -lzmq -lprotobuf
	#-lquickfix -lboost_date_time -lboost_program_options -lboost_filesystem -lOrderBook

CFLAGS=-Wall -std=c++0x -pthread -O3
LDFLAGS=-static

PROTOC=protoc
PROTO=../proto

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g # -DDEBUG_BOOK
endif

LDFLAGS=-static

ORDERBOOK_DIR=../order_book

all: collect

collect : collect.o Application.o  libOrderBook2.a spot_fx_md_1.pb.o
	$(CC) $(CFLAGS) collect.o spot_fx_md_1.pb.o Application.o $(ORDERBOOK_DIR)/libOrderBook2.a $(LIBS) -o $@ 

spot_fx_md_1.pb.o: $(PROTO)/spot_fx_md_1.pb.cc $(PROTO)/spot_fx_md_1.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/spot_fx_md_1.pb.cc

collect.o: collect.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

Application.o: Application.cpp
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) Application.cpp  

libOrderBook2.a:
	cd $(ORDERBOOK_DIR) && make -f Makefile.book2 lib

#libOrderBook.a:
#	cd $(ORDERBOOK_DIR) && make
protos: $(PROTO)/spot_fx_md_1.proto
	$(PROTOC) -I$(PROTO) --cpp_out=$(PROTO) $(PROTO)/*.proto

clean :
	rm -f *.o collect $(PROTO)/*.cc $(PROTO)/*.h
	#cd $(ORDERBOOK_DIR) && make clean 

install :
	cp -r ./cfg /usr/local/capk
	cp ./collect /usr/local/capk
	cp *.sh /usr/local/capk
.PHONY : collect orderBook


