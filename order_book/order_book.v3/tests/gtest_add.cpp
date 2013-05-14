
#include <gtest/gtest.h>

#include "utils/constants.h"
#include "order_book.hxx"

class BookTest : public ::testing::Test {
 public:
  BookTest();

  ~BookTest();

  virtual void SetUp() {
    ob = new order_book(1.00, 0.05, 0.00001);
  }
  virtual void TearDown() {
    delete ob;
  }

  capk:order_book* ob;
}

TEST_F(BookTest, IsInitiallyEmpty) {
  EXPECT_EQUAL(ob.getBid(1.00), 0);
}

TEST_F(BookTest, OutOfBounds) {
  EXPECT_EQUAL(ob.getBid(0.00), capk::NO_BID);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  int result =  RUN_ALL_TESTS();
  return result;
}
