CC = g++

CAPK_ROOT = ~/capitalk
COMMON = $(CAPK_ROOT)/common
UTILS = $(COMMON)/src/c++
UTILS_SRC = $(UTILS)/utils
PROTO = $(COMMON)/proto


INCLUDE = -I..\
          -I. \
          -I$(COMMON)\
          -I$(UTILS)

LIBS = -lgtest 

ifeq ($(dbg), 1)
CFLAGS = -Wall  -std=c++0x -g3 -DDEBUG -ggdb
else
CFLAGS = -Wall -std=c++0x -O3 -DNDEBUG
endif

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $(input) -o $(output)


all: liborder_book2.a

liborder_book2.a: order_book.o 
	ar rcs liborder_book3.a $+

order_book.o: order_book.cpp order_book.hxx
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

#order_book.o: order_book.cpp 
	#$(CC) -c $(INCLUDE) $(CFLAGS) $<

#limit.o: limit.cpp
	#$(CC) -c $(INCLUDE) $(CFLAGS) $<

#time_utils.o: $(UTILS_SRC)/time_utils.cpp
	#$(CC) -c $(INCLUDE) $(CFLAGS) $(UTILS_SRC)/time_utils.cpp


clean :
	rm -f  *.o liborder_book3.a 




