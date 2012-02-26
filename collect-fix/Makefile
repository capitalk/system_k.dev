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
PROTODIR=../proto

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g # -DDEBUG_BOOK
endif

LDFLAGS=-static

ORDERBOOK_DIR=../order_book

all: collect

collect : collect.o Application.o  libOrderBook2.a spot_fx_md_1.pb.o capk_globals.pb.o
	$(CC) $(CFLAGS) collect.o spot_fx_md_1.pb.o capk_globals.pb.o Application.o  $(ORDERBOOK_DIR)/libOrderBook2.a $(LIBS) -o $@ 

spot_fx_md_1.pb.o: $(PROTODIR)/spot_fx_md_1.pb.cc $(PROTODIR)/spot_fx_md_1.pb.h 
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTODIR)/spot_fx_md_1.pb.cc

capk_globals.pb.o: $(PROTODIR)/capk_globals.pb.cc $(PROTODIR)/capk_globals.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTODIR)/capk_globals.pb.cc

collect.o: collect.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

Application.o: Application.cpp
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) Application.cpp  

libOrderBook2.a:
	cd $(ORDERBOOK_DIR) && make -f Makefile.book2 lib

#libOrderBook.a:
#	cd $(ORDERBOOK_DIR) && make
protos: $(PROTODIR)/spot_fx_md_1.proto
	$(PROTOC) -I$(PROTODIR) --cpp_out=$(PROTODIR) $(PROTODIR)/*.proto

clean :
	rm -f *.o collect $(PROTODIR)/*.cc $(PROTODIR)/*.h
	#cd $(ORDERBOOK_DIR) && make clean 

install :
	cp -r ./cfg /usr/local/capk
	cp ./collect /usr/local/capk
	cp *.sh /usr/local/capk
.PHONY : collect orderBook


