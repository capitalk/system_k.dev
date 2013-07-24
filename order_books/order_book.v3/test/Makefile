CC=g++

CAPK_ROOT = ~/capitalk
COMMON=$(CAPK_ROOT)/common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)/utils

SRC=..

INCLUDE= -I $(SRC) \
         -I $(COMMON) \
         -I $(UTILS)

LIBS= -L$(UTILS) \
      -L$(SRC) \
      -lgtest \
      -lorder_book3 \
      -lpthread 

CFLAGS=-Wall  -std=c++0x -O3 # -g -D DEBUG

#%.o: %.cpp
	#$(CC) $(CFLAGS) $(INCLUDE) -c $(input) -o $(output)


all: gtest_book # gtest_del gtest_mod gtest_oob

liborder_book3.a: 
	cd .. && make 

ifeq ($(dbg), 1)
  CFLAGS = -Wall  -std=c++0x  -DDEBUG -ggdb
else
  CFLAGS = -Wall -std=c++0x -O3 -DNDEBUG
endif


# Other tests
gtest_book: gtest_book.cpp $(SRC)/order_book.cpp
	$(CC) $(CFLAGS) gtest_book.cpp  $(SRC)/order_book.cpp $(INCLUDE) -L $(LIBS) -o $@



clean :
	rm -f  *.o gtest_book

.PHONY: clean 


