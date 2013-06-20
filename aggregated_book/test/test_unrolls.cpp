

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <gtest/gtest.h>

#define STRCMP8(dst, src) \
  (dst[0] == src[0] && \
  dst[1] == src[1] && \
  dst[2] == src[2] && \
  dst[3] == src[3] && \
  dst[4] == src[4] && \
  dst[5] == src[5] && \
  dst[6] == src[6] && \
  dst[7] == src[7]) 

TEST(Unroll, strcmp8) {
  const char* s1 = "ABC/DEF";
  const char* s2 = "DEF/GHI";
  const char* s3 = "ABC/DEF";

  int cmp1 = STRCMP8(s1, s2);
  EXPECT_EQ(cmp1, 0);
  int cmp2 = STRCMP8(s1, s3);
  EXPECT_EQ(cmp2, 1);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
   /*
  boost::property_tree::ptree pt;
  boost::property_tree::ini_parser::read_ini("config.ini", pt);
  std::cout << pt.get<std::string>("Section1.Value1") << std::endl;
  std::cout << pt.get<std::string>("Section1.Value2") << std::endl;
  */
  return result;
}

