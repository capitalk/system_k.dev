PROJ_ROOT_DIR =	$(abs_top_srcdir)/../../..

COMMON =	$(PROJ_ROOT_DIR)/common
PROTO =		$(COMMON)/proto
UTILS =		$(COMMON)/src/c++/utils
ORDER_BOOK =	$(PROJ_ROOT_DIR)/system_kr/order_book
ORDER_BOOK2 =	$(ORDER_BOOK)/order_book.v2

AUTOMAKE_OPTIONS =	foreign


CC =	g++

include $(PROTO)/Makefile.inc
include $(UTILS)/Makefile.inc
include $(ORDER_BOOK2)/Makefile.inc


INCLUDES =	-I$(PROJ_ROOT_DIR) -I$(COMMON) -I$(COMMON)/src/c++ -I$(ORDER_BOOK) $(SHARECFLAGS)
CFLAGS =	-Wall -std=c++0x -pthread $(DBG_CFLAGS)

LDFLAGS =	-L. $(SHARELDFLAGS)

PANTHEIOS_LIBS= \
        -lpantheios.1.core.gcc44.file64bit \
        -lpantheios.1.fe.N.gcc44.file64bit \
        -lpantheios.1.util.gcc44.file64bit \
        -lpantheios.1.be.N.gcc44.file64bit \
        -lpantheios.1.bec.file.gcc44.file64bit \
        -lpantheios.1.bec.fprintf.gcc44.file64bit \
        -lpantheios.1.util.gcc44.file64bit \
        -lpantheios.1.core.gcc44.file64bit

LIBS= 	-lquickfix \
	-lorder_book2 \
	-lrt \
	-lzmq \
	-lprotobuf \
	-lgtest \
	-lboost_date_time \
	-lboost_program_options \
	-lboost_system \
	-lboost_filesystem


AM_CPPFLAGS =	$(CFLAGS) $(INCLUDES)
AM_LDFLAGS =	$(LDFLAGS) $(LIBS) $(PANTHEIOS_LIBS)
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS =	collect
collect_SOURCES = \
	$(PROJ_ROOT_DIR)/pantheios_be_zmq/pantheios_be_zmq.cpp \
	collect.cpp \
	Application.cpp

COLLECT_OBJS = pantheios_be_zmq.o collect.o Application.o

# all: protos collect

collect : $(COLLECT_OBJS) $(PROTOBUF_OBJS) $(UTIL_OBJS) liborder_book2.a
	$(CC) $(INCLUDES) $(CFLAGS) pantheios_be_zmq.o $(UTIL_OBJS) collect.o Application.o \
	  $(PROTOBUF_OBJS) $(LDFLAGS) $(LIBS) $(PANTHEIOS_LIBS) -o $@ 

collect.o: collect.cpp $(PROTOBUF_OBJS)
	$(CC) -c $(INCLUDES) $(CFLAGS) $<

Application.o: Application.cpp $(PROTOBUF_OBJS)
	$(CC) -o $@ -c $(INCLUDES) $(CFLAGS) $<  

pantheios_be_zmq.o: $(PROJ_ROOT_DIR)/pantheios_be_zmq/pantheios_be_zmq.cpp
	$(CC) -c $(INCLUDES) $(CFLAGS) -Dunix $<

clean:
	rm -f $(COLLECT_OJBS) collect pantheios_be_zmq.o
	rm -f *.o $(PROTO)/*.cc $(PROTO)/*.h $(PROTO)/*.py
	rm -f  $(ORDER_BOOK_OBJS) liborder_book2.a 
	rm -f $(UTIL_OBJS) libcommon_utils.a

developer-clean: clean maintainer-clean-am
	rm -f $(am__CONFIG_DISTCLEAN_FILES)
	rm -rf $(top_srcdir)/autom4te.cache
	rm -rf ./$(DEPDIR)
	rm -rf ./configure m4 Makefile.in aclocal.m4 ./autom4te.cache
	rm -rf config.guess config.h.in* config.h config.status config.log config.sub
	rm -rf depcomp install-sh ltmain.sh stamp-h1 libtool missing msg
	rm -f Makefile
