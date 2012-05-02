
#ifndef _K_TIME_UTILS_
#define _K_TIME_UTILS_

#include <time.h>
#include <ostream>

#define NANOS_PER_SECOND 1000000000
#define TIME_STR_LEN 26+1

unsigned long int timespec_delta_millis(const timespec& start, const timespec& end);
timespec timespec_delta(const timespec& start, const timespec& end);

std::ostream& operator<<(std::ostream& out, const timespec ts);

char* timespec2str(timespec ts, char* buf, size_t buflen);

#endif // _K_TIME_UTILS_

