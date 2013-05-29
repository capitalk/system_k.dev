CC = g++

CAPK_ROOT = ~/capitalk
COMMON = ../../../common
UTILS = $(COMMON)/src/c++
UTILS_SRC = $(UTILS)/utils
PROTO = $(COMMON)/proto

ORDERBOOK_DIR = ../../order_book/order_book.v2
ORDERBOOK_INC = ../../order_book
ORDERBOOK_LIB = $(ORDERBOOK_DIR)/liborder_book2.a

PANTHEIOS_EXT = $(CAPK_ROOT)/pantheios_be_zmq

INCLUDE = -I $(PANTHEIOS_ROOT)/include \
		-I $(CAPK_ROOT) \
		-I $(STLSOFT)/include \
		-I $(ORDERBOOK_INC) \
		-I $(PANTHEIOS_EXT) \
		-I $(UTILS) \
		-I $(COMMON)

LIBS = -L$(ORDERBOOK_DIR)/ \
		-lquickfix \
		-lorder_book2 \
		-lrt \
		-lzmq \
		-lprotobuf \
		-lgtest \
		-lboost_date_time \
		-lboost_program_options \
		-lboost_system \
		-lboost_filesystem
	

# N.B. MAKE SURE YOU LINK WITH THE RIGHT LIBS (check the compiler gccXX)
PANTHEIOS_LIBS = -L $(PANTHEIOS_ROOT)/lib \
        -l pantheios.1.core.gcc44.file64bit.mt \
        -l pantheios.1.fe.N.gcc44.file64bit.mt \
        -l pantheios.1.util.gcc44.file64bit.mt \
        -l pantheios.1.be.N.gcc44.file64bit.mt \
        -l pantheios.1.bec.file.gcc44.file64bit.mt \
        -l pantheios.1.bec.fprintf.gcc44.file64bit.mt \
        -l pantheios.1.util.gcc44.file64bit.mt \
        -l pantheios.1.core.gcc44.file64bit.mt



ifeq ($(dbg),1)
CFLAGS = -Wall -std=c++0x -pthread -g3 -DLOG -DDEBUG -ggdb  # -DDEBUG_BOOK
LOG=1
else
CFLAGS = -Wall -std=c++0x -pthread -O3 -ULOG -DNDEBUG
endif

all: protos collect

OBJS = jenkins_hash.o \
      config_server.o \
      time_utils.o \
      collect.o \
      md_broadcast.o \
      md_parser_qf.o

LOG_OBJS = pantheios_be_zmq.o \
           logging.o

include $(PROTO)/Makefile.inc

ifdef LOG
collect : \
    $(LOG_OBJS) \
    $(OBJS) \
    $(ORDERBOOK_LIB) \
    $(PROTOBUF_OBJS)
	@echo Building with -DLOG \(logging on\)
	$(CC) $(CFLAGS) \
      $(LOG_OBJS) \
      $(OBJS) \
      $(PROTOBUF_OBJS) \
      $(ORDERBOOK_LIB) \
      $(LIBS) \
      $(PANTHEIOS_LIBS) -o $@ 
else
collect : \
    $(OBJS) \
    $(ORDERBOOK_LIB) \
    $(PROTOBUF_OBJS)
	@echo Building with -ULOG \(logging off\)
	$(CC) $(CFLAGS) \
      $(OBJS) \
      $(PROTOBUF_OBJS) \
      $(ORDERBOOK_LIB) \
      $(LIBS) -o $@ 
endif

pantheios_be_zmq.o: $(PANTHEIOS_EXT)/pantheios_be_zmq.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

logging.o: $(UTILS_SRC)/logging.cpp $(UTILS_SRC)/logging.h
	$(CC) -c $(INCLUDE) $(CFLAGS) -Dunix $<

jenkins_hash.o: $(UTILS_SRC)/jenkins_hash.cpp $(UTILS_SRC)/jenkins_hash.h
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

config_server.o: $(UTILS_SRC)/config_server.cpp $(UTILS_SRC)/config_server.h
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

time_utils.o: $(UTILS_SRC)/time_utils.cpp $(UTILS_SRC)/time_utils.h
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

collect.o: collect.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

md_broadcast.o: md_broadcast.cpp md_broadcast.h
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

md_parser_qf.o: md_parser_qf.cpp md_parser_qf.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $<  


$(ORDERBOOK_LIB):
	cd $(ORDERBOOK_DIR) && make 

protos: 
	cd $(PROTO) && make

clean :
	rm -f *.o collect 
	rm -f *.pid
	cd $(PROTO) && make clean
	cd $(ORDERBOOK_DIR) && make clean

install :
	cp -r ./cfg /usr/local/capk
	cp ./collect /usr/local/capk
	cp *.sh /usr/local/capk

.PHONY : collect 


