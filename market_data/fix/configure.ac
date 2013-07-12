AC_INIT([system_k], [0.0.0], [bugs@capitalkpartners.com], [system_k], [http://www.capitalkpartners.com/systemk])
AC_PREREQ([2.63])
AM_INIT_AUTOMAKE([1.10 no-define])
AC_CONFIG_HEADERS([config.h])

#Check for compilers 
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MKDIR_P

#AC_CONFIG_SUBDIRS([../../order_book])


# Safety check for wrong --srcdir argument - file must exist in src directory
AC_CONFIG_SRCDIR(src/md_fix.cpp)

AC_ARG_ENABLE([log], 
              [AS_HELP_STRING([--enable-log], [Enable logging support via Pantheios @<:@check@:>@])], 
              [:],
              [enable_log=check])

AM_CONDITIONAL([ENABLE_LOG], [test "$enable_log" = "yes"])

# Check for stlsoft installation
STLSOFT_PFX="/opt/stlsoft"
has_stlsoft=false
AC_ARG_WITH(stlsoft, AS_HELP_STRING([--with-stlsoft=<path>],[path to stlsoft installation]),
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
   AC_MSG_ERROR( Cannot find stlsoft installation in $STLSOFT_PFX)
fi
AC_SUBST(STLSOFT_PFX)

# Check for Google protocol buffers library and protoc compiler
PROTOBUF_PFX="/opt/protobuf"
has_protobuf=false
AC_ARG_WITH(protobuf-libdir, [AS_HELP_STRING([--with-protobuf-libdir=LIB_DIR],
            [location of protocol buffers libraries, defaults to /usr/lib])],
            [PROTOBUF_LIBDIR="$withval"],
            [PROTOBUF_LIBDIR='/usr/lib'])
AC_SUBST([PROTOBUF_LIBDIR])

LDFAGS="$LDFLAGS -L$PROTOBUF_LIBDIR"

AC_CHECK_LIB([protobuf], [main], [], [AC_MSG_ERROR([cannot find protobuf library])])

AC_ARG_WITH([protoc], 
            [AS_HELP_STRING([--with-protoc=PATH], 
            [location of the protoc protocol buffer compiler library, defaults to protoc])], 
            [PROTOC="$withval"],
            [PROTOC='protoc'])
AC_SUBST([PROTOC])

# Check for Pantheios logging files
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
   AC_MSG_ERROR( Cannot find pantheios installation )
fi
AC_SUBST(PANTHEIOS_PFX)


# Check for QuickFIX libraries
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
   AC_MSG_ERROR( Cannot find quickfix installation )
fi
AC_SUBST(QUICKFIX_PFX)

# Check for ZeroMQ
# ZMQ_PFX="/opt/zmq"
# has_zmq=false
# AC_ARG_WITH(zmq, AS_HELP_STRING([--with-zmq=<path>],[prefix of zmq installation]),
#       [if test $withval == "no"
#        then
#     has_zmq=false
#        else
#     has_zmq=true
#        fi],
#       has_zmq=false
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
#    AC_MSG_ERROR( Cannot find zmq installation )
# fi
# AC_SUBST(ZMQ_PFX)

AC_DEFUN([AX_GCC_VERSION], [
  GCC_VERSION=""
  #AX_CHECK_COMPILE_FLAG([-dumpversion],
     #[ax_gcc_version_option=yes],
     #[ax_gcc_version_option=no])

  AS_IF([test "x$GCC" = "xyes"],[
    AS_IF([test "x$ax_gcc_version_option" != "xno"],[
      AC_CACHE_CHECK([gcc version],[ax_cv_gcc_version],[
        ax_cv_gcc_version="`$CC -dumpversion`"
        AS_IF([test "x$ax_cv_gcc_version" = "x"],[
          ax_cv_gcc_version=""
        ])
      ])
      GCC_VERSION=$ax_cv_gcc_version
    ])
  ])
  AC_SUBST([GCC_VERSION])
  GCC_VERSION_MAJOR=$(echo $GCC_VERSION | cut -d'.' -f1)
  GCC_VERSION_MINOR=$(echo $GCC_VERSION | cut -d'.' -f2)
  GCC_VERSION_PATCH=$(echo $GCC_VERSION | cut -d'.' -f3)
  AC_SUBST(GCC_VERSION_MAJOR)
  AC_SUBST(GCC_VERSION_MINOR)
  AC_SUBST(GCC_VERSION_PATCH)
])

AX_GCC_VERSION()


# Checks for libraries.
AC_CHECK_LIB([boost_date_time], [main])
AC_CHECK_LIB([boost_filesystem], [main])
AC_CHECK_LIB([boost_program_options], [main])
AC_CHECK_LIB([boost_system], [main])
AC_CHECK_LIB([boost_thread], [main])
AC_CHECK_LIB([gtest], [main])
AC_CHECK_LIB([mysqlcppconn], [main])
AC_CHECK_LIB([protobuf], [main])
AC_CHECK_LIB([pthread], [main])
AC_CHECK_LIB([quickfix], [main])
AC_CHECK_LIB([rt], [main])
AC_CHECK_LIB([zmq], [main])
#AC_CHECK_LIB([pantheios.1.core.gcc44.file64bit], [main])

# Check header files
AC_CHECK_HEADERS([arpa/inet.h float.h inttypes.h limits.h malloc.h memory.h netdb.h stdint.h stdlib.h string.h sys/socket.h unistd.h quickfix/Session.h boost/program_options.hpp zmq.hpp util/config_server.h])

# Checks for typedefs, structures, and compiler characteristics
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Generate the Makefile
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
