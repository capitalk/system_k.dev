#include <../utils/FIXConvertors.h>
#include <../utils/KTimeUtils.h>
#include "gtest/gtest.h"

#include "quickfix/Message.h"
#include "quickfix/Parser.h"
#include "quickfix/Values.h"
#include "quickfix/FieldConvertors.h"
#include "quickfix/FieldMap.h"

#include <string>
#include <iostream> 
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;

TEST(TimeTests, ToPTime) {
    timespec entryTime, eventTime;
    ptime pt;
    FIX::UtcTimeStamp fixts;
    char orig[30];
    char converted[30];

    std::string timeString = FIX::UtcTimeStampConvertor::convert(fixts, true);
    std::cout << "FIX::UTCTimeStamp: " << timeString << "\n";
    EXPECT_EQ(FIXConvertors::UTCTimeStampToPTime(fixts, &orig[0], (int)sizeof(orig), pt), true);
    std::string ptimeString = to_iso_string(pt);
    std::cout << "boost::ptime: " << ptimeString << "\n";
    
}

TEST(TimeTests, ToTimeSpec) {
    FIX::UtcTimeStamp fixts;
    timespec ts1, ts2;
    clock_gettime(CLOCK_REALTIME, &ts1);
    std::string timeString = FIX::UtcTimeStampConvertor::convert(fixts, true);
    std::cout << "FIX::UTCTimeStamp: " << timeString << "\n";
    EXPECT_EQ(FIXConvertors::UTCTimeStampToTimespec(fixts, &ts2), true);
    time_t secSinceEpoch = ts2.tv_sec;
    struct tm* utc;
    utc = gmtime(&secSinceEpoch);
    std::cout <<  "GMTime: " << asctime(utc) << "\n";
    
    std::cout << "sanity check with second clock_gettime CLOCK_REALTIME";
    secSinceEpoch = ts1.tv_sec;
    utc = gmtime(&secSinceEpoch);
    std::cout << " converted from ts - NOTE: second/milli/micro/nano may not match - " << asctime(utc) << "\n";
    EXPECT_EQ(ts2.tv_sec, ts1.tv_sec);
    int h, m, s, mi = 0;
    fixts.getHMS(h, m, s, mi);
    EXPECT_EQ(h, utc->tm_hour);
    EXPECT_EQ(m, utc->tm_min);
    EXPECT_EQ(s, utc->tm_sec);
    
}

int
main(int argc, char** argv)
{
    std::string s;
    int seconds;
    int millis;
    int nanos;
    std::string sseconds;
    std::string snanos;
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " seconds:nanos" << std::endl;
        exit(-1);
    }
    s = argv[1];
    size_t cp = s.find(":");
    if (cp != std::string::npos) {
        sseconds = s.substr(0, int(cp));
        snanos = s.substr(int(cp)+1, s.length());
        if (snanos.length() != 9) {
            std::cerr << "WARNING - nanos usually have exactly 9 digits - converting anyway\n";
            //exit(-1);
        }
    }
    else {
        std::cerr << "Bad ts format - use seconds:nanos - exiting\n";
        exit(-1);
    }
    seconds = atoi(sseconds.c_str());
    nanos = atoi(snanos.c_str());
    struct timespec ts;
    ts.tv_sec = seconds;
    ts.tv_nsec = nanos;
    FIX::UtcTimeStamp fixts;
    
    FIXConvertors::TimespecToUTCTimeStamp(ts, fixts);
    
    std::string timeString = FIX::UtcTimeStampConvertor::convert(fixts ,true);
    std::cout << timeString << std::endl;

/*

    std::cerr << "Read seconds: " << seconds << " nanos: " << nanos << "\n";
    millis = nanos / 1000000;
    struct tm* utc;
    time_t es = seconds;
    utc = gmtime(&es);
    FIX::DateTime fdt = FIX::UtcTimeStamp::fromTm(*utc, millis);
    FIX::UtcTimeStamp fts(fdt.getHour(), fdt.getMinute(), fdt.getSecond(), fdt.getMillisecond(), fdt.getDate(), fdt.getMonth(), fdt.getYear());
*/
}
