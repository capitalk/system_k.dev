CC=g++

CAPK_ROOT=~
PROTOC=protoc

COMMON=$(CAPK_ROOT)/common
PROTO=$(COMMON)/proto/

UTILS=$(COMMON)/src/c++/
SRC=..


INCLUDE=-I$(SRC) \
		-I$(UTILS) \
		-I $(STLSOFT)/include 

LIBS= -lquickfix \
		-lboost_date_time \
		-lboost_program_options \
		-lboost_filesystem \
		-lzmq \
		-lprotobuf \
		-lrt \
		-lboost_thread \
		-lgtest 


CFLAGS=-Wall -std=c++0x -pthread -O3 -DLOG
LDFLAGS=-static

PROTOBUF_OBJS = new_order_single.pb.o \
				capk_globals.pb.o


ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g  -DLOG
endif

LDFLAGS=-static

all: test

time_utils.o: $(SRC)/time_utils.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

fix_convertors.o: $(SRC)/fix_convertors.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

# TESTS
test_time_compare.o: test_time_compare.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

test_time_compare: test_time_compare.o time_utils.o
	$(CC) $(CFLAGS) $?  $(LIBS) -o $@ 

test_uuidgen.o: test_uuidgen.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

test_uuidgen: test_uuidgen.o
	$(CC) $(CFLAGS) $< $(LIBS) -o $@ 

zmq_version.o: zmq_version.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

zmq_version: zmq_version.o 
	$(CC) $(CFLAGS) $< $(LIBS) -o $@ 

test_timespec2str.o: test_timespec2str.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

test_timespec2str: test_timespec2str.o time_utils.o fix_convertors.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ 

test: test_uuidgen zmq_version test_timespec2str test_time_compare 

clean :
	rm -f *.o test_uuidgen zmq_version test_timespec2str test_time_compare

