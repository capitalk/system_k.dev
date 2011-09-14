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

using namespace boost::posix_time;

class FIXConvertors
{
    public:
    static bool UTCTimeStampToPTime(FIX::UtcTimeStamp& t, char* buf, const int buflen, ptime& pt);
    static bool UTCTimeStampToTimespec(FIX::UtcTimeStamp& t, timespec* pt);
    static bool TimespecToUTCTimeStamp(const timespec& ts, FIX::UtcTimeStamp& t);

};













#endif // _CAPK_FIXCONVERTORS_
