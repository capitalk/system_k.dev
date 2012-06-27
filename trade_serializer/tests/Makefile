CC=g++

PROTOC=protoc
PROTO=../../../collectors/proto

BOOST=/usr/local/boost
COLLECTORS=../../../collectors
UTILS=../../../collectors/utils
TESTS=./tests
MYSQL=/usr/local/include/mysql

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
		-I $(UTILS)

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

#LDFLAGS=-static

all: test_trade_serializer 

test_trade_serializer : test_trade_serializer.o logging.o jenkins_hash.o timing.o $(PROTOBUF_OBJS)
	$(CC) $(CFLAGS) test_trade_serializer.o logging.o jenkins_hash.o timing.o  $(LIBS) $(PROTOBUF_OBJS) $(PANTHEIOS_LIBS) -o $@ 

test_trade_serializer.o: test_trade_serializer.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

jenkins_hash.o: $(UTILS)/jenkins_hash.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

logging.o: $(ORDER_FIX)/logging.cpp 
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

timing.o: $(ORDER_FIX)/timing.cpp 
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

# PROTOS
protos: 
	cd $(PROTO) && make

test: 
	cd $(TESTS) && make 

clean :
	rm -f *.o test_trade_serializer

install :
	echo "No install available yet"



