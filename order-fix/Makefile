CC=g++

PROTOC=protoc
PROTO=../../collectors/proto

INCLUDE=-I/usr/local/boost/ -I/usr/local/include/ \
        -I/usr/local/src/quickfix/include -I.. -I../../collectors/ \
		-I $(PANTHEIOS_ROOT)/include -I $(STLSOFT)/include 

LIBS= -L/usr/local/src/quickfix/lib/ -L/usr/local/lib \
      -lquickfix -lboost_date_time -lboost_program_options -lboost_filesystem  -lzmq -lprotobuf -lrt -lboost_thread

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

all: order-fix

order-fix : order-fix.o Application.o logging.o timing.o
	$(CC) $(CFLAGS) order-fix.o Application.o logging.o timing.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

logging.o: logging.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

timing.o: timing.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<
	
new_order_single.pb.o: $(PROTO)/new_order_single.pb.cc $(PROTO)/new_order_single.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/new_order_single.pb.cc

capk_globals.pb.o: $(PROTO)/capk_globals.pb.cc $(PROTO)/capk_globals.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/capk_globals.pb.cc

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


# TESTS

MsgProcessorTestSvr.o: MsgProcessorTestSvr.cpp 
	$(CC) -c $(UTILS)/zhelpers.hpp $(INCLUDE) $(CFLAGS) $<

MsgProcessorTestCli.o: MsgProcessorTestCli.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

testsvr: MsgProcessorTestSvr.o logging.o KMsgProcessor.o timing.o KMsgRouter.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) KMsgProcessor.o KMsgRouter.o  MsgProcessorTestSvr.o logging.o timing.o $(PROTOBUF_OBJS) $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

testcli: MsgProcessorTestCli.o logging.o KTimeUtils.o timing.o $(PROTOBUF_OBJS) 
	$(CC) $(CFLAGS) MsgProcessorTestCli.o logging.o KTimeUtils.o timing.o $(PROTOBUF_OBJS) $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

req.o: req.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

rep.o: rep.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

req: req.o logging.o timing.o
	$(CC) $(CFLAGS) req.o logging.o timing.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

rep: rep.o logging.o timing.o
	$(CC) $(CFLAGS) rep.o logging.o timing.o $(LIBS) $(PANTHEIOS_LIBS) -o $@ 


# PROTOS
protos: $(PROTO)/new_order_single.proto
	protoc -I $(PROTO) --cpp_out=$(PROTO) $(PROTO)/*.proto

tests: testsvr testcli req rep

clean :
	rm -f *.o order-fix req rep testsvr testcli #$(PROTO)/*.cc $(PROTO)/*.h

install :
	echo "No install available yet"

.PHONY : order_engine 


