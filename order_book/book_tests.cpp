
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/tokenizer.hpp>
#include <algorithm>
#include <iterator>
#include "gtest/gtest.h" 

#include "quickfix/Values.h"
#include "quickfix/FieldConvertors.h"

#include "PriceDepthEntry.h" 
#include "KPriceDepthOrderBook.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "utils/hash.cpp"

using namespace std;
using namespace boost::posix_time;


capitalk::PriceDepthEntry* bid1;
capitalk::PriceDepthEntry* bid2; 

TEST(BookTests, InitiallyEmpty) { 
    capitalk::PriceDepthOrderBook book("EUR/USD", 100); 
    EXPECT_EQ(book.numBids(), 0); 
    EXPECT_EQ(book.numOffers(), 0); 	
} 

TEST(BookTests, FindById) { 
    capitalk::PriceDepthOrderBook book ("EUR/USD", 10);
    book.add(bid1); 
    book.add(bid2); 
    capitalk::PriceDepthEntry* found = book.findEntryById(bid1->id()); 
    ASSERT_EQ(found, bid1); 
} 

TEST(BookTests, FindByLevel) { 
    capitalk::PriceDepthOrderBook book ("EUR/USD", 10); 
    book.add(bid1); 
    book.add(bid2); 
    capitalk::PriceDepthEntry* found = book.walkToBidLevel(1); 
    // bid1 has lower price than bid2 
    ASSERT_EQ(found, bid1) << "found wrong entry"; 
} 

TEST(BookTests, FindByPrice) { 
    capitalk::PriceDepthOrderBook book ("EUR/USD", 10); 
    book.add(bid1); 
    book.add(bid2); 
    capitalk::PriceDepthEntry* found = book.walkToBidPrice(2.0); 
    ASSERT_EQ(found, bid2) << " bid1 = " << bid1 << " bid2 = " << bid2 <<  " found = " << found;  
} 
 
TEST(BookTests, ReplaceDoesntIncreaseSize) { 
    capitalk::PriceDepthOrderBook book("EUR/USD", 100); 
    book.add(bid1); 
    book.replace("dummyid", bid1); 
    ASSERT_EQ(book.numBids(), 1); 
} 

/*
TEST(BookTests, DoubleInsertShouldFail) { 
    capitalk::PriceDepthOrderBook book("EUR/USD", 100); 
    book.add(bid1);
    // TODO - KTK removed this line since it was causing segfault
	//ASSERT_FALSE(book.add(bid1)); //Inserting same entry twice"); 
    ASSERT_EQ(book.lastUpdateTime(), bid1->_modified); 
} 
*/

TEST(BookTests, FindAfterDeleteShouldFail) { 
    capitalk::PriceDepthOrderBook book("EUR/USD", 10); 
    book.add(bid1); 
    FIX::UtcTimeStamp now = FIX::UtcTimeStamp(); 
    book.remove(bid1->id(), now, false);
	capitalk::PriceDepthEntry* found = book.findEntryById(bid1->id()); 
	ASSERT_FALSE(found); // not found returns null pointer
    //ASSERT_EQ(book.lastUpdateTime(), now); 
} 

TEST(BookTests, DeleteDecreasesSize) { 
    capitalk::PriceDepthOrderBook book("EUR/USD", 100); 
    book.add(bid1); 
    FIX::UtcTimeStamp now = FIX::UtcTimeStamp(); 
    book.remove("dummyid", now, false); 
    EXPECT_EQ(book.numBids(), 0); 
    EXPECT_EQ(book.numOffers(), 0); 	
    //EXPECT_EQ(book.lastUpdateTime(), now); 
} 

TEST(BookTests, DepthLimit) { 
    capitalk::PriceDepthOrderBook book("EUR/USD", 1); 
    book.add(bid1); 
    book.add(bid2);  

    EXPECT_EQ(book.numBids(), 1); 
    EXPECT_EQ(book.numOffers(), 0); 	
} 


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


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    bid1 = new capitalk::PriceDepthEntry(
        FIX::UtcTimeStamp(),  //created
        FIX::UtcTimeStamp(),  // modified
        FIX::MDEntryType_BID, // side 
        "dummyid", // id
        1.0, // price
        1000); // size 

    bid2 = new capitalk::PriceDepthEntry(
        FIX::UtcTimeStamp(),  //created
        FIX::UtcTimeStamp(),  // modified
        FIX::MDEntryType_BID, // side 
        "dummyid2", // id
        2.0, // price
        200); // size 

    std::cout << "bid1:" << (*bid1) << std::endl;
    std::cout << "bid2:" << (*bid2) << std::endl;
    int result =  RUN_ALL_TESTS();
    //delete bid1;
    //delete bid2; 

    //return result;

/////////////////////////////////////////////////////////
    char x;
    cin >> x;

capitalk::PriceDepthOrderBook  pdob("TESTBOOK", 10);     

#define ACTION_INSERT 0
#define ACTION_MODIFY 1
#define ACTION_REMOVE 2

    const int maxIters = 500000; 
    double randIncrements[maxIters];
    int randSides[maxIters];
    int randAction[maxIters];
    char entStr[256][6];
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
        entStr[k][5] = '\0';
        //std::cout << entStr[k] << std::endl; 
        entId[k] = hashlittle(entStr[k], 5, 0);
    }
    
    ptime time_start(microsec_clock::local_time());
    for (int i = 0; i < maxIters; i++) {
        int pos = rand() % 256;
        action = randAction[i];
        inc = randIncrements[i];
        side = randSides[i];

#ifdef DEBUG
        printf("%d: position(%d), action(%d), increment(%f) side(%d)\n" , i, pos, action, inc, side);
#endif
        capitalk::PriceDepthEntry* pde; 

        if (action == ACTION_INSERT) {
#ifdef DEBUG
        std::cout << "INSERT\n";
#endif
            double bidPrice = 1.0000 + inc;
            double askPrice = bidPrice + 0.0001;
            if (side == 0) {
                //addSuccess += pdob.addBid(entId[pos], 100, bidPrice);
                pde = new capitalk::PriceDepthEntry( 
                    FIX::UtcTimeStamp(),  //created
                    FIX::UtcTimeStamp(),  // modified
                    FIX::MDEntryType_BID, // side 
                    entStr[pos], // id
                    bidPrice, // price
                    100); // size 
                if (pdob.add(pde)) {
                    addSuccess++;
                }
            }
            if (side == 1) {
                //addSuccess += pdob.addAsk(entId[pos], 100, askPrice);
                pde = new capitalk::PriceDepthEntry( 
                    FIX::UtcTimeStamp(),  //created
                    FIX::UtcTimeStamp(),  // modified
                    FIX::MDEntryType_OFFER, // side 
                    entStr[pos], // id
                    askPrice, // price
                    100); // size 
                if (pdob.add(pde)) {
                    addSuccess++;
                }
            }
            countAdd++;
        }
        if (action == ACTION_MODIFY) {
#ifdef DEBUG
        std::cout << "MODIFY\n";
#endif
            //printf("ACTION_MODIFY id(%d)\n",  entId[pos]);
                if (pdob.changeSize(entStr[pos], rand(), FIX::UtcTimeStamp())) {
                    modifySuccess++;
                }
            countModify++;
        }
        if (action == ACTION_REMOVE) {
#ifdef DEBUG
        std::cout << "REMOVE\n";
#endif
            //removeSuccess += pdob.removeBid(entId[pos]);
                if (pdob.remove(entStr[pos], FIX::UtcTimeStamp(), false)) {
                    removeSuccess++;
                }
            countRemove++;
        }
        // count actions
        // modify should increment price and volume
    }
    ptime time_end(microsec_clock::local_time());
    time_duration duration(time_end - time_start);

    printf("Summary\n--------------------------------------------------\n"); 
    std::cout << "Elapsed time: " << duration << std::endl;
    printf("Total adds %d; successful adds %d\n", countAdd, addSuccess);
    printf("Total modify %d; successful modify %d\n", countModify, modifySuccess);
    printf("Total remove %d; successful remove %d\n", countRemove, removeSuccess);

}



//////////////////////////////////////////////////////////

