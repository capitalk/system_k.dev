CC=g++

ORDERBOOK=../order_book
ORDERBOOK_DIR=../order_book
INCLUDE=-I/usr/local/boost/ -I/usr/local/include/ \
        -I/usr/local/src/quickfix/include -I../ -I/usr/local/include/gtest/ -I../utils

LIBS= -L/usr/local/src/quickfix/lib/ -L/usr/local/lib \
      -L/usr/local/boost_1_45_0/stage/lib/ \
                        -lquickfix -lboost_date_time -lboost_program_options -lboost_thread -lgtest
CFLAGS=-Wall -std=c++0x -pthread
LDFLAGS=-static

ORDERBOOK_OBJS=$(ORDERBOOK)/KOrderBook.o $(ORDERBOOK)/KPriceDepthOrderBook.o $(ORDERBOOK)/PriceDepthEntry.o $(ORDERBOOK)/OrderBookEntry.o
ifeq ($(dbg),1)
		status = '*** BUILDING IN DEBUG ***'
		CFLAGS += -DDEBUG -ggdb -g # -DDEBUG_BOOK
endif

UTILSDIR=../utils

LDFLAGS=-static


all: status exchange MsgPumpTest SessionInfoTest FIXConvertorsTest

exchange : executor.o Application.o MP.o MsgDispatcher.o $(UTILSDIR)/FIXConvertors.o SessionInfo.o libOrderBook.a 
	$(CC) $(CFLAGS) executor.o Application.o MP.o MsgDispatcher.o SessionInfo.o $(UTILSDIR)/FIXConvertors.o $(ORDERBOOK_DIR)/libOrderBook.a $(LIBS) -o $@ 


SessionInfo.o: SessionInfo.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) SessionInfo.cpp

$(UTILSDIR)/FIXConvertors.o: $(UTILSDIR)/FIXConvertors.cpp $(UTILSDIR)/FIXConvertors.h
	cd $(UTILSDIR) && $(CC) -c $(INCLUDE) $(CFLAGS) $(UTILSDIR)/FIXConvertors.cpp $(UTILSDIR)/FIXConvertors.h

MsgPump.o: MsgPump.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) MsgPump.cpp

MsgDispatcher.o: MsgDispatcher.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) MsgDispatcher.cpp

executor.o: executor.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

Application.o: Application.cpp
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) Application.cpp  

libOrderBook.a:
	cd $(ORDERBOOK_DIR) && make

MP.o: MP.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) MP.cpp

MsgPumpTest: MsgPumpTest.o MP.o
	$(CC) MsgPumpTest.o MP.o $(LIBS) -o $@

MsgPumpTest.o: MsgPumpTest.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) MsgPumpTest.cpp 

SessionInfoTest: SessionInfoTest.o SessionInfo.o MP.o MsgDispatcher.o $(UTILSDIR)/FIXConvertors.o
	$(CC)  SessionInfoTest.o SessionInfo.o MP.o MsgDispatcher.o $(LIBS) $(UTILSDIR)/FIXConvertors.o -o $@ 

SessionInfoTest.o: SessionInfoTest.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) SessionInfoTest.cpp 

FIXConvertorsTest: FIXConvertorsTest.o 
	$(CC)  FIXConvertorsTest.o $(LIBS) -o $@

FIXConvertorsTest.o: FIXConvertorsTest.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) FIXConvertorsTest.cpp 

clean :
	rm -f *.o exchange MsgPumpTest SessionInfoTest FIXConvertorsTest
	#cd $(ORDERBOOK_DIR) && make clean 

status: 
	@echo $(status)

.PHONY : executor orderBook


