#include "time_utils.h"

#include <assert.h>

namespace capk {

// Calculate milliseconds between two timespecs.
unsigned long timespec_delta_millis(const timespec& start, const timespec& end)
{
    uint64_t usec = ((end.tv_sec * 1000000) + end.tv_nsec / 1000) - ((start.tv_sec * 1000000) + start.tv_nsec / 1000) ;
    return (unsigned long)(usec / 1000) ;
}

timespec timespec_delta(const timespec& start, const timespec& end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = NANOS_PER_SECOND+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

char*
timespec2str(timespec ts, char* buf, size_t buflen) 
{
	time_t t_secs = ts.tv_sec + (ts.tv_nsec/NANOS_PER_SECOND);
	assert(buf);
	if (buf && (buflen >= TIME_STR_LEN)) {
		return ctime_r(&t_secs, buf);
	}
	else {
		buf[0] = NULL;
		return buf;
	}
}

int 
timespec2int64_t(timespec* ts, int64_t* out) 
{
    if (ts && out) {
        *out = (ts->tv_sec * NANOS_PER_SECOND) + (ts->tv_nsec);
        return 0;
    }
    return -1;
}


} // namespace capk


// global namespace
std::ostream& 
operator<<(std::ostream& out, timespec& ts)
{
    out << ts.tv_sec << ":" << ts.tv_nsec;
    return out;
}

std::ostream& 
operator<<(std::ostream& out, const timespec& ts)
{
    out << ts.tv_sec << ":" << ts.tv_nsec;
    return out;
}



