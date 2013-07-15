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

#ifndef _CAPK_FIXCONVERTORS_
#define _CAPK_FIXCONVERTORS_

#include "quickfix/Message.h"
#include "quickfix/Parser.h"
#include "quickfix/Values.h"
#include "quickfix/FieldConvertors.h"
#include "quickfix/FieldMap.h"

#include <chrono>
#include <string>

#include <time.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "types.h"

using namespace boost::posix_time;

class FIXConvertors
{
  public:
  static bool UTCTimeStampToPTime(FIX::UtcTimeStamp& t, char* buf, const int buflen, ptime& pt);
  static void UTCTimeStampToTimespec(FIX::UtcTimeStamp& t, timespec* pt);
  static void TimespecToUTCTimeStamp(const timespec& ts, FIX::UtcTimeStamp& t);
  static void UTCTimestampStringToTimespec(const std::string& s, timespec* pt);
};

inline capk::Side_t const char2side_t(const char c) { 
  if (c == '0') { 
    return capk::BID; 
  } else if(c == '1') { 
    return capk::ASK; 
  }  
  return capk::NO_SIDE;
}

#endif // _CAPK_FIXCONVERTORS_
