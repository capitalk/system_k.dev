
#include "gtest/gtest.h"

#include "utils/FIXConvertors.cpp"

#include "quickfix/Message.h"
#include "quickfix/Parser.h"
#include "quickfix/Values.h"
#include "quickfix/FieldConvertors.h"
#include "quickfix/FieldMap.h"


#include <iostream>
#include <string>

using namespace std;

TEST(FIXTimeConvertor, TimeSanity)
{
    std::string fixtime = "20110330-20:11:05.862";
    std::cout << "Started with string: <" << fixtime << ">" << std::endl;
    FIX::UtcTimeStamp fixts = FIX::UtcTimeStampConvertor::convert(fixtime);
    //FIX::UtcTimeStamp fixts(fixtime);
    char buf[30] = {0};
    ptime pt;
    ASSERT_TRUE(FIXConvertors::UTCTimeStampToPTime(fixts, &buf[0], 30, pt)) ;
    std::cout << "Converted to string: <" << buf << ">" << std::endl;
    std::string outtime = to_iso_extended_string(pt);
    std::cout << "Converted to ptime:  <" << outtime << ">" << std::endl;

    time_duration td1 (1,2,3,123456789);
    time_duration td2 (1,2,3,123458987);
    time_duration diff = td2 - td1;

    std::cout <<"Total millis: " << diff.total_milliseconds() << std::endl;
    std::cout <<"Total micros: " << diff.total_microseconds() << std::endl;
    //ASSERT_TRUE(outtime == buf);
}


int 
main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();  
}


