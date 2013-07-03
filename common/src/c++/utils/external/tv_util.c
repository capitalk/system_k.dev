/* $Id: tv_util.c,v 1.22 2010/03/08 18:38:53 alex Exp alex $ */
/*******************************************************************************

File:

    tv_util.c


Author:    Alex Measday


Purpose:

    These are utility functions used to manipulate (e.g., add and
    subtract) UNIX "timeval"s, which represent time in seconds and
    microseconds.


Procedures:

    tvAdd() - adds two TIMEVALs.
    tvCompare() - compares two TIMEVALs.
    tvCreate() - creates a TIMEVAL from a time expressed in seconds and
        microseconds.
    tvCreateF() - creates a TIMEVAL from a time expressed as a
        floating-point number of seconds.
    tvFloat() - converts a TIMEVAL to a floating-point number of seconds.
    tvShow() - returns a printable representation of a TIMEVAL.
    tvSubtract() - subtracts one TIMEVAL from another.
    tvT2TM() - converts seconds since 1970 to broken-down time.
    tvTM2T() - converts broken-down time to seconds since 1970.
    tvTOD() - returns the current time-of-day (GMT).
    tvTruncate() - truncates a TIMEVAL to the beginning of the year, the day,
        the hour, etc.
    xdr_timeval() - encodes/decodes a TIMEVAL in XDR format.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */
#include  <string.h>			/* Standard C string functions. */
#ifdef __palmos__
#    include  <DateTime.h>		/* PalmOS date/time definitions. */
#    include  <TimeMgr.h>		/* PalmOS time manager functions. */
#endif
#if defined(VMS) && !defined(HAVE_GMTIME)
#    define  HAVE_GMTIME  0		/* VMS/UCX GMTIME() returns NULL. */
#endif
#if defined(HAVE_GETTIMEOFDAY) && !HAVE_GETTIMEOFDAY
#    include  <sys/timeb.h>		/* Time definitions. */
#endif
#include  "tv_util.h"			/* "timeval" manipulation functions. */


/*******************************************************************************
    Time-conversion definitions borrowed from the MINIX library
    ("src/lib/ansi/loc_time.h").  The code is widely available on the web;
    I couldn't find any copyright restrictions.
*******************************************************************************/

#define  YEAR0		1900		/* the first year */
#define  EPOCH_YR	1970		/* EPOCH = Jan 1 1970 00:00:00 */
#define  SECS_DAY	(24UL * 60UL * 60UL)
#define  LEAPYEAR(year)	(!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define  YEARSIZE(year)	(LEAPYEAR ((year)) ? 366 : 365)

static  int  _ytab[2][12] = {		/* # of days in month for non-leap/leap years. */
  { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
  { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
} ;

/*!*****************************************************************************

Procedure:

    tvAdd ()


Purpose:

    Function tvAdd() returns the sum of two TIMEVALs.


    Invocation:

        result = tvAdd (time1, time2) ;

    where

        <time1>		- I
            is the first operand, a time represented by a UNIX TIMEVAL
            structure.
        <time2>		- I
            is the second operand, a time represented by a UNIX TIMEVAL
            structure.
        <result>	- O
            returns, in a UNIX TIMEVAL structure, the sum of TIME1 and
            TIME2.

*******************************************************************************/


struct  timeval  tvAdd (

#    if PROTOTYPES
        struct  timeval  time1,
        struct  timeval  time2)
#    else
        time1, time2)

        struct  timeval  time1 ;
        struct  timeval  time2 ;
#    endif

{    /* Local variables. */
    struct  timeval  result ;



/* Add the two times together. */

    result.tv_sec = time1.tv_sec + time2.tv_sec ;
    result.tv_usec = time1.tv_usec + time2.tv_usec ;
    if (result.tv_usec >= 1000000L) {			/* Carry? */
        result.tv_sec++ ;  result.tv_usec = result.tv_usec - 1000000L ;
    }

    return (result) ;

}

/*!*****************************************************************************

Procedure:

    tvCompare ()


Purpose:

    Function tvCompare() compares two TIMEVALs.


    Invocation:

        comparison = tvCompare (time1, time2) ;

    where

        <time1>		- I
            is a time represented by a UNIX TIMEVAL structure.
        <time2>		- I
            is another time represented by a UNIX TIMEVAL structure.
        <comparison>	- O
            returns an integer indicating how the times compare:
                    -1  if TIME1 < TIME2
                     0  if TIME1 = TIME2
                    +1  if TIME1 > TIME2

*******************************************************************************/


int  tvCompare (

#    if PROTOTYPES
        struct  timeval  time1,
        struct  timeval  time2)
#    else
        time1, time2)

        struct  timeval  time1 ;
        struct  timeval  time2 ;
#    endif

{

    if (time1.tv_sec < time2.tv_sec)
        return (-1) ;				/* Less than. */
    else if (time1.tv_sec > time2.tv_sec)
        return (1) ;				/* Greater than. */
    else if (time1.tv_usec < time2.tv_usec)
        return (-1) ;				/* Less than. */
    else if (time1.tv_usec > time2.tv_usec)
        return (1) ;				/* Greater than. */
    else
        return (0) ;				/* Equal. */

}

/*!*****************************************************************************

Procedure:

    tvCreate ()


Purpose:

    Function tvCreate(), given a time specified in seconds and microseconds,
    creates and returns a TIMEVAL representing that time.


    Invocation:

        result = tvCreate (seconds, microseconds) ;

    where

        <seconds>	- I
            is the whole number of seconds in the time.
        <microseconds>	- I
            is the remaining microseconds in the time.
        <result>	- O
            returns a UNIX TIMEVAL structure representing the specified time.

*******************************************************************************/


struct  timeval  tvCreate (

#    if PROTOTYPES
        long  seconds,
        long  microseconds)
#    else
        seconds, microseconds)

        long  seconds ;
        long  microseconds ;
#    endif

{    /* Local variables. */
    struct  timeval  result ;



    seconds = (seconds < 0) ? 0 : seconds ;
    microseconds = (microseconds < 0) ? 0 : microseconds ;

/* "Normalize" the time so that the microseconds field is less than a million. */

    while (microseconds >= 1000000L) {
        seconds++ ;  microseconds = microseconds - 1000000L ;
    }

/* Return the time in a TIMEVAL structure. */

    result.tv_sec = seconds ;
    result.tv_usec = microseconds ;

    return (result) ;

}

/*!*****************************************************************************

Procedure:

    tvCreateF ()


Purpose:

    Function tvCreateF(), given a floating-point number representing a
    time in seconds and fractional seconds, creates and returns a TIMEVAL
    representing that time.


    Invocation:

        result = tvCreateF (fSeconds) ;

    where

        <fSeconds>	- I
            is a floating-point number representing a time in seconds and
            fractional seconds.
        <result>	- O
            returns a UNIX TIMEVAL structure representing the specified time.

*******************************************************************************/


struct  timeval  tvCreateF (

#    if PROTOTYPES
        double  fSeconds)
#    else
        fSeconds)

        double  fSeconds ;
#    endif

{    /* Local variables. */
    struct  timeval  result ;



    if (fSeconds < 0) {					/* Negative time? */
        result.tv_sec = 0 ;  result.tv_usec = 0 ;
    } else if (fSeconds > (double) LONG_MAX) {		/* Time too large? */
        result.tv_sec = LONG_MAX ;  result.tv_usec = 999999L ;
    } else {						/* Valid time. */
        result.tv_sec = (long) fSeconds ;
        result.tv_usec = (long) ((fSeconds - (double) result.tv_sec)
                                 * 1000000.0) ;
    }

    return (result) ;

}

/*!*****************************************************************************

Procedure:

    tvFloat ()


Purpose:

    Function tvFloat() converts a TIMEVAL structure (seconds and microseconds)
    into the equivalent, floating-point number of seconds.


    Invocation:

        realTime = tvFloat (time) ;

    where

        <time>		- I
            is the TIMEVAL structure that is to be converted to floating point.
        <realTime>	- O
            returns the floating-point representation in seconds of the time.

*******************************************************************************/


double  tvFloat (

#    if PROTOTYPES
        struct  timeval  time)
#    else
        time)

        struct  timeval  time ;
#    endif

{

    return ((double) time.tv_sec + (time.tv_usec / 1000000.0)) ;

}

/*!*****************************************************************************

Procedure:

    tvShow ()


Purpose:

    Function tvShow() returns the ASCII representation of a binary TIMEVAL
    structure (seconds and microseconds).  A caller-specified, strftime(3)
    format is used to format the ASCII representation.

        NOTE:  If the O/S library does not support the gmtime(3) and
        localtime(3) functions, the binary time is assumed to be GMT
        and the in-local argument is ignored.  If the O/S library
        does not support strftime(3), the format argument is ignored
        and the binary time is formatted as "YYYY-DOY-HR:MN:SC.MLS".


    Invocation:

        asciiTime = tvShow (binaryTime, inLocal, format) ;

    where

        <binaryTime>	- I
            is the binary TIMEVAL that will be converted to ASCII.
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
            to tvShow(); tvShow() can be called 4 times before it overwrites
            the result from the earliest call.

*******************************************************************************/


const  char  *tvShow (

#    if PROTOTYPES
        struct  timeval  binaryTime,
        bool  inLocal,
        const  char  *format)
#    else
        binaryTime, inLocal, format)

        struct  timeval  binaryTime ;
        bool  inLocal ;
        const  char  *format ;
#    endif

{    /* Local variables. */
    struct  tm  calendarTime ;
#define  MAX_TIMES  4
    static  char  asciiTime[MAX_TIMES][64] ;
    static  int  current = 0 ;



/* Convert the TIMEVAL to calendar time: year, month, day, etc. */

    (void) tvT2TM ((time_t) binaryTime.tv_sec, inLocal, &calendarTime) ;

/* Format the time in ASCII. */

    current = (current + 1) % MAX_TIMES ;

#if defined(HAVE_STRFTIME) && !HAVE_STRFTIME
    sprintf (asciiTime[current], "%04d-%03d-%02d:%02d:%02d.%03ld",
             calendarTime.tm_year + 1900,
             calendarTime.tm_yday + 1,
             calendarTime.tm_hour,
             calendarTime.tm_min,
             calendarTime.tm_sec,
             (binaryTime.tv_usec % 1000000L) / 1000L) ;
#else
    if (format == NULL) {
        strftime (asciiTime[current], 64, "%Y-%j-%H:%M:%S", &calendarTime) ;
        sprintf (asciiTime[current] + strlen (asciiTime[current]),
                 ".%03ld", (binaryTime.tv_usec % 1000000L) / 1000L) ;
    } else {
        strftime (asciiTime[current], 64, format, &calendarTime) ;
    }
#endif

    return (asciiTime[current]) ;

}

/*!*****************************************************************************

Procedure:

    tvSubtract ()


Purpose:

    Function tvSubtract() subtracts one TIMEVAL from another TIMEVAL.


    Invocation:

        result = tvSubtract (time1, time2) ;

    where

        <time1>		- I
            is the minuend (didn't you graduate from elementary school?),
            a time represented by a UNIX TIMEVAL structure.
        <time2>		- I
            is the subtrahend (I repeat the question), a time represented
            by a UNIX TIMEVAL structure.
        <result>	- O
            returns, in a UNIX TIMEVAL structure, TIME1 minus TIME2.  If
            TIME2 is greater than TIME1, then a time of zero is returned.

*******************************************************************************/


struct  timeval  tvSubtract (

#    if PROTOTYPES
        struct  timeval  time1,
        struct  timeval  time2)
#    else
        time1, time2)

        struct  timeval  time1 ;
        struct  timeval  time2 ;
#    endif

{    /* Local variables. */
    struct  timeval  result ;



/* Subtract the second time from the first. */

    if ((time1.tv_sec < time2.tv_sec) ||
        ((time1.tv_sec == time2.tv_sec) &&
         (time1.tv_usec <= time2.tv_usec))) {		/* TIME1 <= TIME2? */
        result.tv_sec = result.tv_usec = 0 ;
    } else {						/* TIME1 > TIME2 */
        result.tv_sec = time1.tv_sec - time2.tv_sec ;
        if (time1.tv_usec < time2.tv_usec) {
            result.tv_usec = time1.tv_usec + 1000000L - time2.tv_usec ;
            result.tv_sec-- ;				/* Borrow a second. */
        } else {
            result.tv_usec = time1.tv_usec - time2.tv_usec ;
        }
    }

    return (result) ;

}

/*!*****************************************************************************

Procedure:

    tvT2TM ()


Purpose:

    Function tvT2TM() converts a time in seconds since 1970 to a broken-down
    time (year, month, day, etc. in a UNIX "tm" structure).

        NOTE:  If the O/S library supports localtime(3), the input time
        is assumed to be Coordinated Universal Time (UTC) and the in-local
        argument controls whether the broken-down result represents UTC
        or local time.  If localtime(3) is not available, the caller is
        responsible for adjusting the input time to get local time if
        desired.

        NOTE:  The code for this function was borrowed from the MINIX
        implementation of gmtime() ("src/lib/ansi/gmtime.c").  The code
        is widely available on the web; I couldn't find any copyright
        restrictions.


    Invocation:

        result = tvT2TM (seconds, inLocal, &ymdhms) ;

    where

        <seconds>	- I
            is the number of seconds since 12:00 AM on January 1, 1970.
            This time is assumed to be UTC.  If the localtime(3) function
            is not available in the O/S library and the broken-down local
            time is desired, the caller must manually adjust the input
            time before calling tvT2TM().
        <inLocal>	- I
            controls whether the conversions produces the broken-down local
            time (true) or the broken-down UTC (false).  If the localtime(3)
            function is not available in the O/S library and the broken-down
            local time is desired, this argument is ignored.  Instead, the
            caller must manually adjust the input time to account for the
            time zone and for daylight savings time.
        <ymdhms>	- O
            is the address of a "struct tm" that will receive the broken-down
            time.  If this argument is NULL, tvT2TM() will store the result
            in an internal structure and return its address as the function
            value.
        <result>	- O
            returns the address of the broken down time.  If the caller
            supplied a "tm" structure, this address is identical to the
            caller-supplied address.  If the caller specified a NULL "tm"
            address, tvT2TM() returns the address of an internal, statically
            allocated structure containing the broken-down time.

*******************************************************************************/


struct  tm  *tvT2TM (

#    if PROTOTYPES
        time_t  seconds,
        bool  inLocal,
        struct  tm  *ymdhms)
#    else
        seconds, inLocal, ymdhms)

        time_t  seconds ;
        bool  inLocal ;
        struct  tm  *ymdhms ;
#    endif

{    /* Local variables. */
    int  year ;
    unsigned  long  dayclock, dayno ;
    static  struct  tm  breakdown19th ;



/* Use caller-supplied buffer or internal static buffer? */

    if (ymdhms == NULL)  ymdhms = &breakdown19th ;

/* If gmtime(3) and/or localtime(3) are available in the O/S library,
   then use them to compute the broken-down time. */

    if (inLocal) {
#if !defined(HAVE_LOCALTIME) || HAVE_LOCALTIME
        *ymdhms = *(localtime (&seconds)) ;
        return (ymdhms) ;
#endif
    }

#if !defined(HAVE_GMTIME) || HAVE_GMTIME
    *ymdhms = *(gmtime (&seconds)) ;
    return (ymdhms) ;
#endif

/* Compute the number (dayno) of whole days before the specified time and
   the number (dayclock) of seconds remaining in the partial day. */

    dayno = (unsigned long) seconds / SECS_DAY ;
    dayclock = (unsigned long) seconds % SECS_DAY ;

/* Determine the hour, minutes, and seconds in the partial day. */

    ymdhms->tm_sec = dayclock % 60 ;
    ymdhms->tm_min = (dayclock % 3600) / 60 ;
    ymdhms->tm_hour = dayclock / 3600 ;

/* Determine the week day; January 1, 1970 was a Thursday. */

    ymdhms->tm_wday = (dayno + 4) % 7 ;

/* Count up from 1970 to the specified time's year.  The remaining
   number of days is then the day in the year. */

    year = EPOCH_YR ;
    while (dayno >= (unsigned long) YEARSIZE (year)) {
        dayno -= YEARSIZE (year) ;
        year++ ;
    }

    ymdhms->tm_year = year - YEAR0 ;	/* TM year is relative to 1900. */
    ymdhms->tm_yday = dayno ;

/* Determine the month in which the day falls. */

    ymdhms->tm_mon = 0 ;
    while (dayno >= (unsigned long) _ytab[LEAPYEAR (year)][ymdhms->tm_mon]) {
        dayno -= _ytab[LEAPYEAR (year)][ymdhms->tm_mon] ;
        ymdhms->tm_mon++;
    }

    ymdhms->tm_mday = dayno + 1 ;

    ymdhms->tm_isdst = -1 ;		/* Not available. */

    return (ymdhms) ;

}

/*!*****************************************************************************

Procedure:

    tvTM2T ()


Purpose:

    Function tvTM2T() converts a broken-down time (year, month, day, etc.
    in a UNIX "tm" structure) to a time in seconds since 1970.

        NOTE:  If the O/S library supports mktime(3), the input time
        is assumed to be local time and the conversion produces the
        corresponding Coordinated Universal Time (UTC).  If mktime(3)
        is not available, tvTM2T() performs a straight UTC-to-UTC
        conversion.

        NOTE:  This code borrowed ideas from the MINIX ("src/lib/ansi/mktime.c")
        and LIBROCK ("http://www.mibsoftware.com/librock/") implementations of
        mktime(3).  The MINIX code is widely available on the web.


    Invocation:

        seconds = tvTM2T (&ymdhms) ;

    where

        <ymdhms>	- I/O
            is the address of a "struct tm" containing the broken-down time.
            Some fields in the structure may or may not be normalized by
            tvTM2T().  If the mktime(3) function is available in the O/S
            library, the input time is assumed to be local time and the
            result is UTC.  Otherwise, the input time is assumed to be UTC.
        <seconds>	- O
            returns the number of seconds since 12:00 AM on January 1, 1970.
            If the mktime(3) function is available in the O/S library, the
            input time is assumed to be local time and the result is UTC.
            Otherwise, the input time is assumed to be UTC and the result
            is UTC.

*******************************************************************************/


time_t  tvTM2T (

#    if PROTOTYPES
        struct  tm  *ymdhms)
#    else
        ymdhms)

        struct  tm  *ymdhms ;
#    endif

{    /* Local variables. */
#if !defined(HAVE_MKTIME) || HAVE_MKTIME
    return (mktime (ymdhms)) ;
#else
    int  dom, doy, hours, month, year ;
    long  minutes ;
    time_t  seconds ;



/* Normalize the seconds - ignore leap seconds. */

    seconds = ymdhms->tm_sec ;
    if (seconds < 0)  seconds = 0 ;
    minutes = ymdhms->tm_min + (seconds / 60) ;
    seconds = seconds % 60 ;
    ymdhms->tm_sec = (int) seconds ;

/* Normalize the minutes. */

    if (minutes < 0)  minutes = 0 ;
    hours = ymdhms->tm_hour + (minutes / 60) ;
    minutes = minutes % 60 ;
    ymdhms->tm_min = (int) minutes ;

/* Normalize the hours. */

    if (hours < 0)  hours = 0 ;
    dom = ymdhms->tm_mday + (hours / 24) ;
    hours = hours % 24 ;
    ymdhms->tm_hour = hours ;

/* Normalize the day-of-month. */

    if (dom < 1)  dom = 1 ;
    /* ... NEED TO NORMALIZE DAY WITH RESPECT TO MONTH ... */
    ymdhms->tm_mday = dom ;

/* Normalize the month. */

    month = ymdhms->tm_mon ;
    if (month < 0)  month = 0 ;
    year = ymdhms->tm_year + (month / 12) ;
    month = month % 12 ;
    ymdhms->tm_mon = month ;
    ymdhms->tm_year = year ;

/* Determine the year. */

    year += YEAR0 ;
    if (year < EPOCH_YR)  year = EPOCH_YR ;

/* Compute the day-of-year based on the month and day-of-month.  (Ignore
   the day-of-year in the "struct tm"; this allows the caller to convert
   a manually created year-month-day "struct tm" to a binary time in
   seconds.) */

    for (doy = 0 ;  month > 0 ;  month--) {
        doy += _ytab[LEAPYEAR (year)][month] ;
    }

    doy += ymdhms->tm_mday - 1 ;
    ymdhms->tm_yday = doy ;

/* Compute the number of seconds since 1970 preceding the year. */

    seconds = (time_t) (year - EPOCH_YR) * 365UL * SECS_DAY ;
#if EPOCH_YR == 1970			/* Account for leap years since 1968. */
    seconds += (time_t) (year - 1 - 1968) * SECS_DAY ;
#else
    while (year-- > EPOCH_YR) {		/* Add leap day for each leap year. */
        if (LEAPYEAR (year))  seconds += SECS_DAY ;
    }
#endif

/* Add in the day-of-year, hours, minutes, and seconds. */

    minutes = (((doy * 24L) + ymdhms->tm_hour) * 60L) + ymdhms->tm_min ;
    seconds += minutes + ymdhms->tm_sec ;

    return (seconds) ;
#endif
}

/*!*****************************************************************************

Procedure:

    tvTOD ()


Purpose:

    Function tvTOD() returns the current time-of-day (GMT).  tvTOD()
    simply calls the UNIX system function, GETTIMEOFDAY(3), but it
    provides a simple means of using the current time in "timeval"
    calculations without having to allocate a special variable for
    it and making a separate call (i.e., you can "inline" a call to
    tvTOD() within a call to one of the other TV_UTIL functions).

        NOTE:  Under VxWorks, tvTOD() returns the current value of
        the real-time clock, not GMT.


    Invocation:

        currentGMT = tvTOD () ;

    where

        <currentGMT>	- O
            returns, in a UNIX TIMEVAL structure, the current GMT.  (Under
            VxWorks, the current value of the real-time clock is returned
            instead of GMT.)

*******************************************************************************/


struct  timeval  tvTOD (

#    if PROTOTYPES && !defined(__cplusplus)
        void)
#    else
        )
#    endif

{    /* Local variables. */
    struct  timeval  timeOfDay ;



#if defined(__palmos__)
#    define  DELTA_1904_1970	\
	(((17UL * 366UL) + (49UL * 365UL)) * 24UL * 60UL * 60UL)
    unsigned  long  seconds ;	/* Get system time in seconds since 1904. */
    seconds = (unsigned long) TimGetSeconds () ;
    seconds -= DELTA_1904_1970 ;
    timeOfDay.tv_sec = (long) seconds ;
    timeOfDay.tv_usec = 0 ;
#elif defined(HAVE_GETTIMEOFDAY) && !HAVE_GETTIMEOFDAY
#    if defined(_WIN32)
    struct  _timeb  milliTime ;
    _ftime (&milliTime) ;
#    else
    struct  timeb  milliTime ;
    ftime (&milliTime) ;
#    endif
    timeOfDay.tv_sec = (long) milliTime.time ;
    timeOfDay.tv_usec = milliTime.millitm * 1000L ;
#else
    gettimeofday (&timeOfDay, NULL) ;
#endif

    return (timeOfDay) ;

}

/*!*****************************************************************************

Procedure:

    tvTruncate ()


Purpose:

    Function tvTruncate() truncates a TIMEVAL to the beginning of the year,
    the beginning of the day, etc.  The possible units of truncation are
    enumerated in "tv_util.h".


    Invocation:

        truncatedTime = tvTruncate (fullTime, unit) ;

    where

        <fullTime>	- I
            is a time represented as a UNIX TIMEVAL structure.
        <unit>		- I
            is the unit of truncation; see the enumerated type TvUnit defined
            in "tv_util.h".
        <truncatedTime>	- O
            returns a UNIX TIMEVAL structure representing the specified time,
            truncated to the specified unit.

*******************************************************************************/


struct  timeval  tvTruncate (

#    if PROTOTYPES
        struct  timeval  fullTime,
        TvUnit  unit)
#    else
        fullTime, unit)

        struct  timeval  fullTime ;
        TvUnit  unit ;
#    endif

{    /* Local variables. */
    time_t  seconds ;
    struct  timeval  result ;
    struct  tm  gmt ;



/* Break the TIMEVAL down into years, months, days, hours, minutes, and
   seconds and then truncate the appropriate units. */

    seconds = fullTime.tv_sec ;
    tvT2TM (seconds, false, &gmt) ;

    switch (unit) {
    case TvDecade:
        gmt.tm_year = (gmt.tm_year / 10) * 10 ;
    case TvYear:
        gmt.tm_mon = 0 ;
    case TvMonth:
        gmt.tm_mday = 1 ;
    case TvWeek:
        /* ... truncate to day first ... */
    case TvDay:
        gmt.tm_hour = 0 ;
    case TvHour:
        gmt.tm_min = 0 ;
    case TvMinute:
        gmt.tm_sec = 0 ;
    default:
        break ;
    }

    gmt.tm_isdst = -1 ;

/* If truncating to the beginning of the week, call mktime(3) to determine
   the day of the week and subtract that from the day of the month. */

    if (unit == TvWeek) {
        tvTM2T (&gmt) ;
        gmt.tm_mday -= gmt.tm_wday ;
    }

/* Convert the truncated TM structure back to a TIMEVAL. */

    result.tv_sec = (long) tvTM2T (&gmt) ;
    result.tv_usec = 0 ;

    return (result) ;

}

/*!*****************************************************************************

Procedure:

    xdr_timeval ()


Purpose:

    Function xdr_timeval() is an XDR(3)-compatible function that
    encodes/decodes a UNIX "timeval" structure into/from XDR format.


    Invocation:

        successful = xdr_timeval (xdrStream, &time) ;

    where

        <xdrStream>	- I
            is the XDR stream handle returned by one of the xdrTTT_create()
            functions.
        <time>		- I/O
            is a UNIX "timeval" structure.
        <successful>	- O
            returns TRUE if the XDR translation was successful and FALSE
            if it was not.

*******************************************************************************/


bool_t  xdr_timeval (

#    if PROTOTYPES
        XDR  *xdrStream,
        struct  timeval  *time)
#    else
        xdrStream, time)

        XDR  *xdrStream ;
        struct  timeval  *time ;
#    endif

{

    if (!xdr_long (xdrStream, &time->tv_sec))  return (FALSE) ;
    if (!xdr_long (xdrStream, &time->tv_usec))  return (FALSE) ;

    return (TRUE) ;

}
