CC=g++

ROOT=../../..
PROTOC=protoc
COMMON=$(ROOT)/common
PROTO=$(COMMON)/proto

SRC=.

BOOST=/usr/local/boost
COLLECTORS=$(ROOT)/collectors
UTILS=$(ROOT)/common/src/c++/utils

PANTHEIOS_EXT=/home/timir/pantheios_be_zmq


INCLUDE=-I$(BOOST) \
        -I/usr/local/src/quickfix/include \
		-I.. \
		-I. \
		-I$(COLLECTORS) \
		-I $(PANTHEIOS_ROOT)/include \
		-I $(STLSOFT)/include \
		-I $(UTILS)/ \
		-I $(ROOT)/common/src/c++ \
		-I $(PROTO)\
		-I $(PROTO)/proto 

LIBS= -L/usr/local/src/quickfix/lib/ \
		-L/usr/local/lib \
      	-lquickfix -lboost_date_time \
		-lboost_program_options \
		-lboost_filesystem \
		-lzmq \
		-lprotobuf \
		-lrt \
		-lboost_thread \
		-lgtest


PANTHEIOS_LIBS= -L $(PANTHEIOS_ROOT)/lib \
        -l pantheios.1.core.gcc44.file64bit \
        -l pantheios.1.fe.N.gcc44.file64bit.debug \
        -l pantheios.1.util.gcc44.file64bit \
		-l pantheios.1.be.N.gcc44.file64bit \
        -l pantheios.1.bec.file.gcc44.file64bit \
        -l pantheios.1.bec.fprintf.gcc44.file64bit \
        -l pantheios.1.util.gcc44.file64bit \
        -l pantheios.1.core.gcc44.file64bit 

CFLAGS=-Wall -std=c++0x -pthread -O3 -DLOG #-ULOG
LDFLAGS=-static

dbg=1

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g  -DLOG
endif

LDFLAGS=-static

all: tests 

include $(PROTO)/Makefile.inc

# TESTS

test_configuration_server.o: $(SRC)/test_configuration_server.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

strategy_test.o: $(SRC)/strategy_test.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

test_configuration_server: test_configuration_server.o logging.o timing.o config_server.o pantheios_be_zmq.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) test_configuration_server.o logging.o timing.o config_server.o pantheios_be_zmq.o $(PROTOBUF_OBJS) $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

pantheios_be_zmq.o: $(PANTHEIOS_EXT)/pantheios_be_zmq.cpp
	    $(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

logging.o: $(UTILS)/logging.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

timing.o: $(UTILS)/timing.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

config_server.o: $(UTILS)/config_server.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

# PROTOS
protos: 
	cd $(PROTO) && make

tests: test_configuration_server 

clean :
	rm -f *.o test_configuration_server 

install :
	echo "No install available yet"

.PHONY : test_configuration_server 


