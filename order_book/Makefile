CC=g++

UTILS=../utils

INCLUDE=-I/usr/local/boost/ -I/usr/local/include/ -I/usr/local/src/quickfix/include -I../gtest/include -I../ -I./ -I$(UTILS)

#LIBS=-L../utils --L/usr/local/src/quickfix/lib -L/usr/local/boost_1_45_0/stage/lib/ -lquickfix -lboost_date_time -lgtest -rpath=/usr/local/src/quickfix/lib

LIBS=-L../utils -L/usr/local/src/quickfix/lib/ -L/usr/local/boost_1_45_0/stage/lib/ -L../gtest/lib/.libs -Wl,-rpath,../gtest/lib/.libs -lquickfix -lboost_date_time -lgtest 

CFLAGS=-Wall  -std=c++0x -O3 # -g -D DEBUG

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $(input) -o $(output)


all: libOrderBook2.a libOrderBook.a

tests: book_tests book_tests2 hashId convertorTest ts2fix

libOrderBook.a: OrderBookEntry.o KOrderBook.o PriceDepthEntry.o KPriceDepthOrderBook.o 
	ar rcs libOrderBook.a $+

libOrderBook2.a: KOrder.o KBook.o KLimit.o KTimeUtils.o FIXConvertors.o
	ar rcs libOrderBook2.a $+

book_tests : libOrderBook.a book_tests.o PriceDepthEntry.o
	$(CC) book_tests.o -L. -lOrderBook $(LIBS) -o $@   

book_tests.o: book_tests.cpp PriceDepthEntry.o
	$(CC) -c $(INCLUDE) $(CFLAGS) book_tests.cpp

PriceDepthEntry.o: PriceDepthEntry.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) PriceDepthEntry.cpp 

KPriceDepthOrderBook.o: KPriceDepthOrderBook.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) KPriceDepthOrderBook.cpp

KOrderBook.o: KOrderBook.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) KOrderBook.cpp

OrderBookEntry.o:  OrderBookEntry.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) OrderBookEntry.cpp 

book_tests2 : KTimeUtils.o KBook.o KOrder.o KLimit.o book_tests2.o  FIXConvertors.o
	$(CC) KTimeUtils.o KBook.o KOrder.o KLimit.o book_tests2.o FIXConvertors.o -L. $(LIBS) -o $@   

book_tests2.o: book_tests2.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) book_tests2.cpp

KOrder.o: KOrder.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) KOrder.cpp 

KBook.o: KBook.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) KBook.cpp

KLimit.o: KLimit.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) KLimit.cpp

KTimeUtils.o: $(UTILS)/KTimeUtils.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $(UTILS)/KTimeUtils.cpp

FIXConvertors.o: ../utils/FIXConvertors.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $(UTILS)/FIXConvertors.cpp


hashId:
	$(CC) $(CFLAGS) hashId.cpp $(UTILS)/FIXConvertors.cpp $(UTILS)/KTimeUtils.cpp -I../  $(INCLUDE) -L $(UTILS) $(LIBS) -o $@

convertorTest: 
	$(CC) $(CFLAGS) convertortest.cpp $(UTILS)/FIXConvertors.cpp $(UTILS)/KTimeUtils.cpp -I../  $(INCLUDE) -L $(UTILS) $(LIBS) -o $@
	
ts2fix: 
	$(CC) $(CFLAGS) ts2fix.cpp $(UTILS)/FIXConvertors.cpp $(UTILS)/KTimeUtils.cpp -I../  $(INCLUDE) -L $(UTILS) $(LIBS) -o $@
	
testclean:
	rm -f hashId convertorTest	
clean :
	rm -f  *.o book_tests2 *.a *.o book_tests libOrderBook.a libOrderBook2.a hashId convertorTest




