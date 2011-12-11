
#ifndef _K_TIME_UTILS_
#define _K_TIME_UTILS_

#include <time.h>
#include <ostream>

timespec timespec_delta(timespec start, timespec end);

std::ostream& operator<<(std::ostream& out, const timespec ts);


#endif // _K_TIME_UTILS_

