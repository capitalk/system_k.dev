CC = g++

SYSK_ROOT = ../../ 
COMMON = ../../common
UTILS = $(COMMON)/src/c++
UTILS_SRC = $(UTILS)/utils
PROTO = $(COMMON)/proto

ORDERBOOK_DIR = ../../order_book/order_book.v2
ORDERBOOK_INC = ../../order_book
ORDERBOOK_LIB = $(ORDERBOOK_DIR)/liborder_book2.a

PANTHEIOS_EXT = $(COMMON)/pantheios_be_zmq

INCLUDE = -I $(PANTHEIOS_ROOT)/include \
		-I $(SYSK_ROOT) \
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

all: protos md_fix

OBJS = jenkins_hash.o \
      config_server.o \
      time_utils.o \
      md_fix.o \
      md_broadcast.o \
      md_parser_qf.o

LOG_OBJS = pantheios_be_zmq.o \
           logging.o

LOCAL_PROTOS=spot_fx_md_1.pb.o \
             venue_configuration.pb.o \
             capk_globals.pb.o

include $(PROTO)/Makefile.inc

ifdef LOG
md_fix : \
    $(LOG_OBJS) \
    $(OBJS) \
    $(ORDERBOOK_LIB) \
    $(LOCAL_PROTOS)
	@echo Building with -DLOG \(logging on\)
	$(CC) $(CFLAGS) \
      $(LOG_OBJS) \
      $(OBJS) \
      $(LOCAL_PROTOS) \
      $(ORDERBOOK_LIB) \
      $(LIBS) \
      $(PANTHEIOS_LIBS) -o $@ 
else
md_fix : \
    $(OBJS) \
    $(ORDERBOOK_LIB) \
    $(LOCAL_PROTOS)
	@echo Building with -ULOG \(logging off\)
	$(CC) $(CFLAGS) \
      $(OBJS) \
      $(LOCAL_PROTOS) \
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

md_fix.o: md_fix.cpp
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
	rm -f *.o md_fix 
	rm -f *.pid
	cd $(PROTO) && make clean
	cd $(ORDERBOOK_DIR) && make clean

install :
	cp -r ./cfg /usr/local/capk
	cp ./md_fix /usr/local/capk
	cp *.sh /usr/local/capk

.PHONY : md_fix 


