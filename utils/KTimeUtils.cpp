#include <KTimeUtils.h>

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
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

std::ostream& 
operator<<(std::ostream& out, const timespec ts)
{
    out << ts.tv_sec << ":" << ts.tv_nsec;
    return out;
}
