
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/tokenizer.hpp>
#include <algorithm>
#include <iterator>
#include "gtest/gtest.h" 


#include "KTypes.h"

#include "KBook.h"
#include "KOrder.h"
#include "KLimit.h"

#include "utils/hash.cpp"
#include "utils/KTimeUtils.h"
#include "utils/FIXConvertors.h"
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

using namespace std;

KBook book("TESTBOOK", 4);     

TEST(BookTests, LaceBook) { 
    timespec evtTime, exchTime;
    //clock_gettime(CLOCK_REALTIME, &entryTime);
    FIX::UtcTimeStamp exchSndTime;
    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.add(1, BUY, 1, 1.0000, evtTime, exchTime ), 1);

    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.add(2, BUY, 23, 1.0000, evtTime, exchTime ), 1);
        
    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.add(3, SELL, 17, 1.0001, evtTime, exchTime ), 1);

    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.add(4, SELL, 31, 1.045, evtTime, exchTime ), 1);

    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.add(7, BUY, 33, 0.99, evtTime, exchTime ), 1);

    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.add(9, BUY, 123, 0.98, evtTime, exchTime ), 1);

    double best_bid = book.bestPrice(BUY);
    double best_ask = book.bestPrice(SELL);
    std::cerr << "BBO1(" << best_bid << ", " << best_ask << ")\n";
} 

TEST(BookTests, PrintBook1) { 
    std::cout << "INITIAL BOOK" << std::endl;
    std::cout << book;
} 


TEST(BookTests, m1) {
    timespec evtTime, exchTime;
    //clock_gettime(CLOCK_REALTIME, &entryTime);
    FIX::UtcTimeStamp exchSndTime;
    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    double best_bid = book.bestPrice(BUY);
    double best_ask = book.bestPrice(SELL);
    std::cerr << "BBO2(" << best_bid << ", " << best_ask << ")\n" ;
    EXPECT_EQ(book.remove(1, evtTime, exchTime), 1);
    std::cout << book;
    EXPECT_EQ(book.remove(3, evtTime, exchTime), 1);
    std::cout << book;
    EXPECT_EQ(book.remove(2, evtTime, exchTime), 1);
    std::cout << book;
    EXPECT_EQ(book.remove(99, evtTime, exchTime), 0);
    std::cout << book;
    EXPECT_EQ(book.add(10, BUY, 1, 1.0002, evtTime, exchTime ), 1);
    EXPECT_EQ(book.add(11, BUY, 1, 1.0033, evtTime, exchTime ), 1);
    EXPECT_EQ(book.add(12, BUY, 1, 1.0003, evtTime, exchTime ), 1);
    EXPECT_EQ(book.add(13, BUY, 1, 1.0089, evtTime, exchTime ), 1);
    best_bid = book.bestPrice(BUY);
    best_ask = book.bestPrice(SELL);
    EXPECT_EQ(best_bid, 1.0089);
    ASSERT_EQ(best_ask, 1.045);
    std::cerr << "BBO3(" << best_bid << ", " << best_ask << ")\n" ;
    EXPECT_EQ(book.remove(10, evtTime, exchTime), 1);
    std::cout << book;
    EXPECT_EQ(book.remove(11, evtTime, exchTime), 1);
    std::cout << book;
    EXPECT_EQ(book.remove(12, evtTime, exchTime), 1);
    std::cout << book;
    EXPECT_EQ(book.remove(13, evtTime, exchTime), 1);
    std::cout << book;
    EXPECT_EQ(book.add(1, BUY, 1, 1.0000, evtTime, exchTime ), 1);
    std::cout << book;
    EXPECT_EQ(book.add(2, BUY, 23, 1.0000, evtTime, exchTime ), 1);
    std::cout << book;
    EXPECT_EQ(book.add(3, SELL, 17, 1.0001, evtTime, exchTime ), 1);
    std::cout << book;
    best_bid = book.bestPrice(BUY);
    best_ask = book.bestPrice(SELL);
    std::cerr << "BBO4(" << best_bid << ", " << best_ask << ")\n";
    EXPECT_EQ(best_bid, 1.0000);
    ASSERT_EQ(best_ask, 1.0001);

}
/*
TEST(BookTests, BBO) {
    double best_bid = book.bestPrice(BUY);
    double best_ask = book.bestPrice(SELL);
    ASSERT_LT(best_bid, best_ask);
}

TEST(BookTests, CountFirstLevelBids) { 
    uint32_t bestBids = book.getOrderCountAtLimit(BUY, 1.0000); 
    std::cerr << "Found: " << bestBids << " best bids" << std::endl;
    EXPECT_EQ(bestBids, (uint32_t)2); 
} 

TEST(BookTests, FindNonExistentLevelBids) { 
    uint32_t badLevelCount = book.getOrderCountAtLimit(BUY, 100); 
    EXPECT_EQ(badLevelCount, (uint32_t)0); 
} 

TEST(BookTests, GetTotalVolume2) { 
    double volumeAtLevel;
    volumeAtLevel = book.getTotalVolumeAtLimit(BUY, 1.000); 
    EXPECT_EQ(volumeAtLevel, 24); 
} 

TEST(BookTests, Modify1) { 
    double volumeAtLevel;
    timespec evtTime, exchTime;
    FIX::UtcTimeStamp exchSndTime;
    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.modify(1, 5000, evtTime, exchTime), 1);

    volumeAtLevel = book.getTotalVolumeAtLimit(BUY, 1.000); 
    EXPECT_EQ(volumeAtLevel, 5023);
}

TEST(BookTests, Remove1) { 
    double volumeAtLevel;
    timespec evtTime, exchTime;
    FIX::UtcTimeStamp exchSndTime;
    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.remove(1, evtTime, exchTime), 1);

    volumeAtLevel = book.getTotalVolumeAtLimit(BUY, 1.000); 
    EXPECT_EQ(volumeAtLevel, 23);
}

TEST(BookTests, Modify2) { 
    double volumeAtLevel;
    timespec evtTime, exchTime;
    FIX::UtcTimeStamp exchSndTime;
    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.modify(9999, 5000, evtTime, exchTime), 0);
}

TEST(BookTests, PrintBookAfterRemove1) { 
    std::cout << "REMOVED ORDER ID 1" << std::endl;
    std::cout << book;
} 


TEST(BookTests, GetTotalVolume3) { 
    double volumeAtLevel;
    volumeAtLevel = book.getTotalVolumeAtLimit(BUY, 0.98); 
    EXPECT_EQ(volumeAtLevel, 123); 
} 
*/
/*
TEST(BookTests, Remove2) { 
    double volumeAtLevel;
    timespec sndTime, evtTime;
    clock_gettime(CLOCK_MONOTONIC, &sndTime);
    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    EXPECT_EQ(book.removeBid(3432, evtTime, sndTime), 1);
    volumeAtLevel = book.getTotalVolumeAtLimit(BUY, 1.000); 
    EXPECT_EQ(volumeAtLevel, 0);
}
*/
/*
TEST(BookTests, GetTotalVolume4) { 
    double volumeAtLevel;
    volumeAtLevel = book.getTotalVolumeAtLimit(SELL, 1.0001); 
    EXPECT_EQ(volumeAtLevel, 17); 
} 

TEST(BookTests, PrintBook2) { 
    std::cout << book;
} 
*/
void gen_random(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
//        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

#define ACTION_INSERT 0
#define ACTION_MODIFY 1
#define ACTION_REMOVE 2

int 
main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int result =  RUN_ALL_TESTS();
    char x;
    cin >> x;
    const int maxIters = 10000;
    double randIncrements[maxIters];
    int randSides[maxIters];
    int randAction[maxIters];
    char entStr[256][5];
    int entId[256];
    unsigned int countAdd = 0;
    unsigned int addSuccess = 0;
    unsigned int countModify = 0;
    unsigned int modifySuccess = 0;
    unsigned int countRemove = 0;
    unsigned int removeSuccess = 0; 
    int side;
    double inc;
    int action;

    srand(time(0));

    // initialize random vectors
    for (int i = 0; i < maxIters; i++) {
        // bid ask vector
        side = rand() % 2;
        randSides[i] = side;
        // increments vector
        inc = floor(((double)rand()/RAND_MAX)*100+0.5)/100;
        randIncrements[i] = inc;
        // action vector 
        action = rand() % 3;
        randAction[i] = action;
    }

    // Entry Ids
    for (int k = 0; k < 256; k++) {
        gen_random(entStr[k], 5);
        //std::cout << entStr[k] << std::endl; 
        entId[k] = hashlittle(entStr[k], 5, 0);
    }
    
    //ptime time_start(microsec_clock::local_time());
    timespec time_start, time_end;
    clock_gettime(CLOCK_REALTIME, &time_start);
    for (int i = 0; i < maxIters; i++) {
        int pos = rand() % 256;
        action = randAction[i];
        inc = randIncrements[i];

#ifdef DEBUG
        printf("%d: Position(%d), action(%d), increment(%f) side(%d)\n" , i, pos, action, inc, side);
#endif
        timespec exchTime;
        timespec evtTime;
        FIX::UtcTimeStamp exchSndTime;
        clock_gettime(CLOCK_MONOTONIC, &evtTime);
        exchSndTime.setCurrent();
        FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
        
        //clock_gettime(CLOCK_MONOTONIC, &entryTime);
        //std::cout << ctime(&entryTime.tv_sec) << "\n";
        if (action == ACTION_INSERT) {
            double bidPrice = 1.0000 + inc;
            double askPrice = bidPrice + 0.0001;
            if (randSides[i] == BID) {
                addSuccess += book.add(entId[pos], BUY, 100.00, bidPrice, evtTime, exchTime);
            }
            if (randSides[i] == ASK) {
                addSuccess += book.add(entId[pos], SELL, 100.00, askPrice, evtTime, exchTime);
            }
            countAdd++;
        }
        if (action == ACTION_MODIFY) {
            modifySuccess += book.modify(entId[pos], rand(), evtTime, exchTime);
            countModify++;
        }
        if (action == ACTION_REMOVE) {
            removeSuccess += book.remove(entId[pos], evtTime, exchTime);
            countRemove++;
        }
        std::cout << "Book: " << i << "\n"; 
        std::cout << book;
        std::cout << "---------------------------------------------------------\n"; 
    }
    //ptime time_end(microsec_clock::local_time());
    clock_gettime(CLOCK_REALTIME, &time_end);
    timespec d = diff(time_end, time_start);

    printf("Summary\n--------------------------------------------------\n"); 
    std::cout << "Elapsed time: " << d<< std::endl;
    printf("Total adds %d; successful adds %d\n", countAdd, addSuccess);
    printf("Total modify %d; successful modify %d\n", countModify, modifySuccess);
    printf("Total remove %d; successful remove %d\n", countRemove, removeSuccess);

    return result; 
}

