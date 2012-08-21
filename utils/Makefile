CC=g++

PROTOC=protoc
PROTO=../../collectors/proto

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
		-lboost_thread

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

all: utils

KTimeUtils.o: KTimeUtils.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<


# TESTS

uuidgen.o: uuidgen.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

uuidgen: uuidgen.o
	$(CC) $(CFLAGS) uuidgen.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

zmq_version.o: zmq_version.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

zmq_version: zmq_version.o 
	$(CC) $(CFLAGS) zmq_version.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

timespec2str_test.o: timespec2str_test.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

timespec2str_test: timespec2str_test.o KTimeUtils.o
	$(CC) $(CFLAGS) timespec2str_test.o KTimeUtils.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

utils: uuidgen zmq_version timespec2str_test

clean :
	rm -f *.o *.gch uuidgen zmq_version timespec2str_test

