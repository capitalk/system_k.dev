#include "MsgDispatcher.h"

MsgDispatcher::MsgDispatcher(MsgPump* pMsgPump, SessionInfo* pSessionInfo/*std::map<const FIX::SessionID, SessionInfo*>& sessionInfo,*/  /*std::set<FIX::SessionID>& sessions*/): _pMsgPump(pMsgPump), _pSessionInfo(pSessionInfo), /*_sessions(sessions),*/ _bInit(false), _speedFactor(1)
{
}

MsgDispatcher::~MsgDispatcher()
{
    boost::mutex::scoped_lock lock(_timeBufMutex);
    delete[] _timeBuf;
    _timeBuf = NULL;
}

void
MsgDispatcher::init()
{
    boost::mutex::scoped_lock lock(_timeBufMutex);
    _timeBuf = new char[(22+7+1)*1000];
    if (!_timeBuf) {
        //std::exception e("Out of memory allocating time buffer");
        //throw(e);
    }
    memset(_timeBuf, 0, sizeof(_timeBuf));
    _bInit = true;
}

void
MsgDispatcher::join()
{
    _thread->join();
}

bool
MsgDispatcher::getTimeDelta(FIX::Message& m1, FIX::Message& m2, long* tDelta) 
{
    boost::mutex::scoped_lock lock(_timeBufMutex);
    if (m1.isEmpty() || m2.isEmpty()) {
        if (m1.isEmpty()) {
            std::cerr << "m1 is empty" << std::endl;
        }
        if (m2.isEmpty()) {
            std::cerr << "m2 is empty" << std::endl;
        }
        return false;
    }
    
    if (!m1.getHeader().isSetField(FIX::FIELD::SendingTime) || !m2.getHeader().isSetField(FIX::FIELD::SendingTime)) {
#ifdef DEBUG
        if (!m1.getHeader().isSetField(FIX::FIELD::SendingTime)) {
            std::cerr << "m1 SendingTime is empty" << std::endl;
        }
        if (!m2.getHeader().isSetField(FIX::FIELD::SendingTime)) {
            std::cerr << "m2 SendingTime is empty" << std::endl;
        }
#endif // DEBUG
        return false;
    }
    FIX::SendingTime t1;
    FIX::SendingTime t2;

    m1.getHeader().getField(t1);
    m2.getHeader().getField(t2);

    ptime pt1;
    ptime pt2;
    FIX::UtcTimeStamp utc1(t1);
    FIX::UtcTimeStamp utc2(t2);

    char b[30];
    bool bConvert;
    bConvert = FIXConvertors::UTCTimeStampToPTime(utc1, &b[0], 30, pt1);
    if (!bConvert) {
        std::cerr << "Time conversion failed" << std::endl;
    } 
    bConvert = FIXConvertors::UTCTimeStampToPTime(utc2, &b[0], 30, pt2);
    if (!bConvert) {
        std::cerr << "Time conversion failed" << std::endl;
    } 

    time_duration duration = pt2 - pt1;
    //std::cerr << "T1: " << pt1 << " T2: " << pt2 << " DELTA: " << duration << std::endl;
    
    (*tDelta) = duration.total_microseconds();
    return true;
     
}

void 
MsgDispatcher::run()
{
    FIX::Message m1;
    FIX::Message m2;
    FIX::MsgType msgType;
    FIX::SendingTime sendingTime;
    std::string raw1;
    std::string raw2;
    long delta = 0;
    while(!_stopRequested) {
        if (_pMsgPump->getMessage(m1, raw1) == false) {
            std::cerr << "getMessage m1 returned false\n";
            break;
        }        
        if (_pMsgPump->getMessage(m2, raw2) == false) {
            std::cerr << "getMessage m2 returned false\n";
            break;
        }
#if DEBUG 
        std::cout << "****M1: " << m1.toString() << std::endl; 
        std::cout << "****M2: " << m2.toString() << std::endl; 
#endif // DEBUG
        if (!getTimeDelta(m1, m2, &delta)) {
            std::cerr << "Error fetching timestamp from msg" << std::endl;
        }
/*
        else { 
            std::cerr << "DELTA: " << delta << std::endl;
        }
*/
        delta /= _speedFactor;
        //std::cerr << "Sleep micros: " << delta << " micros" << "(speedFactor = " << _speedFactor << ")" << std::endl;
        dispatch(m1);
        boost::this_thread::sleep(boost::posix_time::microseconds(delta));
        dispatch(m2);
    }
    if (!m1.isEmpty()) {
        dispatch(m1); 
    }
    if (!m2.isEmpty()) {
        dispatch(m2); 
    }
}

void 
MsgDispatcher::dispatch(FIX::Message& msg)
{
    FIX::MsgType msgType;
    FIX::BeginString beginString;
    msg.getHeader().getField(msgType);
    msg.getHeader().getField(beginString);

    FIX::SessionID sessionID;
    _pSessionInfo->getSessionID(sessionID);

    FIX::TargetCompID targetCompID = sessionID.getTargetCompID();
    FIX::SenderCompID senderCompID = sessionID.getSenderCompID(); 
    std::string s;
    msg.toString(s);

    FIX::Symbol msgSymbol;
    if (msg.isSetField(FIX::FIELD::Symbol)) {
        msg.getField(msgSymbol);
    }
    // Send only messages for which client has subscribed
    //if (_pSessionInfo->hasSymbol(msgSymbol)) {
        if (msgType == FIX::MsgType_MarketDataSnapshotFullRefresh) {
            if (beginString == FIX::BeginString_FIX42) {
                FIX42::MarketDataSnapshotFullRefresh cm(msg);
                FIX::Session::sendToTarget(cm, senderCompID, targetCompID);
            }
        }
        if (msgType == FIX::MsgType_MarketDataIncrementalRefresh) {
            FIX::Session::sendToTarget(msg, senderCompID, targetCompID);
        }
    //}
/*
    if (msgType == FIX::MsgType_Logout) {
        FIX::Session::sendToTarget(msg, senderCompID, targetCompID);
        //FIX::Session::sendToTarget(msg, sessionID); 
    }
*/
    
}

void
MsgDispatcher::start()
{
    if (!_bInit) {
        init();
    }
    if (_isStarted == true || _stopRequested == true) {
        return;
    }
    _thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&MsgDispatcher::run, this)));
    _isStarted = true;
}

void 
MsgDispatcher::stop()
{
    _isStarted = false;
    _stopRequested = true;
    _thread->join();
}


