CC=g++

ROOT=../../../..
SYS_ROOT=$(ROOT)/capitalk


COMMON=$(SYS_ROOT)/common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)/utils
PROTO=$(COMMON)/proto
PANTHEIOS_EXT=/home/timir/pantheios_be_zmq
TESTS=./tests

INCLUDE= -I $(PANTHEIOS_ROOT)/include \
		-I $(STLSOFT)/include \
		-I $(UTILS)\
		-I $(COMMON)

LIBS= -L/usr/local/lib \
      	-lquickfix \
		-lboost_date_time \
		-lboost_program_options \
		-lboost_filesystem \
		-lzmq \
		-lprotobuf \
		-lrt \
		-lboost_thread \
		-lgtest

# N.B. MAKE SURE YOU LINK WITH THE RIGHT LIBS (check the compiler gccXX)
PANTHEIOS_LIBS= -L $(PANTHEIOS_ROOT)/lib \
        -l pantheios.1.core.gcc44.file64bit \
        -l pantheios.1.fe.N.gcc44.file64bit \
        -l pantheios.1.util.gcc44.file64bit \
		-l pantheios.1.be.N.gcc44.file64bit \
        -l pantheios.1.bec.file.gcc44.file64bit \
        -l pantheios.1.bec.fprintf.gcc44.file64bit \
        -l pantheios.1.util.gcc44.file64bit \
        -l pantheios.1.core.gcc44.file64bit 

CFLAGS=-Wall -std=c++0x -pthread -O3 -ULOG -DNDEBUG
#LDFLAGS=-static

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb  -DLOG -pg -g
		LDFLAGS =  -pg
endif

all: protos order-fix

include $(PROTO)/Makefile.inc

order-fix : order-fix.o application.o logging.o jenkins_hash.o timing.o  msg_processor.o config_server.o pantheios_be_zmq.o $(PROTOBUF_OBJS)
	$(CC) $(LDFLAGS) order-fix.o application.o logging.o jenkins_hash.o timing.o  msg_processor.o config_server.o pantheios_be_zmq.o $(LIBS) $(PROTOBUF_OBJS) $(PANTHEIOS_LIBS) -o $@ 

order-fix.o: order-fix.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

application.o: application.cpp
	#$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) application.cpp  
	$(CC) -c $(INCLUDE) $(CFLAGS) $<  

pantheios_be_zmq.o: $(PANTHEIOS_EXT)/pantheios_be_zmq.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

logging.o: $(UTILS_SRC)/logging.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

jenkins_hash.o: $(UTILS_SRC)/jenkins_hash.cpp
	$(CC) -c -I $(UTILS) $(INCLUDE) $(CFLAGS) $<

timing.o: $(UTILS_SRC)/timing.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

msg_processor.o: msg_processor.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

msg_router.o: msg_router.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

config_server.o: $(UTILS_SRC)/config_server.cpp
	$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

#client_order_interface.o: client_order_interface.cpp
	#$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

#client_market_data_interface.o: client_market_data_interface.cpp
	#$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

#market_data_mux.o: market_data_mux.cpp
	#$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

#order_mux.o: order_mux.cpp
	#$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

#order.o: order.cpp
	#$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

# PROTOS
protos: 
	cd $(PROTO) && make

test: 
	cd $(TESTS) && make 

#protos testsvr testcli req rep uuid_test cache_test serialize_cache_test

clean :
	rm -f *.o order-fix req rep testsvr testcli uuid_test cache_test hash_test serialize_cache_test *.cache

install :
	echo "No install available yet"

.PHONY : order_engine 


