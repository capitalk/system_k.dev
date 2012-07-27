CC=g++

PROTOC=protoc
PROTO=../../../collectors/proto

BOOST=/usr/local/boost
COLLECTORS=../../../collectors
UTILS=../../../collectors/utils
TESTS=./tests
MYSQL=/usr/local/include/mysql
STRATEGY_BASE=../../../strategy/strategy_base

ORDER_FIX=../../order-fix

INCLUDE=-I $(BOOST) \
		-I /usr/local/include/ \
		-I $(MYSQL) \
        -I/usr/local/src/quickfix/include \
		-I.. \
		-I $(COLLECTORS) \
		-I $(PANTHEIOS_ROOT)/include \
		-I $(STLSOFT)/include \
		-I $(ORDER_FIX) \
		-I $(UTILS) \
		-I $(STRATEGY_BASE)

LIBS= -L/usr/local/lib \
	  -L/usr/local/lib64/mysql \
      	-lquickfix -lboost_date_time \
		-lboost_program_options \
		-lboost_filesystem \
		-lzmq \
		-lprotobuf \
		-lrt \
		-lboost_thread \
		-lgtest \
		-lmysqlcppconn


PANTHEIOS_LIBS= -L $(PANTHEIOS_ROOT)/lib \
        -l pantheios.1.core.gcc44.file64bit \
        -l pantheios.1.fe.simple.gcc44.file64bit.debug \
        -l pantheios.1.util.gcc44.file64bit \
		-l pantheios.1.be.N.gcc44.file64bit \
        -l pantheios.1.bec.file.gcc44.file64bit \
        -l pantheios.1.bec.fprintf.gcc44.file64bit \
        -l pantheios.1.util.gcc44.file64bit \
        -l pantheios.1.core.gcc44.file64bit 

CFLAGS=-Wall -std=c++0x -pthread -O2 -ULOG
LDFLAGS=-static


ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g  -DLOG
endif

#LDFLAGS=-static

all: test_trade_serializer test_recovery

include $(PROTO)/Makefile.inc

test_recovery : test_recovery.o logging.o order.o jenkins_hash.o timing.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) test_recovery.o logging.o order.o jenkins_hash.o timing.o  $(LIBS) $(PROTOBUF_OBJS) $(PANTHEIOS_LIBS) -o $@ 

test_trade_serializer : test_trade_serializer.o logging.o order.o jenkins_hash.o timing.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) test_trade_serializer.o logging.o order.o jenkins_hash.o timing.o  $(LIBS) $(PROTOBUF_OBJS) $(PANTHEIOS_LIBS) -o $@ 

test_trade_serializer.o: test_trade_serializer.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

test_recovery.o: test_recovery.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

order.o: $(STRATEGY_BASE)/order.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

jenkins_hash.o: $(UTILS)/jenkins_hash.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

logging.o: $(UTILS)/logging.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

timing.o: $(UTILS)/timing.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<
	
# PROTOS
protos: 
	cd $(PROTO) && make

test: 
	cd $(TESTS) && make 

clean :
	rm -f *.o test_trade_serializer

install :
	echo "No install available yet"



