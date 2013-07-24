
#include <gtest/gtest.h>

#include "utils/constants.h"
#include "order_book.hxx"

#include <stdexcept>


class BookTest : public ::testing::Test {
 public:
  BookTest() {};

  ~BookTest() {};

  virtual void SetUp() {
    ob = new capk::order_book("EUR/USD", 1.00, 100, 0.05, 0.01);
  }
  virtual void TearDown() {
    delete ob;
  }

  capk::order_book* ob;
};

TEST(ValidityTest, TestValidInitPrice) {
  capk::order_book* ob;
  ob = new capk::order_book("EUR/USD", 1.00, 100, 0.05, 0.01);
  EXPECT_EQ(0, validate_init_price(ob->getBook())) << "Initial price is multiple of tick size";
}

TEST(ValidityTest, TestInvalidInitPrice) {
  capk::order_book* ob;
  ob = new capk::order_book("EUR/USD", 1.01, 100, 0.05, 0.02);
  EXPECT_NE(0, validate_init_price(ob->getBook())) << "Initial price NOT multiple of tick size";
}

TEST(ValidityTest, TestInvalidMultiplier) {
  capk::order_book* ob = NULL;
  try {
    ob = new capk::order_book("EUR/USD", 1.00, 10, 0.05, 0.01) ;
  }
  catch (std::runtime_error& e) {
    std::cerr << "CAUGHT" ;  
  }
}

TEST_F(BookTest, IsInitiallyEmpty) {
  //EXPECT_EQ(ob.getBid(1.00), 0);
}

TEST_F(BookTest, OutOfBounds) {
  //EXPECT_EQ(ob.getBid(0.00), capk::NO_BID);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  int result =  RUN_ALL_TESTS();
  return result;
}
