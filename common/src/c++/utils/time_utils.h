
#ifndef _CAPK_TIME_UTILS_
#define _CAPK_TIME_UTILS_

#include <sys/types.h>
#include <time.h>
#include <ostream>

#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace fs = boost::filesystem;
namespace dt = boost::gregorian;

namespace capk {

#define NANOS_PER_SECOND 1000000000
#define TIME_STR_LEN 26+1

unsigned long int timespec_delta_millis(const timespec& start, const timespec& end);

timespec timespec_delta(const timespec& start, const timespec& end);

char* timespec2str(timespec ts, char* buf, size_t buflen);

int timespec2int64_t(timespec* ts, int64_t* out);

std::string date_to_string(const dt::date& d);

bool is_bad_filename_char(char c);

std::string remove_bad_filename_chars(const std::string& str);

 
} // namespace capk

std::ostream& operator<<(std::ostream& out, timespec& ts);
std::ostream& operator<<(std::ostream& out, const timespec& ts);

#endif // _CAPK_TIME_UTILS_

