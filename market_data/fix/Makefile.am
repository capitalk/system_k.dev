# @configure_input@

# Package-related substitution variables
package = @PACKAGE_NAME@
version = @PACKAGE_VERSION@
tarname = @PACKAGE_TARNAME@
distdir = $(tarname)-$(version)

# Prefix-related substitution variables
prefix = @prefix@
exec_prefix = @exec_prefix@
bindir = @bindir@

ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

common_includedir = ../../common
orderbook_includedir = ../../order_book/include
orderbook_libs = ../../order_book/.libs

bin_PROGRAMS = md_fix

md_fix_CPPFLAGS = -std=c++0x

md_fix_CXXFLAGS = -I $(common_includedir) \
              -I $(orderbook_includedir) \
              -I $(common_includedir)/src/c++/ \
              -I $(common_includedir)/proto \
              -I../.. \
              -I ${PANTHEIOS_PFX}/include \
              -I ${STLSOFT_PFX}/include 

md_fix_LDADD = -L$(orderbook_libs) \
               -lorderbook-0.0.0

md_fix_SOURCES = src/md_broadcast.cpp \
	src/md_broadcast.h \
	src/md_fix.cpp \
 	src/md_parser_qf.cpp \
 	src/md_parser_qf.h \
 	../../common/src/c++/utils/time_utils.h \
 	../../common/src/c++/utils/time_utils.cpp \
 	../../common/src/c++/utils/fix_convertors.h \
 	../../common/src/c++/utils/fix_convertors.cpp \
 	../../common/src/c++/utils/jenkins_hash.h \
 	../../common/src/c++/utils/jenkins_hash.cpp \
 	../../common/src/c++/utils/config_server.h \
 	../../common/src/c++/utils/config_server.cpp \
 	../../common/src/c++/utils/venue_globals.h \
  ../../common/proto/spot_fx_md_1.pb.h \
  ../../common/proto/spot_fx_md_1.pb.cc \
  ../../common/proto/venue_configuration.pb.h \
  ../../common/proto/venue_configuration.pb.cc \
  ../../common/proto/capk_globals.pb.h \
  ../../common/proto/capk_globals.pb.cc 

gcc_version = gcc@GCC_VERSION_MAJOR@@GCC_VERSION_MINOR@

if ENABLE_LOG 
md_fix_SOURCES += ../../common/src/c++/utils/logging.cpp \
                  ../../common/src/c++/utils/logging.h 

md_fix_CPPFLAGS += -DLOG

md_fix_LDADD += -L$(PANTHEIOS_PFX)/lib \
               -lpantheios.1.core.gcc44.file64bit.mt \
               -lpantheios.1.fe.N.gcc44.file64bit.mt \
               -lpantheios.1.util.gcc44.file64bit.mt \
               -lpantheios.1.be.N.gcc44.file64bit.mt \
               -lpantheios.1.bec.file.gcc44.file64bit.mt \
               -lpantheios.1.bec.fprintf.gcc44.file64bit.mt \
               -lpantheios.1.util.gcc44.file64bit.mt \
               -lpantheios.1.core.gcc44.file64bit.mt 
endif

dist_noinst_SCRIPTS = autogen.sh
