#ifndef _CAPK_MSGDISPATCHER_
#define _CAPK_MSGDISPATCHER_

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <exception>

#include <boost/tokenizer.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/condition.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "quickfix/Message.h"
#include "quickfix/Parser.h"
#include "quickfix/Values.h"
#include "quickfix/FieldConvertors.h"
#include "quickfix/FieldMap.h"
#include "quickfix/Session.h"

#include "quickfix/fix42/MarketDataIncrementalRefresh.h"
#include "quickfix/fix42/MarketDataRequest.h"
#include "quickfix/fix42/MarketDataRequestReject.h"
#include "quickfix/fix42/MarketDataSnapshotFullRefresh.h"


#include <time.h>

#include "SessionInfo.h"
#include "FIXConvertors.h"
#include "MsgPump.h"

class SessionInfo;

class MsgDispatcher
{
    public:
        MsgDispatcher(MsgPump* pMsgPump, SessionInfo* pSessionInfo/*std::map<const FIX::SessionID, SessionInfo*>& sessionInfo,*/  /*std::set<FIX::SessionID>& sessions*/);
        ~MsgDispatcher(); 
    
        inline void setSpeedFactor(float speedFactor) { this->_speedFactor = speedFactor; }
        void init();
        void run();
        void start();
        void stop();

        void join();
        void dispatch(FIX::Message& msg);//, const FIX::SessionID& sessionID);


    private:
        bool getTimeDelta(FIX::Message& m1, FIX::Message& m2, long* tDelta);
        volatile bool _isStarted;
        volatile bool _stopRequested;
        boost::shared_ptr<boost::thread> _thread;
        boost::mutex _consumedMutex;
        boost::mutex _initMutex;
        boost::mutex _timeBufMutex;
        boost::condition _msgConsumed;

        MsgPump* _pMsgPump;
        //std::map<const FIX::SessionID, SessionInfo*>& _sessionIDToInfo;
        SessionInfo* _pSessionInfo;
        //std::set<FIX::SessionID>& _sessions;
        char* _timeBuf;
        bool _bInit;
        float _speedFactor;
};

#endif //_CAPK_MSGDISPATCHER_
