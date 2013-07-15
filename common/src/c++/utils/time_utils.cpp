//
// System K - A framework for building trading applications and analysis
// Copyright (C) 2013 Timir Karia <tkaria@capitalkpartners.com>
//
// This file is part of System K.
//
// System K is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// System K is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with System K.  If not, see <http://www.gnu.org/licenses/>.
//

#include "time_utils.h"

#include <assert.h>
#include <stdint.h>
#include <iomanip>



namespace capk {

// Calculate MILLISECONDS between two timespecs.
unsigned long timespec_delta_millis(const timespec& start, const timespec& end)
{
    uint64_t usec = ((end.tv_sec * 1000000) + end.tv_nsec / 1000) - ((start.tv_sec * 1000000) + start.tv_nsec / 1000) ;
    return (unsigned long)(usec / 1000) ;
}

timespec timespec_delta(const timespec& start, const timespec& end) {
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

char* timespec2str(timespec ts, char* buf, size_t buflen) {
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

int timespec2int64_t(timespec* ts, int64_t* out) {
    if (ts && out) {
        *out = (ts->tv_sec * NANOS_PER_SECOND) + (ts->tv_nsec);
        return 0;
    }
    return -1;
}


std::string date_to_string(const dt::date& d) {
  dt::date_facet* facet = new dt::date_facet("%Y_%m_%d");
  std::stringstream ss;
  ss.imbue(std::locale(std::cout.getloc(), facet));
  ss << d;
  return ss.str();
}

bool is_bad_filename_char(char c) {
  return c == '/' || c == '\\' || c == ' ';
}

std::string remove_bad_filename_chars(const std::string& str) {
  std::string s(str.c_str());
  s.erase(remove_if(s.begin(), s.end(), is_bad_filename_char), s.end());
  return s;
}


} // namespace capk


// global namespace
std::ostream& 
operator<<(std::ostream& out, timespec& ts)
{
    out << std::setfill('0') << std::setprecision(9);
    out << ts.tv_sec << ":" << std::setw(9) << ts.tv_nsec;
    return out;
}

std::ostream& 
operator<<(std::ostream& out, const timespec& ts)
{
    out << std::setfill('0') << std::setprecision(9);
    out << ts.tv_sec << ":" << std::setw(9) << ts.tv_nsec;
    return out;
}



