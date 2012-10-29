CC=g++

PROTOC=protoc
COMMON=../../../../common
PROTO=$(COMMON)/proto


SRC=..

BOOST=/usr/local/boost
COLLECTORS=../../../collectors
UTILS=../../../../common/src/c++

INCLUDE=-I$(BOOST) -I/usr/local/include/ \
        -I/usr/local/src/quickfix/include \
		-I.. \
		-I$(COLLECTORS) \
		-I $(PANTHEIOS_ROOT)/include \
		-I $(STLSOFT)/include \
		-I $(PROTO) \
		-I $(COMMON) \
		-I $(UTILS)

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

PROTOBUF_OBJS = new_order_single.pb.o \
				order_cancel.pb.o \
				order_cancel_reject.pb.o \
				order_cancel_replace.pb.o \
				capk_globals.pb.o \
				execution_report.pb.o \
				spot_fx_md_1.pb.o \
				order_status.pb.o
				

dbg=1

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g  -DLOG
endif

LDFLAGS=-static

all: tests 

include $(PROTO)/Makefile.inc

# TESTS

test_server.o: $(SRC)/test_server.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<
	#$(CC) -c $(UTILS)/zhelpers.hpp $(INCLUDE) $(CFLAGS) $<

strategy_test.o: $(SRC)/strategy_test.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

testsvr: test_server.o jenkins_hash.o logging.o msg_processor.o timing.o pantheios_be_zmq.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) msg_processor.o jenkins_hash.o  test_server.o logging.o timing.o pantheios_be_zmq.o $(PROTOBUF_OBJS) $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

jenkins_hash.o: $(UTILS)/utils/jenkins_hash.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

logging.o: $(UTILS)/utils/logging.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

timing.o: $(UTILS)/utils/timing.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

uuid_test.o: uuid_test.cpp $(SRC)/msg_cache.h
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

uuid_test: uuid_test.o jenkins_hash.o logging.o pantheios_be_zmq.o
	$(CC) $(CFLAGS) uuid_test.o logging.o jenkins_hash.o pantheios_be_zmq.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

msg_processor.o: $(SRC)/msg_processor.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

cache_test.o: cache_test.cpp $(SRC)/msg_cache.h
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

cache_test: cache_test.o $(SRC)/msg_cache.h jenkins_hash.o logging.o pantheios_be_zmq.o 
	$(CC) $(CFLAGS) logging.o  cache_test.o jenkins_hash.o pantheios_be_zmq.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

hash_test.o: hash_test.cpp $(SRC)/msg_cache.h
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

hash_test: hash_test.o $(SRC)/msg_cache.h jenkins_hash.o
	$(CC) $(CFLAGS) hash_test.o jenkins_hash.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

serialize_cache_test: serialize_cache_test.o logging.o jenkins_hash.o pantheios_be_zmq.o
	$(CC) $(CFLAGS) serialize_cache_test.o logging.o jenkins_hash.o pantheios_be_zmq.o $(LIBS) $(PANTHEIOS_LIBS) -o $@

serialize_cache_test.o: serialize_cache_test.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

pantheios_be_zmq.o: /home/timir/pantheios_be_zmq/pantheios_be_zmq.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

read_cache_file.o: read_cache_file.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $< 

read_cache_file: read_cache_file.o $(SRC)/msg_cache.h jenkins_hash.o logging.o pantheios_be_zmq.o 
	$(CC) $(CFLAGS) logging.o  read_cache_file.o jenkins_hash.o pantheios_be_zmq.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

# PROTOS
protos: 
	cd $(PROTO) && make

tests: protos testsvr uuid_test cache_test serialize_cache_test read_cache_file

clean :
	rm -f *.o order-fix req rep testsvr test_strategy uuid_test cache_test hash_test serialize_cache_test ../*.o

install :
	echo "No install available yet"

.PHONY : order_engine 


