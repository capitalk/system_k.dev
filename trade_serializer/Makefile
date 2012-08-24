CC=g++

PROTOC=protoc

COMMON=../../common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)/utils
PROTO=$(COMMON)/proto

TESTS=./tests
MYSQL=/usr/local/include/mysql
STRATEGY_BASE=../../strategy
ORDER_ENGINE=../order_entry

INCLUDE= -I $(MYSQL) \
		-I $(PANTHEIOS_ROOT)/include \
		-I $(STLSOFT)/include \
		-I $(COMMON) \
		-I $(PROTO) \
		-I $(UTILS) \
		-I $(STRATEGY_BASE) \
		-I $(ORDER_ENGINE) 

LIBS= -L/usr/local/lib64/mysql \
      	-lquickfix \
		-lboost_date_time \
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

jenkins_hash.o: $(UTILS_SRC)/jenkins_hash.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

time_utils.o: $(UTILS_SRC)/time_utils.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

logging.o: $(UTILS_SRC)/logging.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

timing.o: $(UTILS_SRC)/timing.cpp 
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



