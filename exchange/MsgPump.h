#ifndef _CAPK_MSGPUMP_
#define _CAPK_MSGPUMP_

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <boost/tokenizer.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/condition.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

#include "quickfix/Message.h"
#include "quickfix/Parser.h"
#include "quickfix/Values.h"
#include "quickfix/FieldConvertors.h"
#include "quickfix/FieldMap.h"


using namespace std;

class MsgPump
{
    public:
        MsgPump();
        MsgPump(std::string inFile);
        ~MsgPump(); 
        bool open();
        void run();
        void start();
        void stop();
        bool read();

        void join();
        //void wait();
        //void notify();

        bool setInputFile(std::string inFile);
        bool getMessage(FIX::Message& m, std::string& raw);
        void setDataDictionary(const FIX::DataDictionary& dd);

    private:
        uint64_t _msgCount;
        FIX::Message _msg;
        std::string _strMsg;
        FIX::Parser _parser;
    
        std::ifstream _in;
        std::string _inFile;
        volatile bool _isStarted;
        volatile bool _stopRequested;
        boost::shared_ptr<boost::thread> _thread;
        boost::mutex _consumedMutex;
        boost::mutex _readMutex;
        boost::condition _msgConsumed;
        boost::condition _msgRead;
        bool _bEOF;
        static int32_t _dbgLine;
        FIX::DataDictionary _dict;
};

#endif // _CAPK_MSGPUMP_
