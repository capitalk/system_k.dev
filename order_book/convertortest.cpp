
#include "gtest/gtest.h"
#include "../utils/FIXConvertors.h"
#include "../utils/KTimeUtils.h"

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
    // Get fix time
    std::string timeString = FIX::UtcTimeStampConvertor::convert(fixts, true);
    std::cout << "FIX::UTCTimeStamp: " << timeString << "\n";
    FIXConvertors::UTCTimeStampToTimespec(fixts, &ts2);
    std::cout << "TS Seconds: " << ts2.tv_sec << " nanoseconds: " << ts2.tv_nsec << "\n";
    time_t secSinceEpoch = ts2.tv_sec;
    struct tm* utc;
    utc = gmtime(&secSinceEpoch);
    std::cout <<  "GMTime: " << asctime(utc) << "\n";
    
    std::cout << "Sanity check with second clock_gettime CLOCK_REALTIME";
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

TEST(TimeTests, ToUTC) {
    FIX::UtcTimeStamp fixts;
    timespec ts1, ts2;
    timespec ts;
    ts.tv_sec = time(NULL);
    ts.tv_nsec = 123000000;
    fixts.clearDate();
    fixts.clearTime();
    FIXConvertors::TimespecToUTCTimeStamp(ts, fixts);
    std::string timeString = FIX::UtcTimeStampConvertor::convert(fixts, true);
    std::cout << "FIX::UTCTimeStamp: " << timeString << "\n";
    EXPECT_LT(fixts.getMillisecond(), 1000);
    
    ts.tv_nsec = 45000000;
    fixts.clearDate();
    fixts.clearTime();
    FIXConvertors::TimespecToUTCTimeStamp(ts, fixts);
    timeString = FIX::UtcTimeStampConvertor::convert(fixts, true);
    std::cout << "FIX::UTCTimeStamp: " << timeString << "\n";
    EXPECT_LT(fixts.getMillisecond(), 100);

    ts.tv_nsec = 6000000;
    fixts.clearDate();
    fixts.clearTime();
    FIXConvertors::TimespecToUTCTimeStamp(ts, fixts);
    timeString = FIX::UtcTimeStampConvertor::convert(fixts, true);
    std::cout << "FIX::UTCTimeStamp: " << timeString << "\n";
    EXPECT_LT(fixts.getMillisecond(), 10);

}
/*
TEST(TimeTests, Convert) {
    FIX::UtcTimeStamp fixts;
    timespec ts1, ts2;
    for (int i = 0; i<1000000000; i++) {
        clock_gettime(CLOCK_REALTIME, &ts2);
        FIXConvertors::TimespecToUTCTimeStamp(fixts, &ts2);
        std::cerr << fixts.getMillisecond(); << std::endl;
    }

}
*/
int
main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    int result =  RUN_ALL_TESTS();
    char buf[30];
    ptime pt;
    timespec ts;
    timespec start, stop; 
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i<1000000; i++) {
        FIX::UtcTimeStamp fixutc;
        FIXConvertors::UTCTimeStampToTimespec(fixutc, &ts);
        FIXConvertors::UTCTimeStampToPTime(fixutc, &buf[0], (int)sizeof(buf), pt);
        //std::cout << ts.tv_sec << ":" << ts.tv_nsec << "\n";
        //std::string x = FIX::UtcTimeStampConvertor::convert(fixutc, true);
        //std::cout << x << "\n";
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    timespec d = timespec_delta(start, stop);
    std::cout << "Seconds: " << d.tv_sec << " nanseconds: " << d.tv_nsec << "\n";

}

/*
int
main(int argc, char** argv)
{
    std::string s;
    int seconds;
    int millis;
    int micros;
    std::string sseconds;
    std::string smicros;
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " seconds:microseconds" << std::endl;
        exit(-1);
    }
    s = argv[1];
    size_t cp = s.find(":");
    if (cp != std::string::npos) {
        sseconds = s.substr(0, int(cp));
        smicros = s.substr(int(cp)+1, s.length());
        if (smicros.length() != 9) {
            std::cerr << "Bad ts format - microseconds must have exactly 9 digits - exiting\n";
            exit(-1);
        }
    }
    else {
        std::cerr << "Bad ts format - use seconds:microseconds - exiting\n";
        exit(-1);
    }
    seconds = atoi(sseconds.c_str());
    micros = atoi(smicros.c_str());
    std::cerr << "Read seconds: " << seconds << " micros: " << micros << "\n";
    millis = micros / 1000000;
    struct tm* utc;
    time_t es = seconds;
    utc = gmtime(&es);
    FIX::DateTime fdt = FIX::UtcTimeStamp::fromTm(*utc, millis);
    FIX::UtcTimeStamp fts(fdt.getHour(), fdt.getMinute(), fdt.getSecond(), fdt.getMillisecond(), fdt.getDate(), fdt.getMonth(), fdt.getYear());
    std::string timeString = FIX::UtcTimeStampConvertor::convert(fts ,true);
    std::cout << timeString << std::endl;
}
*/
