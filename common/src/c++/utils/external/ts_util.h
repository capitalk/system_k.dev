/* $Id: ts_util.h,v 1.11 2009/09/09 22:38:13 alex Exp $ */
/*******************************************************************************

    ts_util.h

    "timespec" Manipulation Definitions.

*******************************************************************************/

#ifndef  TS_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  TS_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  "tv_util.h"			/* "timeval" manipulation functions. */
#if defined(VXWORKS)
#    include  <timers.h>		/* POSIX timer definitions. */
#elif defined(__hpux)
#    include  <sys/timers.h>		/* POSIX timer definitions. */
#elif defined(_POSIX_SOURCE)
#    include  <sys/time.h>		/* System time definitions. */
#elif defined(HAVE_TIMESPEC) && !HAVE_TIMESPEC
    struct  timespec {
        time_t  tv_sec ;		/* Seconds. */
        long  tv_nsec ;			/* Nanoseconds. */
    } ;
#endif


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  struct  timespec tsAdd P_((struct timespec time1,
                                   struct timespec time2)) ;

extern  int  tsCompare P_((struct timespec time1,
                           struct timespec time2)) ;

extern  struct  timespec  tsCreate P_((long seconds,
                                       long nanoseconds)) ;

extern  struct  timespec  tsCreateF P_((double fSeconds)) ;

extern  double  tsFloat P_((struct timespec time)) ;

extern  const  char  *tsShow P_((struct timespec binaryTime,
                                 bool inLocal,
                                 const char *format)) ;

extern  struct  timespec  tsSubtract P_((struct timespec time1,
                                         struct timespec time2)) ;

extern  struct  timespec  tsTOD (
#    if PROTOTYPES && !defined(__cplusplus)
        void
#    endif
    ) ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
