CC=g++

CAPK_ROOT=~/capitalk
COMMON=$(CAPK_ROOT)/common
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

all: liborder_book2.a book_tests2

liborder_book2.a: 
	cd .. && make 

book_tests2 : liborder_book2.a book_tests2.o 
	$(CC) book_tests2.o  $(LIBS) -lorder_book2 -o $@   

book_tests2.o: book_tests2.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) book_tests2.cpp


clean :
	rm -f  *.o book_tests2 

.PHONY: clean 


