/* $Id: aperror.c,v 1.7 2011/07/18 17:36:19 alex Exp $ */
/*******************************************************************************

Procedure:

    aperror ()

    Format and Output a System Error Message.


Author:    Alex Measday


Purpose:

    Function APERROR formats and outputs a system error message.  APERROR
    essentially combines the C Library functions FPRINTF and PERROR, allowing
    the programmer to easily construct and output an arbitrary error message.
    Global variable APERROR_PRINT must be set to a non-zero value in order
    for the message to actually be output.  APERROR_PRINT can be set
    explicitly by the application or by setting environment variable,
    "APERROR_PRINT", to a non-zero value.

    NOTE:  APERROR() is NOT reentrant under VxWorks.  Setting APERROR_PRINT
           in one task sets it for all tasks.


    Invocation:

        aperror (format, arg1, arg2, ..., argN) ;

    where

        <format>
            is a normal PRINTF format string.
        <arg1, ..., argN>
            are the arguments referenced by the format string.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#if HAVE_STDARG_H
#    include  <stdarg.h>		/* Variable-length argument lists. */
#else
#    include  <varargs.h>		/* Variable-length argument lists. */
#endif
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */



/*******************************************************************************
    Global APERROR controls:
        APERROR_PRINT - turns APERROR output on or off.  Set APERROR_PRINT to
            a non-zero value to turn output on; set it to zero to turn output
            off.
        APERROR_FILE - is the (FILE *) pointer for APERROR's output device;
            the default destination of error output is STDERR.
*******************************************************************************/

int  aperror_print = 0 ;		/* 0 = no print, !0 = print */
FILE  *aperror_file = NULL ;		/* APERROR output device */


void  aperror (

#    if PROTOTYPES
        const  char  *format,
        ...)
#    else
        format, va_alist)

        char  *format ;
        va_dcl
#    endif

{    /* Local variables. */
    char  *s ;
    va_list  ap ;
    static  char  *envar_value = NULL ;




/* On the first call to this function with the global APERROR_PRINT flag set
   to false, check to see if environment variable, APERROR_PRINT, is defined.
   Waiting until the flag is false before checking the environment variable
   allows this mechanism to work for programs that enable error output during
   initialization and disable it in subsequent processing. */

    if ((envar_value == NULL) & !aperror_print) {
        PUSH_ERRNO ;
        envar_value = getenv ("APERROR_PRINT") ;
        if (envar_value == NULL)
            envar_value = "0" ;
        else
            aperror_print = atoi (envar_value) ;
        POP_ERRNO ;
    }

    if (!aperror_print)  return ;	/* Printing disabled? */

					/* Output file assigned? */
#if !defined(HAVE_STDERR) || HAVE_STDERR
    if (aperror_file == NULL)  aperror_file = stderr ;
#else
    if (aperror_file == NULL)  aperror_file = stdout ;
#endif

/* Get the error message text for the error code. */

    s = STRERROR (errno) ;

/* Format and output the user text to standard error, followed by the
   system error message. */

    PUSH_ERRNO ;

#if HAVE_STDARG_H
    va_start (ap, format) ;
#else
    va_start (ap) ;
#endif
    vfprintf (aperror_file, format, ap) ;
    fprintf (aperror_file, "%s\n", (s == NULL) ? "invalid ERRNO code" : s) ;
    va_end (ap) ;

    POP_ERRNO ;

    return ;

}

#ifdef  TEST
/*******************************************************************************

    Program to test APERROR.  Compile as follows:

        % cc -g -DTEST aperror.c -I<... includes ...>

    Invocation:

        % a.out <errno_code> [ <vmserr_code> ]

*******************************************************************************/

main (argc, argv)
    int  argc ;
    char  *argv[] ;
{

    if (argc < 2) {
        fprintf (stderr, "Usage: test <error_code>\n") ;
        exit (EINVAL) ;
    }

    aperror_print = 1 ;

    errno = atoi (argv[1]) ;
#ifdef VMS
    if ((errno == EVMSERR) && (argc > 2))  vaxc$errno = atoi (argv[2]) ;
#endif
    aperror ("APERROR(%d): ", errno) ;

}
#endif  /* TEST */
