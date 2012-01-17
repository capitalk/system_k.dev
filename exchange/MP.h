#ifndef _CAPK_MP
#define _CAPK_MP

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

typedef enum MSG_RET {
    MSG_OK = 0,
    MSG_NOT_READY = 1,
    MSG_EOF = 2,
    MSG_ERROR = 3
} MSG_RET;

class MP
{
    public:
        MP();
        MP(std::string inFile);
        ~MP(); 
        bool open();
        void run();
        void start();
        void stop();

        void join();

        bool setInputFile(std::string inFile);
        void getMsg(std::string& raw, MSG_RET* ret);

    private:
        void read();

        std::string _inFile;
        volatile bool _isStarted;
        volatile bool _stopRequested;
        MSG_RET _msgCode;
        volatile bool _read;
        volatile bool _consumed;
        std::string _strMsg;
    
        std::ifstream _in;
        boost::shared_ptr<boost::thread> _thread;
        boost::mutex _mutex;
        boost::condition _msgConsumed;
        boost::condition _msgRead;
        boost::condition _c;
        boost::condition _stop;
        FILE* fp;
};

#endif // _CAPK_MP
