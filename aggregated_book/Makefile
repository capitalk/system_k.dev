CC=g++

PROTO=../proto
UTILS=../utils

INCLUDE=-I/usr/local/boost/ \
		-I/usr/local/include/ \
        -I/usr/local/src/quickfix/include \
		-I.. \
		-I$(UTILS) \
		-I$(PROTO)

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

CFLAGS=-Wall -std=c++0x -pthread -O3 -ULOG -DNDEBUG


ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g # -DDEBUG_BOOK
endif

PROTOBUF_OBJS = spot_fx_md_1.pb.o \
				capk_globals.pb.o 

all: protos aggregated_book

aggregated_book.o: aggregated_book.cpp
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) aggregated_book.cpp 

spot_fx_md_1.pb.o: $(PROTO)/spot_fx_md_1.pb.cc $(PROTO)/spot_fx_md_1.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/spot_fx_md_1.pb.cc 

capk_globals.pb.o: $(PROTO)/capk_globals.pb.cc $(PROTO)/capk_globals.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/capk_globals.pb.cc

time_utils.o: $(UTILS)/time_utils.cpp $(UTILS)/time_utils.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(UTILS)/time_utils.cpp

venue_utils.o: $(UTILS)/venue_utils.cpp $(UTILS)/venue_utils.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(UTILS)/venue_utils.cpp


aggregated_book : aggregated_book.o time_utils.o venue_utils.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) aggregated_book.o time_utils.o venue_utils.o $(PROTOBUF_OBJS) $(LIBS) -o $@ 

protos : 
	cd $(PROTO) && make 

clean :
	rm -f *.o aggregated_book

install :
	cp -r ./cfg /usr/local/capk
	cp ./collect /usr/local/capk
	cp *.sh /usr/local/capk
.PHONY : collect orderBook


