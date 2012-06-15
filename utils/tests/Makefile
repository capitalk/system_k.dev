CC=g++

PROTOC=protoc
PROTO=../../collectors/proto

SRC=..

INCLUDE=-I/usr/local/boost/ \
		-I/usr/local/include/ -I. \
        -I/usr/local/src/quickfix/include \
		-I.. \
		-I../../collectors/ \
		-I $(PANTHEIOS_ROOT)/include \
		-I $(STLSOFT)/include 

LIBS= -L/usr/local/src/quickfix/lib/ \
		-L/usr/local/lib \
      	-lquickfix \
		-lboost_date_time \
		-lboost_program_options \
		-lboost_filesystem \
		-lzmq \
		-lprotobuf \
		-lrt \
		-lboost_thread \
		-lgtest 

UTILS=../../collectors/utils

PANTHEIOS_LIBS= -L $(PANTHEIOS_ROOT)/lib \
        -l pantheios.1.core.gcc44.file64bit \
        -l pantheios.1.fe.simple.gcc44.file64bit.debug \
        -l pantheios.1.util.gcc44.file64bit \
		-l pantheios.1.be.N.gcc44.file64bit \
        -l pantheios.1.bec.file.gcc44.file64bit \
        -l pantheios.1.bec.fprintf.gcc44.file64bit \
        -l pantheios.1.util.gcc44.file64bit \
        -l pantheios.1.core.gcc44.file64bit 

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
	$(CC) $(CFLAGS) test_time_compare.o time_utils.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

uuidgen_test.o: uuidgen_test.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

uuidgen_test: uuidgen_test.o
	$(CC) $(CFLAGS) uuidgen_test.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

zmq_version.o: zmq_version.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

zmq_version: zmq_version.o 
	$(CC) $(CFLAGS) zmq_version.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

timespec2str_test.o: timespec2str_test.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

timespec2str_test: timespec2str_test.o time_utils.o fix_convertors.o
	$(CC) $(CFLAGS) timespec2str_test.o time_utils.o fix_convertors.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

test: uuidgen_test zmq_version timespec2str_test test_time_compare 

clean :
	rm -f *.o uuidgen_test zmq_version timespec2str_test test_time_compare

