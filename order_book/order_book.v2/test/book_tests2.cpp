
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

class FullBookTest : public ::testing::Test {
  public:
   FullBookTest() {};
   ~FullBookTest() {};

   virtual void SetUp() {
      ob = new capk::KBook("FOOBAR", 100);
   }

   virtual void TearDown() {
      delete ob;
   }

   capk::KBook* ob;
};

class InitBookTest : public ::testing::Test {
 public:
  InitBookTest() {};

  ~InitBookTest() {};

  virtual void SetUp() {
    ob = new capk::KBook("EURUSD", 5);
  }

  virtual void TearDown() {
    delete ob;
  }

  capk::KBook* ob;     

};

TEST_F(InitBookTest, EmptyBook) {
  EXPECT_EQ(capk::NO_BID, ob->bestPrice(capk::BID));
  EXPECT_EQ(capk::NO_ASK, ob->bestPrice(capk::ASK));
}


TEST_F(InitBookTest, Add1Bid) {
  int orderId = 1;
  double qty = 1000000;
  double price = 1.00000;
  timespec timeStamp;
  clock_gettime(CLOCK_REALTIME, &timeStamp);
  EXPECT_EQ(ob->add(orderId, capk::BID, qty, price, timeStamp, timeStamp), 1);
  EXPECT_EQ(price, ob->bestPrice(capk::BID));
  capk::pKOrder order = ob->getOrder(orderId);  
  EXPECT_EQ(order->getPrice(), price);
  EXPECT_EQ(order->getSize(), qty);
  EXPECT_EQ(order->getBuySell(), capk::BID);
}

TEST_F(InitBookTest, Add1Ask) {
  int orderId = 2;
  double qty = 2000000;
  double price = 2.00000;
  timespec timeStamp;
  clock_gettime(CLOCK_REALTIME, &timeStamp);
  EXPECT_EQ(ob->add(orderId, capk::ASK, qty, price, timeStamp, timeStamp), 1);
  EXPECT_EQ(price, ob->bestPrice(capk::ASK));
  capk::pKOrder order = ob->getOrder(orderId);  
  EXPECT_EQ(order->getPrice(), price);
  EXPECT_EQ(order->getSize(), qty);
  EXPECT_EQ(order->getBuySell(), capk::ASK);
}

TEST_F(InitBookTest, GetNonExistentOrder) {
  int orderId = 22222;
  capk::pKOrder order = ob->getOrder(orderId);  
  EXPECT_EQ(capk::pKOrder(), order);
  EXPECT_EQ(order, capk::pKOrder());
}

TEST_F(InitBookTest, DeleteNonExistentOrder) {
  int orderId = 22222;
  timespec timeStamp;
  clock_gettime(CLOCK_REALTIME, &timeStamp);
  // Removing a non-exitent order returns false - should it? 
  EXPECT_EQ(ob->remove(orderId, timeStamp, timeStamp), 0);
}

TEST_F(InitBookTest, Add1Delete1Ask) {
  int orderId = 2;
  double qty = 2000000;
  double price = 2.00000;
  timespec timeStamp;
  clock_gettime(CLOCK_REALTIME, &timeStamp);
  // Add order
  EXPECT_EQ(ob->add(orderId, capk::ASK, qty, price, timeStamp, timeStamp), 1);
  // Check that order is there
  EXPECT_EQ(price, ob->bestPrice(capk::ASK));
  capk::pKOrder order = ob->getOrder(orderId);  
  EXPECT_EQ(order->getPrice(), price);
  EXPECT_EQ(order->getSize(), qty);
  EXPECT_EQ(order->getBuySell(), capk::ASK);
  // Remove the order
  EXPECT_EQ(ob->remove(orderId, timeStamp, timeStamp), 1);
  // Check that order does not exist
  order = ob->getOrder(orderId);  
  EXPECT_EQ(capk::pKOrder(), order);
  EXPECT_EQ(order, capk::pKOrder());
}

TEST_F(InitBookTest, Add1Delete1Bid) {
  int orderId = 2;
  double qty = 2000000;
  double price = 2.00000;
  timespec timeStamp;
  clock_gettime(CLOCK_REALTIME, &timeStamp);
  // Add order
  EXPECT_EQ(ob->add(orderId, capk::BID, qty, price, timeStamp, timeStamp), 1);
  // Check that order is there
  EXPECT_EQ(price, ob->bestPrice(capk::BID));
  capk::pKOrder order = ob->getOrder(orderId);  
  EXPECT_EQ(order->getPrice(), price);
  EXPECT_EQ(order->getSize(), qty);
  EXPECT_EQ(order->getBuySell(), capk::BID);
  // Remove the order
  EXPECT_EQ(ob->remove(orderId, timeStamp, timeStamp), 1);
  // Check that order does not exist
  order = ob->getOrder(orderId);  
  EXPECT_EQ(capk::pKOrder(), order);
  EXPECT_EQ(order, capk::pKOrder());
}

TEST_F(InitBookTest, ImproveBid) {
  int bid1Id = 21;
  int bid2Id = 43;
  double qty1 = 11111111;
  double qty2 = 5;
  double price1 = 1.00000;
  double price2 = 2.00000;
  timespec timeStamp;
  clock_gettime(CLOCK_REALTIME, &timeStamp);
  // Add order
  EXPECT_EQ(ob->add(bid1Id, capk::BID, qty1, price1, timeStamp, timeStamp), 1);
  EXPECT_EQ(ob->add(bid2Id, capk::BID, qty2, price2, timeStamp, timeStamp), 1);
  // Check that order is there
  EXPECT_EQ(price2, ob->bestPrice(capk::BID));
  capk::pKOrder order = ob->getOrder(bid2Id);  
  EXPECT_EQ(order->getPrice(), price2);
  EXPECT_EQ(order->getSize(), qty2);
  EXPECT_EQ(order->getBuySell(), capk::BID);
  // Check that the other order still exists
  order = ob->getOrder(bid1Id);  
  EXPECT_EQ(order->getPrice(), price1);
  EXPECT_EQ(order->getSize(), qty1);
  EXPECT_EQ(order->getBuySell(), capk::BID);
}

TEST_F(InitBookTest, MutipleOrdersAtPrice) {
  int bid1Id = 1;
  int bid2Id = 2;
  double qty1 = 6;
  double qty2 = 5;
  double price1 = 1.00001;
  double price2 = 1.00001;
  timespec timeStamp;
  clock_gettime(CLOCK_REALTIME, &timeStamp);
  // Add order
  EXPECT_EQ(ob->add(bid1Id, capk::BID, qty1, price1, timeStamp, timeStamp), 1);
  EXPECT_EQ(ob->add(bid2Id, capk::BID, qty2, price2, timeStamp, timeStamp), 1);
  // Check that order is there
  EXPECT_EQ(price2, ob->bestPrice(capk::BID));
  capk::pKOrder order = ob->getOrder(bid2Id);  
  EXPECT_EQ(order->getPrice(), price2);
  EXPECT_EQ(order->getSize(), qty2);
  EXPECT_EQ(order->getBuySell(), capk::BID);
  // Check that the other order still exists
  order = ob->getOrder(bid1Id);  
  EXPECT_EQ(order->getPrice(), price1);
  EXPECT_EQ(order->getSize(), qty1);
  EXPECT_EQ(order->getBuySell(), capk::BID);

  EXPECT_EQ(ob->getOrderCountAtLimit(capk::BID, price1), 2);
  EXPECT_EQ(ob->getTotalVolumeAtLimit(capk::BID, price1), 11);
}

TEST_F(InitBookTest, DeleteMutipleOrdersAtPrice) {
  int bid1Id = 1;
  int bid2Id = 2;
  double qty1 = 6;
  double qty2 = 5;
  double price1 = 1.00001;
  double price2 = 1.00001;
  timespec timeStamp;
  clock_gettime(CLOCK_REALTIME, &timeStamp);
  // Add order
  EXPECT_EQ(ob->add(bid1Id, capk::BID, qty1, price1, timeStamp, timeStamp), 1);
  EXPECT_EQ(ob->add(bid2Id, capk::BID, qty2, price2, timeStamp, timeStamp), 1);
  // Delete the first one
  EXPECT_EQ(1, ob->remove(bid1Id, timeStamp, timeStamp));
  EXPECT_EQ(price1, ob->bestPrice(capk::BID));
  capk::pKOrder order = ob->getOrder(bid1Id);  
  EXPECT_EQ(order, capk::pKOrder()); 
  // Check that the second order still exists
  order = ob->getOrder(bid2Id);  
  EXPECT_EQ(order->getPrice(), price2);
  EXPECT_EQ(order->getSize(), qty2);
  EXPECT_EQ(order->getBuySell(), capk::BID);

  EXPECT_EQ(ob->getOrderCountAtLimit(capk::BID, price1), 1);
  EXPECT_EQ(ob->getTotalVolumeAtLimit(capk::BID, price1), 5);
}

TEST_F(InitBookTest, ModifySize) {
  int bid1Id = 21;
  int bid2Id = 43;
  double qty1 = 1000000;
  double qty2 = 5;
  double price1 = 1.00000;
  double price2 = 2.00000;
  timespec timeStamp;
  clock_gettime(CLOCK_REALTIME, &timeStamp);
  // Add orders
  EXPECT_EQ(ob->add(bid1Id, capk::BID, qty1, price1, timeStamp, timeStamp), 1);
  EXPECT_EQ(ob->add(bid2Id, capk::BID, qty2, price2, timeStamp, timeStamp), 1);
  // Check that order is there
  EXPECT_EQ(price2, ob->bestPrice(capk::BID));
  capk::pKOrder order = ob->getOrder(bid2Id);  
  EXPECT_EQ(order->getPrice(), price2);
  EXPECT_EQ(order->getSize(), qty2);
  EXPECT_EQ(order->getBuySell(), capk::BID);
  // modify the order
  ob->modify(bid2Id, 2000000, timeStamp, timeStamp);
  // Check that the other order still exists
  order = ob->getOrder(bid2Id);  
  EXPECT_EQ(order->getPrice(), price2);
  EXPECT_EQ(order->getSize(), 2000000);
  EXPECT_EQ(order->getBuySell(), capk::BID);
}




/*
TEST(BookTests, LaceBook) { 
    timespec evtTime, exchTime;
    //clock_gettime(CLOCK_REALTIME, &entryTime);
    FIX::UtcTimeStamp exchSndTime;
    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.add(1, capk::BID, 1, 1.0000, evtTime, exchTime ), 1);

    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.add(2, capk::BID, 23, 1.0000, evtTime, exchTime ), 1);
        
    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.add(3, capk::ASK, 17, 1.0001, evtTime, exchTime ), 1);

    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.add(4, capk::ASK, 31, 1.045, evtTime, exchTime ), 1);

    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.add(7, capk::BID, 33, 0.99, evtTime, exchTime ), 1);

    clock_gettime(CLOCK_MONOTONIC, &evtTime);
    exchSndTime.setCurrent();
    FIXConvertors::UTCTimeStampToTimespec(exchSndTime, &exchTime);
    EXPECT_EQ(book.add(9, capk::BID, 123, 0.98, evtTime, exchTime ), 1);

    double best_bid = book.bestPrice(capk::BID);
    double best_ask = book.bestPrice(capk::ASK);
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
    double best_bid = book.bestPrice(capk::BID);
    double best_ask = book.bestPrice(capk::ASK);
    std::cerr << "BBO2(" << best_bid << ", " << best_ask << ")\n" ;
    EXPECT_EQ(book.remove(1, evtTime, exchTime), 1);
    std::cout << book;
    EXPECT_EQ(book.remove(3, evtTime, exchTime), 1);
    std::cout << book;
    EXPECT_EQ(book.remove(2, evtTime, exchTime), 1);
    std::cout << book;
    EXPECT_EQ(book.remove(99, evtTime, exchTime), 0);
    std::cout << book;
    EXPECT_EQ(book.add(10, capk::BID, 1, 1.0002, evtTime, exchTime ), 1);
    EXPECT_EQ(book.add(11, capk::BID, 1, 1.0033, evtTime, exchTime ), 1);
    EXPECT_EQ(book.add(12, capk::BID, 1, 1.0003, evtTime, exchTime ), 1);
    EXPECT_EQ(book.add(13, capk::BID, 1, 1.0089, evtTime, exchTime ), 1);
    best_bid = book.bestPrice(capk::BID);
    best_ask = book.bestPrice(capk::ASK);
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
    EXPECT_EQ(book.add(1, capk::BID, 1, 1.0000, evtTime, exchTime ), 1);
    std::cout << book;
    EXPECT_EQ(book.add(2, capk::BID, 23, 1.0000, evtTime, exchTime ), 1);
    std::cout << book;
    EXPECT_EQ(book.add(3, capk::ASK, 17, 1.0001, evtTime, exchTime ), 1);
    std::cout << book;
    best_bid = book.bestPrice(capk::BID);
    best_ask = book.bestPrice(capk::ASK);
    std::cerr << "BBO4(" << best_bid << ", " << best_ask << ")\n";
    EXPECT_EQ(best_bid, 1.0000);
    ASSERT_EQ(best_ask, 1.0001);

}
*/
/*
TEST(BookTests, BBO) {
    double best_bid = book.bestPrice(capk::BID);
    double best_ask = book.bestPrice(capk::ASK);
    ASSERT_LT(best_bid, best_ask);
}

TEST(BookTests, CountFirstLevelBids) { 
    uint32_t bestBids = book.getOrderCountAtLimit(capk::BID, 1.0000); 
    std::cerr << "Found: " << bestBids << " best bids" << std::endl;
    EXPECT_EQ(bestBids, (uint32_t)2); 
} 

TEST(BookTests, FindNonExistentLevelBids) { 
    uint32_t badLevelCount = book.getOrderCountAtLimit(capk::BID, 100); 
    EXPECT_EQ(badLevelCount, (uint32_t)0); 
} 

TEST(BookTests, GetTotalVolume2) { 
    double volumeAtLevel;
    volumeAtLevel = book.getTotalVolumeAtLimit(capk::BID, 1.000); 
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

    volumeAtLevel = book.getTotalVolumeAtLimit(capk::BID, 1.000); 
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

    volumeAtLevel = book.getTotalVolumeAtLimit(capk::BID, 1.000); 
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
    volumeAtLevel = book.getTotalVolumeAtLimit(capk::BID, 0.98); 
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
    volumeAtLevel = book.getTotalVolumeAtLimit(capk::BID, 1.000); 
    EXPECT_EQ(volumeAtLevel, 0);
}
*/
/*
TEST(BookTests, GetTotalVolume4) { 
    double volumeAtLevel;
    volumeAtLevel = book.getTotalVolumeAtLimit(capk::ASK, 1.0001); 
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
/*
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
        std::cout << "Book: " << i << "\n"; 
        std::cout << book;
        std::cout << "---------------------------------------------------------\n"; 
    }
    clock_gettime(CLOCK_REALTIME, &time_end);
    timespec d = capk::timespec_delta(time_end, time_start);

    printf("Summary\n--------------------------------------------------\n"); 
    std::cout << "Elapsed time: " << d<< std::endl;
    printf("Total adds %d; successful adds %d\n", countAdd, addSuccess);
    printf("Total modify %d; successful modify %d\n", countModify, modifySuccess);
    printf("Total remove %d; successful remove %d\n", countRemove, removeSuccess);
*/
    return result; 
}

