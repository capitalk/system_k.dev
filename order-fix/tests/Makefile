CC=g++

PROTOC=protoc
PROTO=../../../collectors/proto


SRC=..

BOOST=/usr/local/boost
COLLECTORS=../../../collectors
UTILS=../../../collectors/utils

INCLUDE=-I$(BOOST) -I/usr/local/include/ \
        -I/usr/local/src/quickfix/include -I.. -I$(COLLECTORS) \
		-I $(PANTHEIOS_ROOT)/include -I $(STLSOFT)/include \
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
        -l pantheios.1.fe.simple.gcc44.file64bit.debug \
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

order-fix : order-fix.o Application.o logging.o jenkins_hash.o timing.o  msg_processor.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) order-fix.o Application.o logging.o jenkins_hash.o timing.o  msg_processor.o $(LIBS) $(PROTOBUF_OBJS) $(PANTHEIOS_LIBS) -o $@ 

logging.o: $(SRC)/logging.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

timing.o: $(SRC)/timing.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<
	
new_order_single.pb.o: $(PROTO)/new_order_single.pb.cc $(PROTO)/new_order_single.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/new_order_single.pb.cc

order_status.pb.o: $(PROTO)/order_status.pb.cc $(PROTO)/order_status.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/order_status.pb.cc

capk_globals.pb.o: $(PROTO)/capk_globals.pb.cc $(PROTO)/capk_globals.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/capk_globals.pb.cc

order_cancel.pb.o: $(PROTO)/order_cancel.pb.cc $(PROTO)/order_cancel.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/order_cancel.pb.cc

order_cancel_reject.pb.o: $(PROTO)/order_cancel_reject.pb.cc $(PROTO)/order_cancel_reject.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/order_cancel_reject.pb.cc

order_cancel_replace.pb.o: $(PROTO)/order_cancel_replace.pb.cc $(PROTO)/order_cancel_replace.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/order_cancel_replace.pb.cc

execution_report.pb.o: $(PROTO)/execution_report.pb.cc $(PROTO)/execution_report.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/execution_report.pb.cc

spot_fx_md_1.pb.o: $(PROTO)/spot_fx_md_1.pb.cc $(PROTO)/spot_fx_md_1.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/spot_fx_md_1.pb.cc

order-fix.o: $(SRC)/order-fix.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

order.o: $(SRC)/order.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

Application.o: $(SRC)/Application.cpp
	#$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) Application.cpp  
	$(CC) -c $(INCLUDE) $(CFLAGS) $<  

msg_processor.o: $(SRC)/msg_processor.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

msg_router.o: $(SRC)/msg_router.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

time_utils.o:  $(UTILS)/time_utils.cpp
	$(CC) -c -I $(UTILS) $(INCLUDE) $(CFLAGS) $<

jenkins_hash.o: $(UTILS)/jenkins_hash.cpp
	$(CC) -c -I $(UTILS) $(INCLUDE) $(CFLAGS) $<

client_order_interface.o: $(SRC)/client_order_interface.cpp
	$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

order_mux.o: $(SRC)/order_mux.cpp
	$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

client_market_data_interface.o: $(SRC)/client_market_data_interface.cpp
	$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

market_data_mux.o: $(SRC)/market_data_mux.cpp
	$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

# TESTS

test_server.o: $(SRC)/test_server.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<
	#$(CC) -c $(UTILS)/zhelpers.hpp $(INCLUDE) $(CFLAGS) $<

strategy_test.o: $(SRC)/strategy_test.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

testsvr: test_server.o jenkins_hash.o logging.o msg_processor.o timing.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) msg_processor.o jenkins_hash.o  test_server.o logging.o timing.o $(PROTOBUF_OBJS) $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

test_strategy: strategy_test.o client_order_interface.o order_mux.o logging.o jenkins_hash.o time_utils.o timing.o market_data_mux.o client_market_data_interface.o order.o $(PROTOBUF_OBJS) 
	$(CC) $(CFLAGS) strategy_test.o jenkins_hash.o client_order_interface.o order_mux.o logging.o time_utils.o timing.o market_data_mux.o client_market_data_interface.o order.o $(PROTOBUF_OBJS) $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

uuid_test.o: uuid_test.cpp $(SRC)/msg_cache.h
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

uuid_test: uuid_test.o jenkins_hash.o logging.o
	$(CC) $(CFLAGS) uuid_test.o logging.o jenkins_hash.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

cache_test.o: cache_test.cpp $(SRC)/msg_cache.h
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

cache_test: cache_test.o $(SRC)/msg_cache.h jenkins_hash.o logging.o
	$(CC) $(CFLAGS) logging.o  cache_test.o jenkins_hash.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

hash_test.o: hash_test.cpp $(SRC)/msg_cache.h
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

hash_test: hash_test.o $(SRC)/msg_cache.h jenkins_hash.o
	$(CC) $(CFLAGS) hash_test.o jenkins_hash.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

serialize_cache_test: serialize_cache_test.o logging.o jenkins_hash.o
	$(CC) $(CFLAGS) serialize_cache_test.o logging.o jenkins_hash.o $(LIBS) $(PANTHEIOS_LIBS) -o $@

serialize_cache_test.o: serialize_cache_test.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<



# PROTOS
protos: 
	cd $(PROTO) && make

tests: protos testsvr test_strategy uuid_test cache_test serialize_cache_test

clean :
	rm -f *.o order-fix req rep testsvr test_strategy uuid_test cache_test hash_test serialize_cache_test ../*.o

install :
	echo "No install available yet"

.PHONY : order_engine 


