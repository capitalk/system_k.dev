CC=g++

CAPK_ROOT=~/capitalk
COMMON=$(CAPK_ROOT)/common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)/utils
PROTO=$(COMMON)/proto

ORDERBOOK_V1=../order_book.v1
ORDERBOOK_V2=../order_book.v2

INCLUDE= -I $(ORDERBOOK_V1) \
		-I $(ORDERBOOK_V2) \
		-I $(COMMON) \
		-I $(UTILS)

LIBS=-L$(ORDERBOOK_V1) \
	 -L$(ORDERBOOK_V2) \
	 -lquickfix \
	 -lboost_date_time \
	 -lboost_system \
	 -lgtest \
	 -lorder_book \
	 -lorder_book2

CFLAGS=-Wall  -std=c++0x -O3 # -g -D DEBUG

all: liborder_book.a liborder_book2.a test_hash_id test_convertor test_timespec_to_fix

liborder_book.a:
	cd ../order_book.v1 && make 

liborder_book2.a: 
	cd ../order_book.v2 && make 

test_hash_id:
	$(CC) $(CFLAGS) test_hash_id.cpp $(UTILS_SRC)/fix_convertors.cpp $(UTILS_SRC)/time_utils.cpp -I../  $(INCLUDE) -L $(UTILS_SRC) $(LIBS) -o $@

test_convertor: 
	$(CC) $(CFLAGS) test_convertor.cpp $(UTILS_SRC)/fix_convertors.cpp $(UTILS_SRC)/time_utils.cpp -I../  $(INCLUDE) -L $(UTILS_SRC) $(LIBS) -o $@
	
test_timespec_to_fix: 
	$(CC) $(CFLAGS) test_timespec_to_fix.cpp $(UTILS_SRC)/fix_convertors.cpp $(UTILS_SRC)/time_utils.cpp -I../  $(INCLUDE) -L $(UTILS_SRC) $(LIBS) -o $@
	
testclean:
	rm -f hash_id test_convertor	
clean :
	rm -f  *.o test_convertor hash_id test_timespec_to_fix test_hash_id

.PHONY: clean testclean


