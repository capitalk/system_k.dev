CC=g++

PROTOC=protoc
PROTO=../../collectors/proto

BOOST=/usr/local/boost
COLLECTORS=../../collectors
UTILS=../../collectors/utils
TESTS=./tests

INCLUDE=-I $(BOOST) -I /usr/local/include/ \
        -I/usr/local/src/quickfix/include -I.. -I $(COLLECTORS) \
		-I $(PANTHEIOS_ROOT)/include -I $(STLSOFT)/include \
		-I $(UTILS)

LIBS= -L/usr/local/lib \
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

CFLAGS=-Wall -std=c++0x -pthread -O3 -ULOG
LDFLAGS=-static

PROTOBUF_OBJS = new_order_single.pb.o \
				order_cancel.pb.o \
				order_cancel_reject.pb.o \
				order_cancel_replace.pb.o \
				capk_globals.pb.o \
				execution_report.pb.o \
				order_status.pb.o
				

dbg=1

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g  -DLOG
endif

LDFLAGS=-static

all: protos order-fix

order-fix : order-fix.o application.o logging.o jenkins_hash.o timing.o  msg_processor.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) order-fix.o application.o logging.o jenkins_hash.o timing.o  msg_processor.o $(LIBS) $(PROTOBUF_OBJS) $(PANTHEIOS_LIBS) -o $@ 

logging.o: logging.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

timing.o: timing.cpp 
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

order-fix.o: order-fix.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

application.o: application.cpp
	#$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) application.cpp  
	$(CC) -c $(INCLUDE) $(CFLAGS) $<  

msg_processor.o: msg_processor.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

msg_router.o: msg_router.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

KTimeUtils.o:  $(UTILS)/KTimeUtils.cpp
	$(CC) -c -I $(UTILS) $(INCLUDE) $(CFLAGS) $<

jenkins_hash.o: $(UTILS)/jenkins_hash.cpp
	$(CC) -c -I $(UTILS) $(INCLUDE) $(CFLAGS) $<

client_order_interface.o: client_order_interface.cpp
	$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

client_market_data_interface.o: client_market_data_interface.cpp
	$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

market_data_mux.o: market_data_mux.cpp
	$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

order_mux.o: order_mux.cpp
	$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

specializations.o: specializations.cpp
	$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

order.o: order.cpp
	$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

# TESTS

#MsgProcessorTestSvr.o: MsgProcessorTestSvr.cpp 
	#$(CC) -c $(INCLUDE) $(CFLAGS) $<
	#$(CC) -c $(UTILS)/zhelpers.hpp $(INCLUDE) $(CFLAGS) $<

#MsgProcessorTestCli.o: strategy_test.cpp
	#$(CC) -c $(INCLUDE) $(CFLAGS) $<


# PROTOS
protos: 
	cd $(PROTO) && make

test: 
	cd $(TESTS) && make 

#protos testsvr testcli req rep uuid_test cache_test serialize_cache_test

clean :
	rm -f *.o order-fix req rep testsvr testcli uuid_test cache_test hash_test serialize_cache_test

install :
	echo "No install available yet"

.PHONY : order_engine 


