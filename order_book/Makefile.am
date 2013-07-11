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

AUTOMAKE_OPTIONS = subdir-objects

includedir = ./include
common_includedir = ../common/src/c++

ACLOCAL_AMFLAGS = -I m4 

ORDER_BOOK_LIBRARY_VERSION=0:0:0

lib_LTLIBRARIES = liborderbook-0.0.0.la

AM_CPPFLAGS = -I$(common_includedir) \
              -I$(includedir) \
              -std=c++0x

library_includedir=$(includedir)

library_include_HEADERS = include/limit.h \
                           include/order.h \
                           include/order_book.h \
                           include/book_types.h 

liborderbook_0_0_0_la_LDFLAGS = -version-info $(ORDER_BOOK_LIBRARY_VERSION)

liborderbook_0_0_0_la_SOURCES = src/limit.cpp \
                    src/order_book.cpp \
                    src/order.cpp \
                    include/limit.h \
                    include/order.h \
                    include/order_book.h \
                    include/book_types.h \
                    ../common/src/c++/utils/time_utils.cpp \
                    ../common/src/c++/utils/time_utils.h

dist_noinst_SCRIPTS = autogen.sh

