/* $Id: tv_util.h,v 1.13 2009/09/09 22:38:13 alex Exp $ */
/*******************************************************************************

    tv_util.h

    "timeval" Manipulation Definitions.

*******************************************************************************/

#ifndef  TV_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  TV_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#if defined(__palmos__) || defined(_WIN32)
#    include  "skt_util.h"		/* TCP/IP networking utilities. */
#else
#    include  <sys/time.h>		/* System time definitions. */
#endif
#if !defined(HAVE_TIME_H) || HAVE_TIME_H
#    include  <time.h>			/* Time definitions. */
#else
    typedef  long  time_t ;		/* Time in seconds since 1970. */
    struct  tm {			/* Broken-down time. */
        int  tm_sec ;			/* 0-61 */
        int  tm_min ;			/* 0-59 */
        int  tm_hour ;			/* 0-23 */
        int  tm_mday ;			/* 1-31 */
        int  tm_mon ;			/* 0-11 */
        int  tm_year ;			/* Years since 1900. */
        int  tm_wday ;			/* 0-6 */
        int  tm_yday ;			/* 0-365 */
        int  tm_isdst ;			/* >0=in-effect, 0=not-in-effect, <0=N/A */
        long  tm_gmtoff ;
        char  *tm_zone ;
    } ;
#endif
#include  <rpc/types.h>			/* RPC type definitions. */
#include  <rpc/xdr.h>			/* XDR type definitions. */
#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    Units of truncation for tvTruncate().
*******************************************************************************/

typedef  enum  TvUnit {
    TvDecade,
    TvYear,
    TvMonth,
    TvWeek,
    TvDay,
    TvHour,
    TvMinute
}  TvUnit ;


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  struct  timeval tvAdd P_((struct timeval time1,
                                  struct timeval time2))
    OCD ("tv_util") ;

extern  int  tvCompare P_((struct timeval time1,
                           struct timeval time2))
    OCD ("tv_util") ;

extern  struct  timeval  tvCreate P_((long seconds,
                                      long microseconds))
    OCD ("tv_util") ;

extern  struct  timeval  tvCreateF P_((double fSeconds))
    OCD ("tv_util") ;

extern  double  tvFloat P_((struct timeval time))
    OCD ("tv_util") ;

extern  const  char  *tvShow P_((struct timeval binaryTime,
                                 bool inLocal,
                                 const char *format))
    OCD ("tv_util") ;

extern  struct  timeval  tvSubtract P_((struct timeval time1,
                                        struct timeval time2))
    OCD ("tv_util") ;

extern  struct  tm  *tvT2TM P_((time_t seconds,
                                bool inLocal,
                                struct tm *ymdhms))
    OCD ("tv_util") ;

extern  time_t  tvTM2T P_((struct tm *ymdhms))
    OCD ("tv_util") ;

extern  struct  timeval  tvTOD (
#    if PROTOTYPES && !defined(__cplusplus)
        void
#    endif
    )
    OCD ("tv_util") ;

extern  struct  timeval  tvTruncate P_((struct timeval fullTime,
                                        TvUnit unit))
    OCD ("tv_util") ;

extern  bool_t  xdr_timeval P_((XDR *xdrStream,
                                struct timeval *time))
    OCD ("tv_util") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
