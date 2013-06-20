CC=g++


COMMON=../../common

UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)/utils
PROTO=$(COMMON)/proto

INCLUDE= -I$(UTILS) \
		-I$(COMMON) 

LIBS= -lquickfix \
		-lzmq \
		-lrt \
		-lprotobuf \
    -lgtest \
		-lboost_date_time \
		-lboost_thread \
		-lboost_program_options \
		-lboost_system \
		-lboost_filesystem 

CFLAGS=-Wall -std=c++0x -pthread -O3 -ULOG -DNDEBUG

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb 
endif

all: test_unrolls 

test_unrolls: test_unrolls.o
	$(CC) $< $(LIBS) -o $@

test_unrolls.o: test_unrolls.cpp
	$(CC) -o $@ -c $< $(INCLUDE) $(CFLAGS)

clean :
	rm -f *.o test_unrolls


