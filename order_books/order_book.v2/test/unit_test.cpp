
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

int 
main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int result =  RUN_ALL_TESTS();
    return result;
}

