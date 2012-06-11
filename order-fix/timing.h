
#ifndef TIMING_68575056_e387_462b_a9a4_f677e9b77288
#define TIMING_68575056_e387_462b_a9a4_f677e9b77288

#include <boost/date_time/posix_time/posix_time.hpp>

#define START_PTIME  boost::posix_time::ptime start_ptime(boost::posix_time::microsec_clock::local_time()); 

#define STOP_PTIME  boost::posix_time::ptime stop_ptime(boost::posix_time::microsec_clock::local_time()); 

#define DIFF_PTIME  boost::posix_time::time_duration ptime_duration(stop_ptime - start_ptime); 

const char* ptime_string(const boost::posix_time::time_duration&); 

#define PTIME_STRING  ptime_string(ptime_duration); 
	

#include <time.h>

#define START_MTIME { \
				timespec start_mtime; \
				clock_gettime(CLOCK_MONOTONIC, &start_mtime); \
				}

#define STOP_MTIME { \
				timespec stop_mtime; \
				clock_gettime(CLOCK_MONOTONIC, &stop_mtime); \
				}

#define T0(NAME) boost::posix_time::ptime NAME(boost::posix_time::microsec_clock::local_time()); 

#define TN(NAME) boost::posix_time::ptime NAME(boost::posix_time::microsec_clock::local_time()); 

#define TDIFF(RES, START, STOP)  boost::posix_time::time_duration RES(STOP - START); 


#define DIFF_MTIME { unsigned long mtime_duration = timespec_delta_micros(start_mtime, stop_mtime); }
#endif // TIMING_68575056-e387-462b-a9a4-f677e9b77288
