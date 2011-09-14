/*
#include "Logging.h"

#ifdef DEBUG

#include <stdio.h>
#include <pthread.h>
#include <time.h>

namespace itch
  {

  static FILE *LogStream = stdout;

  void DebugLog ( const char *pszFormat, ... )
  {
    if ( pszFormat == NULL )
      return;

    char szTimeBuffer[32] = {0};
    time_t currentTime;

    currentTime = time ( NULL );
    ctime_r ( &currentTime, szTimeBuffer );

    fprintf ( LogStream, "%s [%d] ", szTimeBuffer, ptread_self() );

    va_list vl;
    va_start ( vl, pszFormat );
    vfprintf ( LogStream, pszFormat, vl );
    va_end ( vl );

    if ( !*pszFormat || pszFormat[strlen ( pszFormat ) - 1] != '\n' )
      fprintf ( LogStream, "\n" );
  }

}

#endif
*/