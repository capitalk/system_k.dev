CC=g++

PROTOC=protoc
PROTO=../../collectors/proto


UTILS=../../collectors/utils

INCLUDE=-I/usr/local/boost/ -I/usr/local/include/ \
        -I/usr/local/src/quickfix/include -I.. -I../../collectors/ \
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
				execution_report.pb.o
				

dbg=1

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g  -DLOG
endif

LDFLAGS=-static

all: order-fix

order-fix : order-fix.o Application.o logging.o JenkinsHash.o timing.o KMsgProcessor.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) order-fix.o Application.o logging.o JenkinsHash.o timing.o KMsgProcessor.o $(LIBS) $(PROTOBUF_OBJS) $(PANTHEIOS_LIBS) -o $@ 

logging.o: logging.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

timing.o: timing.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<
	
new_order_single.pb.o: $(PROTO)/new_order_single.pb.cc $(PROTO)/new_order_single.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/new_order_single.pb.cc

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

order-fix.o: order-fix.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

Application.o: Application.cpp
	#$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) Application.cpp  
	$(CC) -c $(INCLUDE) $(CFLAGS) $<  

KMsgProcessor.o: KMsgProcessor.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

KMsgRouter.o: KMsgRouter.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

KTimeUtils.o:  $(UTILS)/KTimeUtils.cpp
	$(CC) -c -I $(UTILS) $(INCLUDE) $(CFLAGS) $<

JenkinsHash.o: $(UTILS)/JenkinsHash.cpp
	$(CC) -c -I $(UTILS) $(INCLUDE) $(CFLAGS) $<

ClientOrderInterface.o: ClientOrderInterface.cpp
	$(CC) -c -I $(INCLUDE) $(CFLAGS) $<

OrderMux.o: OrderMux.cpp
	$(CC) -c -I $(INCLUDE) $(CFLAGS) $<


# TESTS

MsgProcessorTestSvr.o: MsgProcessorTestSvr.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<
	#$(CC) -c $(UTILS)/zhelpers.hpp $(INCLUDE) $(CFLAGS) $<

MsgProcessorTestCli.o: MsgProcessorTestCli.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

testsvr: MsgProcessorTestSvr.o JenkinsHash.o logging.o KMsgProcessor.o timing.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) KMsgProcessor.o JenkinsHash.o  MsgProcessorTestSvr.o logging.o timing.o $(PROTOBUF_OBJS) $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

testcli: MsgProcessorTestCli.o ClientOrderInterface.o OrderMux.o logging.o KTimeUtils.o timing.o $(PROTOBUF_OBJS) 
	$(CC) $(CFLAGS) MsgProcessorTestCli.o ClientOrderInterface.o OrderMux.o logging.o KTimeUtils.o timing.o $(PROTOBUF_OBJS) $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

req.o: req.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

rep.o: rep.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

req: req.o logging.o timing.o
	$(CC) $(CFLAGS) req.o logging.o timing.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

rep: rep.o logging.o timing.o
	$(CC) $(CFLAGS) rep.o logging.o timing.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

uuid_test.o: uuid_test.cpp KMsgCache.h
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

uuid_test: uuid_test.o JenkinsHash.o logging.o
	$(CC) $(CFLAGS) uuid_test.o logging.o JenkinsHash.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

cache_test.o: cache_test.cpp KMsgCache.h
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

cache_test: cache_test.o KMsgCache.h JenkinsHash.o logging.o
	$(CC) $(CFLAGS) logging.o  cache_test.o JenkinsHash.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

hash_test.o: hash_test.cpp KMsgCache.h
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

hash_test: hash_test.o KMsgCache.h JenkinsHash.o
	$(CC) $(CFLAGS) hash_test.o JenkinsHash.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 



# PROTOS
protos: 
	cd $(PROTO) && make

tests: protos testsvr testcli req rep uuid_test cache_test

clean :
	rm -f *.o order-fix req rep testsvr testcli uuid_test cache_test hash_test

install :
	echo "No install available yet"

.PHONY : order_engine 


