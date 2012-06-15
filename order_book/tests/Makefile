CC=g++

UTILS=../../utils

ORDERBOOK_V1=../order_book.v1
ORDERBOOK_V2=../order_book.v2

INCLUDE=-I/usr/local/boost/ \
		-I/usr/local/include/ \
		-I/usr/local/src/quickfix/include \
		-I.. \
	   	-I. \
		-I../.. \
		-I$(ORDERBOOK_V1) \
		-I$(ORDERBOOK_V2) \
		-I$(UTILS)

LIBS=-L$(ORDERBOOK_V1) \
	 -L$(ORDERBOOK_V2) \
	 -L$(UTILS) \
	 -L/usr/local/src/quickfix/lib/ \
	 -L/usr/local/boost_1_45_0/stage/lib/ \
	 -lquickfix \
	 -lboost_date_time \
	 -lgtest \
	 -lorder_book \
	 -lorder_book2

CFLAGS=-Wall  -std=c++0x -O3 # -g -D DEBUG

#%.o: %.cpp
	#$(CC) $(CFLAGS) $(INCLUDE) -c $(input) -o $(output)


all: liborder_book.a liborder_book2.a book_tests book_tests2 test_hash_id test_convertor test_timespec_to_fix

liborder_book.a:
	cd ../order_book.v1 && make 

liborder_book2.a: 
	cd ../order_book.v2 && make 

# Version 1 test
book_tests : liborder_book.a book_tests.o
	$(CC) book_tests.o  $(LIBS) -o $@   

book_tests.o: book_tests.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) book_tests.cpp

# Version 2 test
book_tests2 : liborder_book2.a book_tests2.o 
	$(CC) book_tests2.o  $(LIBS) -o $@   

book_tests2.o: book_tests2.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) book_tests2.cpp


# Other tests
test_hash_id:
	$(CC) $(CFLAGS) test_hash_id.cpp $(UTILS)/fix_convertors.cpp $(UTILS)/time_utils.cpp -I../  $(INCLUDE) -L $(UTILS) $(LIBS) -o $@

test_convertor: 
	$(CC) $(CFLAGS) test_convertor.cpp $(UTILS)/fix_convertors.cpp $(UTILS)/time_utils.cpp -I../  $(INCLUDE) -L $(UTILS) $(LIBS) -o $@
	
test_timespec_to_fix: 
	$(CC) $(CFLAGS) test_timespec_to_fix.cpp $(UTILS)/fix_convertors.cpp $(UTILS)/time_utils.cpp -I../  $(INCLUDE) -L $(UTILS) $(LIBS) -o $@
	
testclean:
	rm -f hash_id test_convertor	
clean :
	rm -f  *.o test_convertor hash_id test_timespec_to_fix book_tests book_tests2 test_hash_id




