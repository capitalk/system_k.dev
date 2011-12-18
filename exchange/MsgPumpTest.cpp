
#include "gtest.h"
#include "MP.h"
#include <iostream>
#include <string>
#include <thread>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

MP* pMP;
std::string replayFile;

TEST(MP, OpenFile)
{
    pMP = new MP(replayFile);
    EXPECT_TRUE(pMP->open()) << "open() called on file that is already open";
    EXPECT_FALSE(pMP->open()) << "open() called twice and succeeded second time - file should already be open";
}

int 
main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);

    po::options_description desc("Allowed options");
    desc.add_options()
    ("i", po::value<std::string>(), "input file (FIX message log)"  );
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm); 

    if (vm.count("i") != 1) {
        std::cout << desc << "\n";
        return 1;
    }

    if (vm.count("i")) {
        replayFile = vm["i"].as<std::string>();
    }
    pMP = new MP(replayFile);
    pMP->open();
    pMP->start();
    FIX::Message m1, m2;
    std::string s1, s2;
    
/*
    FIX::Message m1, m2, m3, m4;
        //m2.clear();
        //std::cout << "An empty message: " << m2.toString() << std::endl;

    pMsgPump->getMessage(m1, s1);
        std::cout << "1: " << m1.toString() << std::endl;
    sleep(1);
    pMsgPump->getMessage(m1, s1);

        std::cout << "2: " << m1.toString() << std::endl;
    sleep(1);
    pMsgPump->getMessage(m1, s1);
        std::cout << "3: " << m1.toString() << std::endl;
    sleep(1);
    pMsgPump->getMessage(m2, s2);
        std::cout << "4: " << m1.toString() << std::endl;
    sleep(1);
*/   
    FILE* fp = 0;
    fp = fopen("./OUT", "wb+");
    assert(fp);

    MSG_RET gotMsg1 = MSG_ERROR;
    MSG_RET gotMsg2 = MSG_ERROR;

    while (1) { 
        pMP->getMsg(s1, &gotMsg1);
        if (gotMsg1 == MSG_EOF) {printf("M1 BREAK"); break;}
            printf("M1: %s\n", s1.c_str()); 
            fprintf(fp, "%s\n", s1.c_str()); 

        pMP->getMsg(s2, &gotMsg2);
        if (gotMsg2 == MSG_EOF) {printf("M2 BREAK"); break;}
            fprintf(fp, "%s\n", s2.c_str()); 
            printf("M2: %s\n", s2.c_str()); 
        
/*
        if (m1.getHeader().isSetField(FIX::FIELD::SendingTime)) {
            FIX::SendingTime sendingTime;
            std::cout << m1.getHeader().getField(sendingTime);
        }
        else {
            std::cout << "*** SendingTime not set\n" ;
        }

        if (m1.getHeader().isSetField(FIX::FIELD::MsgType)) {
            FIX::MsgType mt;
            std::cout << m1.getHeader().getField(mt);
        }
        else {
            std::cout << "*** MsgType not set\n" ;
        }

        if (m2.getHeader().isSetField(FIX::FIELD::SendingTime)) {
            FIX::SendingTime sendingTime;
            std::cout << m1.getHeader().getField(sendingTime);
        }
        else {
            std::cout << "*** SendingTime not set\n" ;
        }

        if (m2.getHeader().isSetField(FIX::FIELD::MsgType)) {
            FIX::MsgType mt;
            std::cout << m1.getHeader().getField(mt);
        }
        else {
            std::cout << "*** MsgType not set\n" ;
        }
*/
        //FIX::MsgType msgType;
        //std::cout << i++ << ":";
        //if (m.getHeader().isSetField(FIX::FIELD::MsgType)) {
            //m.getHeader().getField(msgType);
            //std::cout << msgType;
        //}
        //std::cout << i++ << ": "  << m1.toString() << std::endl;
        //std::cout << i++ << ": "  << m2.toString() << std::endl;
        //std::cout << std::endl;

        //std::string sm;
        //m.toString(sm);
    } 
    fflush(fp);
    fclose(fp);
    std::cerr << "about to join msg pump thread ... \n";
    pMP->join(); 
/*
    for (int i = 0; i < 100000; i++) {
        std::cerr << "=================> " << i << std::endl;
    }
*/
    //pMsgPump->stop();

    return RUN_ALL_TESTS();  
}
