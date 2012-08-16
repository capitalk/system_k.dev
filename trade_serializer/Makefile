CC=g++

PROTOC=protoc
PROTO=../../collectors/proto

BOOST=/usr/local/boost
COLLECTORS=../../collectors
UTILS=../../collectors/utils
TESTS=./tests
MYSQL=/usr/local/include/mysql
STRATEGY_BASE=../../strategy
ORDER_ENGINE=../../order_engines/order-fix

INCLUDE=-I $(BOOST) \
		-I /usr/local/include/ \
		-I $(MYSQL) \
        -I/usr/local/src/quickfix/include \
		-I.. \
	   	-I $(COLLECTORS) \
		-I $(PANTHEIOS_ROOT)/include \
		-I $(STLSOFT)/include \
		-I $(UTILS) \
		-I $(STRATEGY_BASE) \
		-I $(ORDER_ENGINE) 

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

CFLAGS=-Wall -std=c++0x -pthread -O3 -ULOG

LDFLAGS=-static

ifeq ($(dbg),1)
        CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -g  -DLOG
endif

#LDFLAGS=-static

all: protos trade_serializer 

include $(PROTO)/Makefile.inc

trade_serializer : trade_serializer.o logging.o jenkins_hash.o timing.o time_utils.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) trade_serializer.o logging.o jenkins_hash.o timing.o time_utils.o $(LIBS) $(PROTOBUF_OBJS) $(PANTHEIOS_LIBS) -o $@ 

jenkins_hash.o: $(UTILS)/jenkins_hash.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

time_utils.o: $(UTILS)/time_utils.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

logging.o: $(UTILS)/logging.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

timing.o: $(UTILS)/timing.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) $<
	
trade_serializer.o: trade_serializer.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

# PROTOS
protos: 
	cd $(PROTO) && make

test: 
	cd $(TESTS) && make 

clean :
	rm -f *.o trade_serializer

install :
	echo "No install available yet"



