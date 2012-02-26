#include "timing.h"

const char* ptime_string(const boost::posix_time::time_duration& d) { 
	std::stringstream ss; 
	ss << d; 
	return ss.str().c_str();
}
