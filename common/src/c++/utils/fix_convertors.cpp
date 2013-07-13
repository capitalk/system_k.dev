
#include "fix_convertors.h"


bool
FIXConvertors::UTCTimeStampToPTime(FIX::UtcTimeStamp& t, char* buf, const int buflen, ptime& pt)
{
#ifdef DEBUG
    ptime time_start(microsec_clock::local_time());
#endif // DEBUG
    // 2011-01-01-12:02:09.987654 = 27 chars including null
    if (!buf || buflen < 27) {
        return false;
    }
    
    memset(buf, 0, buflen);
    std::string ts = FIX::UtcTimeStampConvertor::convert(t, true);
    //const char SPACE = ' ';
    //const char ZERO = '0'; 
    memcpy(buf, ts.c_str(), 4);
    buf[4] = '-';
    memcpy(buf+5, ts.c_str()+4, 2); 
    buf[7] = '-';
    memcpy(buf+8, ts.c_str()+6, 2); 
    buf[10] = ' ';
    memcpy(buf+11, ts.c_str()+9, 12);
    memset(buf+23, '0', 3);// buflen-23);

    pt = time_from_string(buf);
    //std::cerr << "ORIG: (" <<buf << ") " << "PTIME: (" << pt << ")" << std::endl;
#ifdef DEBUG
    ptime time_end(microsec_clock::local_time());
    time_duration duration(time_end - time_start);
    std::cout << __FILE__ << " " <<  __FUNCTION__ << " execution time: " << duration << '\n';
#endif // DEBUG
    return true;

}

void
FIXConvertors::TimespecToUTCTimeStamp(const timespec& ts, FIX::UtcTimeStamp& t)
{
    std::string s;
    int seconds;
    int millis;
    int nanos;

    seconds = ts.tv_sec;
    nanos = ts.tv_nsec;
    millis = (nanos + 100000) / 1000000;
    if (nanos <= 999999999) millis = nanos / 1000000;
    if (nanos <= 99999999) millis = nanos / 1000000;
    if (nanos <= 9999999) millis = nanos / 1000000;
    if (nanos <= 999999) millis = 0;
    struct tm* utc;
    time_t es = seconds;
    utc = gmtime(&es);
    FIX::DateTime fdt = FIX::UtcTimeStamp::fromTm(*utc, millis);
    t.setHMS(fdt.getHour(), fdt.getMinute(), fdt.getSecond(), fdt.getMillisecond());
    t.setYMD(fdt.getYear(), fdt.getMonth(), fdt.getDate());
}


void
FIXConvertors::UTCTimeStampToTimespec(FIX::UtcTimeStamp& t, timespec* pt)
{
    int hour, minute, second, millis;
    //struct timespec ts;
    //int ret2 = clock_getres(CLOCK_REALTIME, &ts);
    //assert(ret2 == 0);
    time_t tt = t.getTimeT(); 
    t.getHMS(hour, minute, second, millis);
    //struct timespec tspec;
    //int ret = clock_gettime(CLOCK_REALTIME, &tspec);
    pt->tv_sec = tt; 
    pt->tv_nsec = millis*1000000;
    //std::cout << "OUT: " << pt->tv_sec << ":" << pt->tv_nsec << "\n";
}

void
FIXConvertors::UTCTimestampStringToTimespec(const std::string& s, timespec* pt)
{
#ifdef DEBUG
    ptime time_start(microsec_clock::local_time());
#endif // DEBUG
    // 2011-01-01-12:02:09.987 = 24 chars including null
    
    FIX::UtcTimeStamp ts = FIX::UtcTimeStampConvertor::convert(s);
    UTCTimeStampToTimespec(ts, pt); 

#ifdef DEBUG
    ptime time_end(microsec_clock::local_time());
    time_duration duration(time_end - time_start);
    std::cout << __FILE__ << " " <<  __FUNCTION__ << " execution time: " << duration << '\n';
#endif // DEBUG

}


