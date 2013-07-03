/* $Id: ts_util.c,v 1.18 2010/03/08 18:38:53 alex Exp $ */
/*******************************************************************************

File:

    ts_util.c


Author:    Alex Measday


Purpose:

    These are utility functions used to manipulate (e.g., add and
    subtract) POSIX "timespec"s, which represent time in seconds
    and nanoseconds.


Procedures:


    tsAdd() - adds two TIMESPECs.
    tsCompare() - compares two TIMESPECs.
    tsCreate() - creates a TIMESPEC from a time expressed in seconds and
        nanoseconds.
    tsCreateF() - creates a TIMESPEC from a time expressed as a
        floating-point number of seconds.
    tsFloat() - converts a TIMESPEC to a floating-point number of seconds.
    tsShow() - returns a printable representation of a TIMESPEC.
    tsSubtract() - subtracts one TIMESPEC from another.
    tsTOD() - returns the current time-of-day (GMT).

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */
#include  <string.h>			/* Standard C string functions. */
#ifdef __palmos__
#    include  <DateTime.h>		/* PalmOS date/time definitions. */
#    include  <TimeMgr.h>		/* PalmOS time manager functions. */
#else
#    include  <time.h>			/* Time definitions. */
#endif
#if defined(VMS)
#    define  gmtime  localtime		/* VMS/UCX GMTIME() returns NULL. */
#elif defined(VXWORKS)
#    include  <timers.h>		/* POSIX timer definitions. */
#endif
#include  "tv_util.h"			/* "timeval" manipulation functions. */
#include  "ts_util.h"			/* "timespec" manipulation functions. */

/*!*****************************************************************************

Procedure:

    tsAdd ()


Purpose:

    Function tsAdd() returns the sum of two TIMESPECs.


    Invocation:

        result = tsAdd (time1, time2) ;

    where

        <time1>		- I
            is the first operand, a time represented by a POSIX TIMESPEC
            structure.
        <time2>		- I
            is the second operand, a time represented by a POSIX TIMESPEC
            structure.
        <result>	- O
            returns, in a POSIX TIMESPEC structure, the sum of TIME1 and
            TIME2.

*******************************************************************************/


struct  timespec  tsAdd (

#    if PROTOTYPES
        struct  timespec  time1,
        struct  timespec  time2)
#    else
        time1, time2)

        struct  timespec  time1 ;
        struct  timespec  time2 ;
#    endif

{    /* Local variables. */
    struct  timespec  result ;



/* Add the two times together. */

    result.tv_sec = time1.tv_sec + time2.tv_sec ;
    result.tv_nsec = time1.tv_nsec + time2.tv_nsec ;
    if (result.tv_nsec >= 1000000000L) {		/* Carry? */
        result.tv_sec++ ;  result.tv_nsec = result.tv_nsec - 1000000000L ;
    }

    return (result) ;

}

/*!*****************************************************************************

Procedure:

    tsCompare ()


Purpose:

    Function tsCompare() compares two TIMESPECs.


    Invocation:

        comparison = tsCompare (time1, time2) ;

    where

        <time1>		- I
            is a time represented by a POSIX TIMESPEC structure.
        <time2>		- I
            is another time represented by a POSIX TIMESPEC structure.
        <comparison>	- O
            returns an integer indicating how the times compare:
                    -1  if TIME1 < TIME2
                     0  if TIME1 = TIME2
                    +1  if TIME1 > TIME2

*******************************************************************************/


int  tsCompare (

#    if PROTOTYPES
        struct  timespec  time1,
        struct  timespec  time2)
#    else
        time1, time2)

        struct  timespec  time1 ;
        struct  timespec  time2 ;
#    endif

{

    if (time1.tv_sec < time2.tv_sec)
        return (-1) ;				/* Less than. */
    else if (time1.tv_sec > time2.tv_sec)
        return (1) ;				/* Greater than. */
    else if (time1.tv_nsec < time2.tv_nsec)
        return (-1) ;				/* Less than. */
    else if (time1.tv_nsec > time2.tv_nsec)
        return (1) ;				/* Greater than. */
    else
        return (0) ;				/* Equal. */

}

/*!*****************************************************************************

Procedure:

    tsCreate ()


Purpose:

    Function tsCreate(), given a time specified in seconds and nanoseconds,
    creates and returns a TIMESPEC representing that time.


    Invocation:

        result = tsCreate (seconds, nanoseconds) ;

    where

        <seconds>	- I
            is the whole number of seconds in the time.
        <nanoseconds>	- I
            is the remaining nanoseconds in the time.
        <result>	- O
            returns a POSIX TIMESPEC structure representing the specified time.

*******************************************************************************/


struct  timespec  tsCreate (

#    if PROTOTYPES
        long  seconds,
        long  nanoseconds)
#    else
        seconds, nanoseconds)

        long  seconds ;
        long  nanoseconds ;
#    endif

{    /* Local variables. */
    struct  timespec  result ;



    seconds = (seconds < 0) ? 0 : seconds ;
    nanoseconds = (nanoseconds < 0) ? 0 : nanoseconds ;

/* "Normalize" the time so that the nanoseconds field is less than a billion. */

    while (nanoseconds >= 1000000000L) {
        seconds++ ;  nanoseconds = nanoseconds - 1000000000L ;
    }

/* Return the time in a TIMESPEC structure. */

    result.tv_sec = seconds ;
    result.tv_nsec = nanoseconds ;

    return (result) ;

}

/*!*****************************************************************************

Procedure:

    tsCreateF ()


Purpose:

    Function tsCreateF(), given a floating-point number representing
    a time in seconds and fractional seconds, creates and returns a
    TIMESPEC representing that time.


    Invocation:

        result = tsCreateF (fSeconds) ;

    where

        <fSeconds>	- I
            is a floating-point number representing a time in seconds and
            fractional seconds.
        <result>	- O
            returns a POSIX TIMESPEC structure representing the specified time.

*******************************************************************************/


struct  timespec  tsCreateF (

#    if PROTOTYPES
        double  fSeconds)
#    else
        fSeconds)

        double  fSeconds ;
#    endif

{    /* Local variables. */
    struct  timespec  result ;



    if (fSeconds < 0) {					/* Negative time? */
        result.tv_sec = 0 ;  result.tv_nsec = 0 ;
    } else if (fSeconds > (double) LONG_MAX) {		/* Time too large? */
        result.tv_sec = LONG_MAX ;  result.tv_nsec = 999999999L ;
    } else {						/* Valid time. */
        result.tv_sec = (time_t) fSeconds ;
        result.tv_nsec = (long) ((fSeconds - (double) result.tv_sec)
                                 * 1000000000.0) ;
    }

    return (result) ;

}

/*!*****************************************************************************

Procedure:

    tsFloat ()


Purpose:

    Function tsFloat() converts a TIMESPEC structure (seconds and nanoseconds)
    into the equivalent, floating-point number of seconds.


    Invocation:

        realTime = tsFloat (time) ;

    where

        <time>		- I
            is the TIMESPEC structure that is to be converted to floating point.
        <realTime>	- O
            returns the floating-point representation in seconds of the time.

*******************************************************************************/


double  tsFloat (

#    if PROTOTYPES
        struct  timespec  time)
#    else
        time)

        struct  timespec  time ;
#    endif

{

    return ((double) time.tv_sec + (time.tv_nsec / 1000000000.0)) ;

}

/*!*****************************************************************************

Procedure:

    tsShow ()


Purpose:

    Function tsShow() returns the ASCII representation of a binary TIMESPEC
    structure (seconds and nanoseconds).  A caller-specified, strftime(3)
    format is used to format the ASCII representation.


    Invocation:

        asciiTime = tsShow (binaryTime, inLocal, format) ;

    where

        <binaryTime>	- I
            is the binary TIMESPEC that will be converted to ASCII.
        <inLocal>	- I
            specifies (true or false) if the binary time is to be adjusted
            to local time by adding the GMT offset.
        <format>	- I
            is the STRFTIME(3) format used to format the binary time in
            ASCII.  If this argument is NULL, the time is formatted as
            "YYYY-DOY-HR:MN:SC.MLS".
        <asciiTime>	- O
            returns a pointer to an ASCII string, which contains the formatted
            time.  The ASCII string is stored in multiply-buffered memory local
            to tsShow(); tsShow() can be called 4 times before it overwrites
            the result from the earliest call.

*******************************************************************************/


const  char  *tsShow (

#    if PROTOTYPES
        struct  timespec  binaryTime,
        bool  inLocal,
        const  char  *format)
#    else
        binaryTime, inLocal, format)

        struct  timespec  binaryTime ;
        bool  inLocal ;
        const  char  *format ;
#    endif

{    /* Local variables. */
    struct  tm  calendarTime ;
#define  MAX_TIMES  4
    static  char  asciiTime[MAX_TIMES][64] ;
    static  int  current = 0 ;



/* Convert the TIMESPEC to calendar time: year, month, day, etc. */

#ifdef VXWORKS
    if (inLocal)
        localtime_r ((time_t *) &binaryTime.tv_sec, &calendarTime) ;
    else
        gmtime_r ((time_t *) &binaryTime.tv_sec, &calendarTime) ;
#else
    if (inLocal)
        calendarTime = *(localtime ((time_t *) &binaryTime.tv_sec)) ;
    else
        calendarTime = *(gmtime ((time_t *) &binaryTime.tv_sec)) ;
#endif

/* Format the time in ASCII. */

    current = (current + 1) % MAX_TIMES ;

    if (format == NULL) {
        strftime (asciiTime[current], 64, "%Y-%j-%H:%M:%S", &calendarTime) ;
        sprintf (asciiTime[current] + strlen (asciiTime[current]),
                 ".%06ld", (binaryTime.tv_nsec % 1000000000L) / 1000L) ;
    } else {
        strftime (asciiTime[current], 64, format, &calendarTime) ;
    }

    return (asciiTime[current]) ;

}

/*!*****************************************************************************

Procedure:

    tsSubtract ()


Purpose:

    Function tsSubtract() subtracts one TIMESPEC from another TIMESPEC.


    Invocation:

        result = tsSubtract (time1, time2) ;

    where

        <time1>		- I
            is the minuend, a time represented by a POSIX TIMESPEC structure.
        <time2>		- I
            is the subtrahend, a time represented by a POSIX TIMESPEC structure.
        <result>	- O
            returns, in a POSIX TIMESPEC structure, TIME1 minus TIME2.  If
            TIME2 is greater than TIME1, then a time of zero is returned.

*******************************************************************************/


struct  timespec  tsSubtract (

#    if PROTOTYPES
        struct  timespec  time1,
        struct  timespec  time2)
#    else
        time1, time2)

        struct  timespec  time1 ;
        struct  timespec  time2 ;
#    endif

{    /* Local variables. */
    struct  timespec  result ;



/* Subtract the second time from the first. */

    if ((time1.tv_sec < time2.tv_sec) ||
        ((time1.tv_sec == time2.tv_sec) &&
         (time1.tv_nsec <= time2.tv_nsec))) {		/* TIME1 <= TIME2? */
        result.tv_sec = result.tv_nsec = 0 ;
    } else {						/* TIME1 > TIME2 */
        result.tv_sec = time1.tv_sec - time2.tv_sec ;
        if (time1.tv_nsec < time2.tv_nsec) {
            result.tv_nsec = time1.tv_nsec + 1000000000L - time2.tv_nsec ;
            result.tv_sec-- ;				/* Borrow a second. */
        } else {
            result.tv_nsec = time1.tv_nsec - time2.tv_nsec ;
        }
    }

    return (result) ;

}

/*!*****************************************************************************

Procedure:

    tsTOD ()


Purpose:

    Function tsTOD() returns the current time-of-day (GMT).  tsTOD()
    simply calls the POSIX function, CLOCK_GETTIME(3), but it provides
    a simple means of using the current time in "timespec" calculations
    without having to allocate a special variable for it and making a
    separate call (i.e., you can "inline" a call to tsTOD() within a
    call to one of the other TS_UTIL functions).


    Invocation:

        currentGMT = tsTOD () ;

    where

        <currentGMT>	- O
            returns, in a POSIX TIMESPEC structure, the current GMT.

*******************************************************************************/


struct  timespec  tsTOD (

#    if PROTOTYPES && !defined(__cplusplus)
        void)
#    else
        )
#    endif

{    /* Local variables. */
    struct  timespec  timeOfDay ;



#if defined(VXWORKS)
    clock_gettime (CLOCK_REALTIME, &timeOfDay) ;
#else
    struct  timeval  currentGMT = tvTOD () ;
    timeOfDay.tv_sec = currentGMT.tv_sec ;
    timeOfDay.tv_nsec = currentGMT.tv_usec * 1000 ;
#endif

    return (timeOfDay) ;

}
