/* $Id: aperror.h,v 1.3 2009/09/09 22:38:13 alex Exp $ */
/*******************************************************************************

    aperror.h

    Formatted System Error Message Output.

*******************************************************************************/

#ifndef  APERROR_H		/* Has the file been INCLUDE'd already? */
#define  APERROR_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  <stdio.h>			/* Standard I/O definitions. */
#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    APERROR_PRINT - turns APERROR output on or off.  Set APERROR_PRINT to a
        a non-zero value to turn output on; set it to zero to turn output off.
    APERROR_FILE - is the (FILE *) pointer for APERROR's output device; the
        default destination of error output is STDERR.
*******************************************************************************/

extern  int  aperror_print ;		/* 0 = no print, !0 = print */
extern  FILE  *aperror_file ;		/* APERROR output device */


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  void  aperror P_((const char *format,
                          ...)) ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
