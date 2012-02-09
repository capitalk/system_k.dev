
#ifndef _K_TIME_UTILS_
#define _K_TIME_UTILS_

#include <time.h>
#include <ostream>

unsigned long int timespec_delta_millis(const timespec& start, const timespec& end);
timespec timespec_delta(const timespec& start, const timespec& end);

std::ostream& operator<<(std::ostream& out, const timespec ts);


#endif // _K_TIME_UTILS_

