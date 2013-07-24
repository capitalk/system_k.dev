CC=g++

SYSK_ROOT=../../..
COMMON=$(SYSK_ROOT)/common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)
PROTO=$(COMMON)/proto
BOOST=/opt/src/boost

SRC=..

INCLUDE= -I $(SRC) \
    -I $(COMMON) \
		-I $(UTILS) \
    -I $(BOOST)

LIBS=-L$(SRC) \
	 -L$(UTILS) \
	 -lquickfix \
	 -lboost_date_time \
   -lboost_system \
	 -lgtest 

CFLAGS=-Wall  -std=c++0x -ggdb -DDEBUG

all: liborder_book.a book_tests

liborder_book.a: 
	cd .. && make 

book_tests : liborder_book.a book_tests.o 
	$(CC) book_tests.o  $(LIBS) -lorder_book -o $@   

book_tests.o: book_tests.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) book_tests.cpp


clean :
	rm -f  *.o book_tests 

.PHONY: clean 


