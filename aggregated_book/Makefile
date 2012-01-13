CC=g++

INCLUDE=-I/usr/local/boost/ -I/usr/local/include/ \
        -I/usr/local/src/quickfix/include -I.. -I../orderBook2 -I../utils
LIBS= -L/usr/local/src/quickfix/lib/ -L/usr/local/lib \
	  -L../order_book/ \
      -lquickfix -lboost_date_time -lboost_program_options -lboost_filesystem -lOrderBook2 -lzmq -lprotobuf -l boost_thread 
	#-lquickfix -lboost_date_time -lboost_program_options -lboost_filesystem -lOrderBook

CFLAGS=-Wall -std=c++0x -pthread -O3 
LDFLAGS=-static

PROTOC=protoc
PROTO=../proto
UTILS=../utils

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g # -DDEBUG_BOOK
endif

LDFLAGS=-static

ORDERBOOK_DIR=../order_book

#debug: CFLAGS += -DDEBUG -g
#debug: LDFLAGS += -DDEBUG -g
#debug: collect

aggregated_book : main.o spot_fx_md_1.pb.o KTimeUtils.o
	$(CC) $(CFLAGS) main.o spot_fx_md_1.pb.o KTimeUtils.o $(LIBS) -o $@ 

main.o: main2.cpp
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) main2.cpp 

spot_fx_md_1.pb.o: $(PROTO)/spot_fx_md_1.pb.cc $(PROTO)/spot_fx_md_1.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/spot_fx_md_1.pb.cc 

KTimeUtils.o: $(UTILS)/KTimeUtils.cpp $(UTILS)/KTimeUtils.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(UTILS)/KTimeUtils.cpp

#libOrderBook2.a:
	#cd $(ORDERBOOK_DIR) && make -f Makefile.book2 lib

#libOrderBook.a:
#	cd $(ORDERBOOK_DIR) && make
#
protos : $(PROTO)/spot_fx_md_1.proto 
	protoc -I $(PROTO)  --cpp_out=$(PROTO) $(PROTO)/*.proto

clean :
	rm -f *.o main
	#cd $(ORDERBOOK_DIR) && make clean 

install :
	cp -r ./cfg /usr/local/capk
	cp ./collect /usr/local/capk
	cp *.sh /usr/local/capk
.PHONY : collect orderBook


