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

inline capk::Side_t char2side_t(char c) { 
        if (c == '0') { 
            return capk::BID; 
        } else if(c == '1') { 
            return capk::ASK; 
        }  
    }













#endif // _CAPK_FIXCONVERTORS_
