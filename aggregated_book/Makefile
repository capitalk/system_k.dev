CC=g++

PROTO=../proto
UTILS=../utils

INCLUDE=-I/usr/local/boost/ \
		-I/usr/local/include/ \
        -I/usr/local/src/quickfix/include \
		-I.. \
		-I$(UTILS)

LIBS= -L/usr/local/src/quickfix/lib/ \
		-L/usr/local/lib \
		-L../order_book/ \
      	-lquickfix \
		-lboost_date_time \
		-lboost_thread \
		-lboost_program_options \
		-lboost_filesystem \
		-lzmq \
		-lprotobuf \

CFLAGS=-Wall -std=c++0x -pthread -O3 
LDFLAGS=-static

PROTOC=protoc

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g # -DDEBUG_BOOK
endif

LDFLAGS=-static

ORDERBOOK_DIR=../order_book

aggregated_book : aggregated_book.o spot_fx_md_1.pb.o KTimeUtils.o
	$(CC) $(CFLAGS) aggregated_book.o spot_fx_md_1.pb.o KTimeUtils.o $(LIBS) -o $@ 

aggregated_book.o: aggregated_book.cpp
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) aggregated_book.cpp 

spot_fx_md_1.pb.o: $(PROTO)/spot_fx_md_1.pb.cc $(PROTO)/spot_fx_md_1.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/spot_fx_md_1.pb.cc 

KTimeUtils.o: $(UTILS)/KTimeUtils.cpp $(UTILS)/KTimeUtils.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(UTILS)/KTimeUtils.cpp

protos : $(PROTO)/spot_fx_md_1.proto 
	protoc -I $(PROTO)  --cpp_out=$(PROTO) $(PROTO)/*.proto

clean :
	rm -f *.o aggregated_book

install :
	cp -r ./cfg /usr/local/capk
	cp ./collect /usr/local/capk
	cp *.sh /usr/local/capk
.PHONY : collect orderBook


