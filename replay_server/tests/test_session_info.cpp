
#include "gtest/gtest.h"
#include "session_info.h"
#include <iostream>
#include <string>
#include <thread>

#include <quickfix/Session.h>

using namespace std;

SessionInfo* pSI;
std::string symbol1;
std::string symbol2;
const std::string usdjpy = "USD/JPY";


TEST(SessionInfo, Empty)
{
    symbol1 = "EUR/USD";
    ASSERT_FALSE(pSI->hasSymbol(symbol1));
}

TEST(SessionInfo, InsertNew)
{
    symbol1 = "EUR/USD";
    EXPECT_TRUE(pSI->addSymbol(symbol1));
    ASSERT_TRUE(pSI->hasSymbol(symbol1));
}

TEST(SessionInfo, InsertDup)
{
    symbol2 = "EUR/USD";
    EXPECT_FALSE(pSI->addSymbol(symbol2));
    ASSERT_TRUE(pSI->hasSymbol(symbol2));
}

TEST(SessionInfo, InsertSecond)
{ 
    symbol2 = "USD/JPY";
    EXPECT_TRUE(pSI->addSymbol(symbol2));
    ASSERT_TRUE(pSI->hasSymbol(symbol2));
}

TEST(SessionInfo, CheckSize)
{
    EXPECT_EQ(pSI->size(), (size_t)2);     
}

TEST(SessionInfo, RemoveOne)
{
    symbol1 = "EUR/USD";
    pSI->removeSymbol(symbol1);
    EXPECT_EQ(pSI->size(), (size_t)1);
    ASSERT_FALSE(pSI->hasSymbol(symbol1));
}

TEST(SessionInfo, RemoveTwo)
{
    symbol1 = "USD/JPY";
    pSI->removeSymbol(symbol1);
    EXPECT_EQ(pSI->size(), (size_t)0);
    ASSERT_FALSE(pSI->hasSymbol(symbol1));
    ASSERT_FALSE(pSI->hasSymbol(symbol2));
}

TEST(SessionInfo, RemoveEmpty)
{
    symbol1 = "USD/AUD";
    pSI->removeSymbol(symbol1);
    EXPECT_EQ(pSI->size(), (size_t)0);
    ASSERT_FALSE(pSI->hasSymbol(symbol1));
}

int 
main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    pSI = new SessionInfo(FIX::SessionID("S1", "S2", "S3", "S4"));
    return RUN_ALL_TESTS();  
}
