/* $Id: pragmatics.h,v 1.17 2011/07/18 17:55:15 alex Exp alex $ */
/*******************************************************************************

    pragmatics.h

    Compiler, Operating System, and Debug Logging Particulars.

*******************************************************************************/

#ifndef  PRAGMATICS_H		/* Has the file been INCLUDE'd already? */
#define  PRAGMATICS_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#if defined(HAVE_CONFIG_H) && HAVE_CONFIG_H
#    include  "config.h"		/* Auto-generated configuration file. */
#endif


/*******************************************************************************
    Platform peculiarities.
*******************************************************************************/

#ifndef HAVE_STPCPY
#    define  HAVE_STPCPY  0
#endif
#ifndef HAVE_STRLCAT
#    define  HAVE_STRLCAT  0
#endif
#ifndef HAVE_STRLCPY
#    define  HAVE_STRLCPY  0
#endif

/* Cygwin oddities. */

#ifdef __CYGWIN__
#    define  HAVE_FLOCK  1
#    define  HAVE_WCSDUP  0
#endif

/* NDS oddities. */

#ifdef NDS
#    define  HAVE_CUSERID  0
#    define  HAVE_DUP  0
#    define  HAVE_GETHOSTBYADDR  0
#    define  HAVE_GETLOGIN  0
#    define  HAVE_GETSERVBYNAME  0
#    define  HAVE_H_ADDR  0		/* Field name in "hostent" structure. */
#    define  HAVE_IEEEFP  1		/* IEEE floating-point? */
#    define  HAVE_IN_ADDR_T  0		/* Binary IPV4 address type. */
#    define  HAVE_INTTYPES  0
#    define  HAVE_IOCTL_H  0		/* Declared in <sys/socket.h>. */
#    define  HAVE_LSTAT  0
#    define  HAVE_NETWORK_PORT_0  1
#    define  HAVE_RMDIR  0
#    define  HAVE_SLEEP  0
#    define  HAVE_SOCKLEN_T  0
#    define  HAVE_STDDEF  0
#    define  HAVE_STDERR  0
#    define  HAVE_TIMESPEC  0
#    define  HAVE_WCS  0
#    define  HAVE_WORKING_FORK  0
#    define  IEEEFP  yes		/* Used by "xdr_float.c". */
#    define  MAX_LISTEN_BACKLOG  1
#    define  USE_ENUM_T  1		/* Use fixed-width XDR enum_t in xdr_enum(). */
#    include  <nds.h>			/* NDS definitions. */
#endif

/* PalmOS oddities. */

#ifdef __palmos__
#    define  HAVE_ATOF  0		/* Implemented in "std_util.c". */
#    define  HAVE_CADDR_T  0
#    define  HAVE_ETC_SERVICES  0	/* Until implemented with PalmOS DB. */
#    define  HAVE_FREOPEN  0
#    define  HAVE_FSEEK  0
#    define  HAVE_GETCWD  0
#    define  HAVE_GMTIME  0
#    define  HAVE_IEEEFP  1		/* IEEE floating-point? */
#    define  HAVE_IN_ADDR_T  0		/* Binary IPV4 address type. */
#    define  HAVE_IOCTL  0
#    define  HAVE_LOCALTIME  0
#    define  HAVE_MKTIME  0
#    define  HAVE_MODF  0
#    define  HAVE_POPEN  0
#    define  HAVE_SIGNAL  0
#    define  HAVE_SSCANF  0
#    define  HAVE_SSIZE_T  0
#    define  HAVE_STAT_H  0
#    define  HAVE_STRCSPN  0
#    define  HAVE_STRDUP  0
#    define  HAVE_STRFTIME  0
#    define  HAVE_STRRCHR  0
#    define  HAVE_STRSPN  0
#    define  HAVE_STRTOD  0		/* Implemented in "std_util.c". */
#    define  HAVE_STRTOK  0
#    define  HAVE_TIME_H  0
#    define  HAVE_TIMESPEC  0
#    define  HAVE_WORKING_FORK  0
#    define  HAVE_WCS  0
#    define  IEEEFP  yes		/* Used by "xdr_float.c". */
#    define  MAX_LISTEN_BACKLOG  1
#    undef  NULL
#    define  NULL  ((void *) 0)
#    define  SECTIONS_BY_MODULE
#    define  __stdarg_h			/* Exclude Palm's "unix_stdarg.h". */
#    define  USE_ENUM_T  1		/* Use fixed-width XDR enum_t in xdr_enum(). */
#    include  <HostControl.h>		/* POSE debug/logging functions. */
#endif

/* VMS or VAX-C oddities.  I don't have access to a DEC-C compiler, but I do
   have a copy of its header files.  So, I'm trying to make informed guesses
   as to the differences between the VAX-C and DEC-C compilers. */

#ifdef vms
#    include  <descrip.h>		/* Descriptor definitions. */
#    define  ASSIGN(desc, string)	\
      { desc.dsc$w_length = (string == NULL) ? 0 : strlen (string) ;	\
        desc.dsc$b_dtype = DSC$K_DTYPE_T ;				\
        desc.dsc$b_class = DSC$K_CLASS_S ;				\
        desc.dsc$a_pointer = (string == NULL) ? "" : string ;		\
  }
#    define  HAVE_CUSERID  1
#    define  HAVE_IOCTL  0
#    define  HAVE_LSTAT  0
#    define  HAVE_RMDIR  0
#    define  HAVE_SOCKLEN_T  0
#    define  HAVE_WORKING_FORK  0
#    define  PROTOTYPES  1
#    define  STDC_HEADERS  1
#    ifdef vaxc
#        define  HAVE_GETSERVBYNAME  0
#        define  HAVE_GETTIMEOFDAY  0
#        define  HAVE_IEEEFP  0		/* IEEE floating-point? */
#        define  HAVE_IN_ADDR_T  0	/* Binary IPV4 address type. */
#        define  HAVE_INTTYPES_H  0
#        define  HAVE_POPEN  0		/* Although I do have this available. */
#        define  HAVE_SSIZE_T  0
#        define  HAVE_STRCASECMP  0
#        define  HAVE_STRDUP  0
#        define  HAVE_TIMESPEC  0	/* POSIX high-resolution time. */
#        define  HAVE_WCS  0
#    else
#        define  HAVE_WCSDUP  0
#    endif
#endif

/* Windows oddities. */

#ifdef _WIN32
#    define  _CRT_NONSTDC_NO_DEPRECATE	/* No warnings of non-standard functions. */
#    define  _CRT_SECURE_NO_WARNINGS	/* No warnings of non-secure standard functions. */
#    define  chdir  _chdir
#    define  mkdir  _mkdir
#    define  rmdir  _rmdir
#    define  strcasecmp  _stricmp
#    define  strncasecmp  _strnicmp
#    define  HAVE_CADDR_T  0
#    define  HAVE_CUSERID  0
#    define  HAVE_GETLOGIN  0
#    define  HAVE_GETTIMEOFDAY  0
#    define  HAVE_IEEEFP  1		/* IEEE floating-point? */
#    define  HAVE_IN_ADDR_T  0		/* Binary IPV4 address type. */
#    define  HAVE_INTTYPES_H  0
#    define  HAVE_LSTAT  0
#    define  HAVE_POPEN  1		/* But of limited usefulness! */
#    define  popen  _popen
#    define  pclose  _pclose
#    define  HAVE_SIGNAL  0
#    define  HAVE_SSIZE_T  0
#    if _MSC_VER < 1600				/* Earlier than VS 2010? */
#        define  HAVE_STDINT_H  0
#    else
#        define  HAVE_STDINT_H  1
#    endif
#    define  HAVE_TIMESPEC  0		/* POSIX high-resolution time. */
#    define  HAVE_WORKING_FORK  0
#    define  IEEEFP  yes		/* Used by "xdr_float.c". */
#    define  PROTOTYPES  1
#    define  STDC_HEADERS  1
#endif

/* Windows CE oddities. */

#ifdef _WIN32_WCE
#    define  strcasecmp  _stricmp
#    define  strncasecmp  _strnicmp
#    define  HAVE_CADDR_T  0
#    define  HAVE_GETENV  0
#    define  HAVE_GETTIMEOFDAY  0
#    define  HAVE_IEEEFP  1		/* IEEE floating-point? */
#    define  HAVE_IN_ADDR_T  0		/* Binary IPV4 address type. */
#    define  HAVE_INTTYPES_H  0
#    define  HAVE_POPEN  0
#    define  HAVE_SIGNAL  0
#    define  HAVE_SSIZE_T  0
#    define  HAVE_STRERROR  0
#    define  HAVE_TIMESPEC  0		/* POSIX high-resolution time. */
#    define  HAVE_WORKING_FORK  0
#    define  PROTOTYPES  1
#    define  STDC_HEADERS  1
#endif

/*******************************************************************************
    Compiler peculiarities.
*******************************************************************************/

/* Guess if the Standard C header files are available. */

#ifndef STDC_HEADERS
#    if defined(__STDC__) || defined(__cplusplus)
#        define  STDC_HEADERS  1
#    else
#        define  STDC_HEADERS  0
#    endif
#endif

#ifndef HAVE_STDARG_H
#    if STDC_HEADERS
#        define  HAVE_STDARG_H  1
#    else
#        define  HAVE_STDARG_H  0
#    endif
#endif

/* Are namespaces supported? */

#if defined(__cplusplus) && !defined(OS_ALTERNATIVE_STL_NAMES)
#    define  HAVE_NAMESPACES  1
#else
#    define  HAVE_NAMESPACES  0
#endif

/* Are function prototypes available? */

#if !defined(PROTOTYPES) && !defined(__PROTOTYPES)
#    if defined(__STDC__) || defined(__cplusplus)
#        define  PROTOTYPES  1
#    else
#        define  PROTOTYPES  0
#    endif
#endif

#if PROTOTYPES
#    define  P_(s)  s
#else
#    define  P_(s)  ()
#endif

#ifndef __PROTOTYPES
#    define  __PROTOTYPES  PROTOTYPES
#endif

/* Are const and volatile suppported by the compiler? */

#if !defined(const) && !defined(__STDC__) && !defined(__cplusplus)
#    define  const
#endif

#if !defined(volatile) && !defined(__STDC__) && !defined(__cplusplus)
#    define  volatile
#endif

/* Put functions/constants in different object code sections? */

#ifndef OCD
#    ifdef SECTIONS_BY_MODULE
#        define  OCD(m)  __attribute__ ((__section__ (m)))
#    else
#        define  OCD(m)
#    endif
#endif

/*******************************************************************************
    Data types of interest.
*******************************************************************************/

/* Define the C99 _Bool data type if <stdbool.h> is not available. */

#if defined(__GNUC__) && !defined(HAVE_STDBOOL_H)
#    define  HAVE_STDBOOL_H  1
#endif

#if defined(HAVE_STDBOOL_H) && HAVE_STDBOOL_H
#    include  <stdbool.h>		/* C99 boolean type and values. */
#elif defined(_WIN32) && defined(__cplusplus)
					/* In "yvals.h" via "limits.h" below. */
#elif !defined(_Bool) && (!defined(HAVE__BOOL) || (!HAVE__BOOL))
#    define  _Bool  int
#    if !defined(__cplusplus) || defined(OS_ALTERNATIVE_STL_NAMES)
#        define  OS_OMIT_BOOL  0
#        define  bool  _Bool
#        ifndef true
#            define  true  1
#        endif
#        ifndef false
#            define  false  0
#        endif
#    endif
#endif

/* Character types and such. */

#if defined(__vax__)
#    include  <socket.h>			/* "caddr_t" definition. */
#    define  HAVE_CADDR_T  1
#endif

#if !defined(HAVE_CADDR_T) || HAVE_CADDR_T
#    include  <sys/types.h>			/* System type definitions. */
#else
    typedef  char  *caddr_t ;
#endif

/* Integer data types of specific bit widths.  NOTE that the intN_t types
   are *exact-width* integers; these may not be available on all platforms.
   In the C99 standard, these types are defined in <stdint.h>, which
   is included by <inttypes.h>; some platforms have the UNIX standard
   <inttypes.h> only. */

#if defined(__palmos__)
#    include  <Unix/sys_types.h>		/* System type definitions. */
#    define  int8_t  Int8
#    define  uint8_t  UInt8
#    define  int16_t  Int16
#    define  uint16_t  UInt16
#    define  int32_t  Int32
#    define  uint32_t  UInt32
#    define  __INT_MAX__  32767			/* To correct PRC <limits.h> below. */
#elif defined(HAVE_STDINT_H) && HAVE_STDINT_H
#    include  <stdint.h>			/* C99 integer types. */
#elif !defined(HAVE_INTTYPES_H) || HAVE_INTTYPES_H
#    include  <inttypes.h>			/* C99 integer types. */
#else
#    if defined(vaxc)
#        define  int8_t  char			/* VAX-C doesn't allow "signed char". */
#    else
#        define  int8_t  signed  char
#    endif
#    define  uint8_t  unsigned  char
#    define  int16_t  short			/* Assume short is 16 bits. */
#    define  uint16_t  unsigned  short
#    define  int32_t  int			/* Assume int is 32 bits. */
#    define  uint32_t  unsigned  int
#endif

#if defined(HAVE_IN_ADDR_T) && !HAVE_IN_ADDR_T
    typedef  uint32_t  in_addr_t ;		/* Binary IPV4 address type. */
#endif

#if defined(HAVE_SSIZE_T) && !HAVE_SSIZE_T
    typedef  long  ssize_t ;
#endif

#include  <limits.h>			/* Maximum/minimum value definitions. */

/*******************************************************************************
    Operating system peculiarities.
*******************************************************************************/

/* Errno! */

#if defined(__palmos__)
#    include  <Unix/sys_socket.h>	/* UNIX socket definitions. */
    typedef  Err  errno_t ;
#elif defined(HAVE_ERRNO_H) && !HAVE_ERRNO_H
#    include  "strerror.h"		/* User-supplied error definitions. */
    typedef  int  errno_t ;
#else
#    include  <errno.h>			/* System error definitions. */
    typedef  int  errno_t ;
#endif

#define  SET_ERRNO(error)  (errno = (errno_t) (error))
#define  PUSH_ERRNO  { errno_t  save_errno = (errno_t) errno ;
#define  POP_ERRNO     SET_ERRNO (save_errno) ; }

/* Supply the ANSI strerror(3) function on systems that don't support it. */

#if defined(vms)
#    define  STRERROR(code)  strerror ((code), vaxc$errno)
#    include  <stsdef.h>		/* VMS status code structure. */
#else
#    define  STRERROR  strerror
#endif

/* Pathnames and such. */

#if defined(HAVE_SYS_PARAM_H) && HAVE_SYS_PARAM_H
#    include  <sys/param.h>		/* System parameters. */
#    define  PATH_MAX  MAXPATHLEN
#endif

#ifndef PATH_MAX
#    if defined(vms)
#        include  <nam.h>		/* RMS name block (NAM) definitions. */
#        define  PATH_MAX  NAM$C_MAXRSS
#    elif defined(_WIN32)
#        include  <windows.h>		/* Windows definitions. */
#        define  PATH_MAX  MAX_PATH
#    else
#        define  PATH_MAX  256
#    endif
#endif

#if defined(VXWORKS) || defined(_WIN32)
#    define  MKDIR(path,mode)  mkdir ((path))
#else
#    define  MKDIR(path,mode)  mkdir ((path), (mode))
#endif

/* Signals. */

#if !defined(HAVE_SIGNAL_H) || HAVE_SIGNAL_H
#    if !defined(HAVE_SIGNAL) || HAVE_SIGNAL
#        include  <signal.h>		/* Signal definitions. */
#        define  HAVE_SIGNAL  1
#    endif
#endif

/* fork(2) and popen(3) - assume they're available if not told otherwise. */

#ifndef HAVE_WORKING_FORK
#    define  HAVE_WORKING_FORK  1
#endif

#ifndef HAVE_POPEN
#    define  HAVE_POPEN  1
#endif

/*******************************************************************************
    Debug logging.
*******************************************************************************/

#ifndef I_DEFAULT_GUARD
#    define  I_DEFAULT_GUARD  1
#endif
#ifndef I_DEFAULT_HANDLER
#    define  I_DEFAULT_HANDLER  printf
#endif
#ifndef I_DEFAULT_PARAMS
#    define  I_DEFAULT_PARAMS
#endif
#ifndef I_LEVEL
#    define  I_LEVEL  1
#endif

#ifndef LGI
#    define  LGI							\
        if ((I_LEVEL == 2) || ((I_LEVEL == 1) && (I_DEFAULT_GUARD)))	\
            I_DEFAULT_HANDLER (I_DEFAULT_PARAMS
#endif

/*******************************************************************************
    Error logging.
*******************************************************************************/

#include  "aperror.h"			/* Error reporting function. */

#ifndef E_DEFAULT_GUARD
#    define  E_DEFAULT_GUARD  1
#endif
#ifndef E_DEFAULT_HANDLER
#    define  E_DEFAULT_HANDLER  aperror
#endif
#ifndef E_DEFAULT_PARAMS
#    define  E_DEFAULT_PARAMS
#endif
#ifndef E_LEVEL
#    define  E_LEVEL  2
#endif

#ifndef LGE
#    define  LGE							\
        if ((E_LEVEL == 2) || ((E_LEVEL == 1) && (E_DEFAULT_GUARD)))	\
            E_DEFAULT_HANDLER (E_DEFAULT_PARAMS
#endif


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
