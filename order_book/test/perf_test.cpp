
#include <time.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/tokenizer.hpp>

#include "gtest/gtest.h" 

#include "order_book.h"
#include "order.h"
#include "limit.h"

#include "utils/types.h"
#include "utils/jenkins_hash.cpp"
#include "utils/time_utils.h"
#include "utils/fix_convertors.h"


namespace pt = boost::posix_time;

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

TEST(BookTests, Performance) { 
    capk::order_book book("AMISLOW", 100);
    const int maxIters = 700000;
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
    int side = capk::NO_SIDE;
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
    
    timespec time_start, time_end;
    clock_gettime(CLOCK_REALTIME, &time_start);
    for (int i = 0; i < maxIters; i++) {
        int pos = rand() % 256;
        action = randAction[i];
        inc = randIncrements[i];

        //printf("%d: Position(%d), action(%d), increment(%f) side(%d)\n" , i, pos, action, inc, side);
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
            if (randSides[i] == capk::BID) {
                addSuccess += book.add(entId[pos], capk::BID, 100.00, bidPrice, evtTime, exchTime);
            }
            if (randSides[i] == capk::ASK) {
                addSuccess += book.add(entId[pos], capk::ASK, 100.00, askPrice, evtTime, exchTime);
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
        //std::cout << "Book: " << i << "\n"; 
        //std::cout << book;
        //std::cout << "---------------------------------------------------------\n"; 
    }
    clock_gettime(CLOCK_REALTIME, &time_end);
    timespec d = capk::timespec_delta(time_start, time_end);

    printf("Summary\n--------------------------------------------------\n"); 
    std::cout << "Elapsed time: " << d<< std::endl;
    printf("Total adds %d; successful adds %d\n", countAdd, addSuccess);
    printf("Total modify %d; successful modify %d\n", countModify, modifySuccess);
    printf("Total remove %d; successful remove %d\n", countRemove, removeSuccess);

}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    int result =  RUN_ALL_TESTS();
    return result; 
}

