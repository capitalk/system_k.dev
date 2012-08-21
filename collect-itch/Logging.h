#ifndef LOGGING_H
#define LOGGING_H

#if !defined(NDEBUG)

#include <stdio.h>

#define LOG(format, ...) printf ( "%s::%s (%d): " format "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ )

#else // defined(NDEBUG)

#define LOG(format, ...)

#endif

#endif // LOGGING_H
