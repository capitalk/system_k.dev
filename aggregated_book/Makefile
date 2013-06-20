CC=g++

COMMON=../../common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)/utils
PROTO=$(COMMON)/proto

INCLUDE= -I$(UTILS) \
		-I$(COMMON) \

LIBS= -lquickfix \
		-lzmq \
		-lrt \
		-lprotobuf \
		-lboost_date_time \
		-lboost_thread \
		-lboost_program_options \
		-lboost_system \
		-lboost_filesystem 

CFLAGS=-Wall -std=c++0x -pthread -O3 -ULOG -DNDEBUG

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g # -DDEBUG_BOOK
endif

all: protos aggregated_book

include $(PROTO)/Makefile.inc

LOCAL_PROTOS=spot_fx_md_1.pb.o \
             venue_configuration.pb.o \
             capk_globals.pb.o

aggregated_book.o: aggregated_book.cpp
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $< 

time_utils.o: $(UTILS_SRC)/time_utils.cpp $(UTILS_SRC)/time_utils.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $< 

config_server.o: $(UTILS_SRC)/config_server.cpp $(UTILS_SRC)/config_server.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $< 

aggregated_book : aggregated_book.o time_utils.o config_server.o $(LOCAL_PROTOS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ 

protos : 
	cd $(PROTO) && make 

clean :
	rm -f *.o aggregated_book

.PHONY: all protos clean install

