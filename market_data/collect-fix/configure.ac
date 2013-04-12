#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([capitalk],[0.1.0])
AM_INIT_AUTOMAKE([1.11 foreign no-define])
AC_PROG_MAKE_SET
AC_CONFIG_SRCDIR([Application.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
LT_INIT([disable-static])

# check if we compile with debugging options

dbg=false
AC_ARG_ENABLE([debug],AS_HELP_STRING([--enable-debug],[enable debugging]),
	      [if test $enableval == "no"
	       then
		 dbg=false
	       else
	      	 dbg=true
	       fi],
	      [dbg=false]
)

if test $dbg = true
then
	DBG_CFLAGS="-DDEBUG -ggdb  -DLOG -pg -g"
	DBG_LDFLAGS="-pg"
else
	DBG_CFLAGS="-O3 -ULOG -DNDEBUG"
	DBG_LDFLAGS=""
fi
AC_SUBST(DBG_CFLAGS)
AC_SUBST(DBG_LDFLAGS)

# Checks for programs.

AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LEX
AC_PROG_MKDIR_P
AC_PROG_RANLIB

AC_LANG([C++])


# check for packages

STLSOFT_PFX="/opt/stlsoft"
has_stlsoft=false
AC_ARG_WITH(stlsoft, AS_HELP_STRING([--with-stlsoft=<path>],[prefix of stlsoft installation]),
	    [if test $withval == "no"
	     then
		has_stlsoft=false
	     else
		has_stlsoft=true
	     fi],
	    has_stlsoft=false
)

if test $has_stlsoft = true; then
   STLSOFT_PFX=$with_stlsoft
else
   if test "${STLSOFT+set}" = set; then
      STLSOFT_PFX=$STLSOFT
   fi
fi

if test ! -d $STLSOFT_PFX; then
   AC_MSG_ERROR( Canoot find stlsoft installation )
fi
AC_SUBST(STLSOFT_PFX)

# PROTOBUF_PFX="/opt/protobuf"
# has_protobuf=false
# AC_ARG_WITH(protobuf, AS_HELP_STRING([--with-protobuf=<path>],[prefix of protobuf installation]),
# 	    [if test $withval == "no"
# 	     then
# 		has_protobuf=false
# 	     else
# 		has_protobuf=true
# 	     fi],
# 	    has_protobuf=false
# )
# 
# if test $has_protobuf = true; then
#    PROTOBUF_PFX=$with_protobuf
# else
#    if test "${PROTOBUF+set}" = set; then
#       PROTOBUF_PFX=$PROTOBUF
#    fi
# fi
# 
# if test ! -d $PROTOBUF_PFX; then
#    AC_MSG_ERROR( Canoot find protobuf installation )
# fi
# AC_SUBST(PROTOBUF_PFX)


PANTHEIOS_PFX="/opt/pantheios"
has_pantheios=false
AC_ARG_WITH(pantheios, AS_HELP_STRING([--with-pantheios=<path>],[prefix of pantheios installation]),
	    [if test $withval == "no"
	     then
		has_pantheios=false
	     else
		has_pantheios=true
	     fi],
	    has_pantheios=false
)

if test $has_pantheios = true; then
   PANTHEIOS_PFX=$with_pantheios
else
   if test "${PANTHEIOS+set}" = set; then
      PANTHEIOS_PFX=$PANTHEIOS
   fi
fi

if test ! -d $PANTHEIOS_PFX; then
   AC_MSG_ERROR( Canoot find pantheios installation )
fi
AC_SUBST(PANTHEIOS_PFX)


QUICKFIX_PFX="/opt/quickfix"
has_quickfix=false
AC_ARG_WITH(quickfix, AS_HELP_STRING([--with-quickfix=<path>],[prefix of quickfix installation]),
	    [if test $withval == "no"
	     then
		has_quickfix=false
	     else
		has_quickfix=true
	     fi],
	    has_quickfix=false
)

if test $has_quickfix = true; then
   QUICKFIX_PFX=$with_quickfix
else
   if test "${QUICKFIX+set}" = set; then
      QUICKFIX_PFX=$QUICKFIX
   fi
fi

if test ! -d $QUICKFIX_PFX; then
   AC_MSG_ERROR( Canoot find quickfix installation )
fi
AC_SUBST(QUICKFIX_PFX)


# ZMQ_PFX="/opt/zmq"
# has_zmq=false
# AC_ARG_WITH(zmq, AS_HELP_STRING([--with-zmq=<path>],[prefix of zmq installation]),
# 	    [if test $withval == "no"
# 	     then
# 		has_zmq=false
# 	     else
# 		has_zmq=true
# 	     fi],
# 	    has_zmq=false
# )
# 
# if test $has_zmq = true; then
#    ZMQ_PFX=$with_zmq
# else
#    if test "${ZMQ+set}" = set; then
#       ZMQ_PFX=$ZMQ
#    fi
# fi
# 
# if test ! -d $ZMQ_PFX; then
#    AC_MSG_ERROR( Canoot find zmq installation )
# fi
# AC_SUBST(ZMQ_PFX)


SHARECFLAGS="-I/usr/local/include -I${STLSOFT_PFX}/include -I${PANTHEIOS_PFX}/include -I${QUICKFIX_PFX}/include"
AC_SUBST(SHARECFLAGS)

SHARELDFLAGS="-L/usr/local/lib -L${STLSOFT_PFX}/lib -L${PANTHEIOS_PFX}/lib -L${QUICKFIX_PFX}/lib"
AC_SUBST(SHARELDFLAGS)


# Checks for libraries.
AC_CHECK_LIB([boost_date_time], [main])
AC_CHECK_LIB([boost_filesystem], [main])
AC_CHECK_LIB([boost_program_options], [main])
AC_CHECK_LIB([boost_system], [main])
AC_CHECK_LIB([boost_thread], [main])
AC_CHECK_LIB([gtest], [main])
AC_CHECK_LIB([mysqlcppconn], [main])
# AC_CHECK_LIB([order_book], [main])
# AC_CHECK_LIB([order_book2], [main])
AC_CHECK_LIB([protobuf], [main])
AC_CHECK_LIB([pthread], [main])
AC_CHECK_LIB([quickfix], [main])
AC_CHECK_LIB([rt], [main])
AC_CHECK_LIB([zmq], [main])
AC_CHECK_LIB([pantheios.1.core.gcc44.file64bit], [main])


# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h float.h inttypes.h limits.h malloc.h memory.h netdb.h stdint.h stdlib.h string.h sys/socket.h unistd.h quickfix/Session.h boost/program_options.hpp zmq.hpp util/config_server.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([clock_gettime floor localtime_r memchr memmove memset select socket strerror strtol])

AC_CONFIG_FILES([Makefile])


AC_OUTPUT
