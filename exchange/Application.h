/* -*- C++ -*- */

/****************************************************************************
** Copyright (c) quickfixengine.org  All rights reserved.
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifndef EXECUTOR_APPLICATION_H
#define EXECUTOR_APPLICATION_H

//#include "MP.h"
#include "SessionInfo.h"

#include <unordered_set>

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Values.h"
#include "quickfix/Utility.h"
#include "quickfix/Mutex.h"

#include "quickfix/fix40/NewOrderSingle.h"
#include "quickfix/fix41/NewOrderSingle.h"
#include "quickfix/fix42/NewOrderSingle.h"
#include "quickfix/fix43/NewOrderSingle.h"
#include "quickfix/fix44/NewOrderSingle.h"
#include "quickfix/fix50/NewOrderSingle.h"


/*
#include <google/dense_hash_map>
using google::dense_hash_map;
using tr1::hash;
struct eqSession
{
    bool operator() (const FIX::SessionID& s1, const FIX::SessionID& s2) const {
        return (s1.getBeginString() == s2.getBeginString() &&
                    s1.getSenderCompID() == s2.getSenderCompID() &&
                    s1.getTargetCompID() == s2.getTargetCompID());
    }
};
*/

enum FIXVersion {
    FIX_40 = 40,
    FIX_41 = 41,
    FIX_42 = 42,
    FIX_43 = 43,
    FIX_44 = 44,
    FIX_50 = 45
};

struct ApplicationConfig {
    std::string mic_code;
    FIXVersion version;
    bool printDebug;
};



class Application : 
            public FIX::Application, 
            public FIX::MessageCracker
{
public:
    Application(const ApplicationConfig& config) : m_orderID(0), m_execID(0), _config(config), _isReplay(false) { }

    // Application overloads
    void onCreate(const FIX::SessionID&);
    void onLogon(const FIX::SessionID& sessionID);
    void onLogout(const FIX::SessionID& sessionID);
    void toAdmin(FIX::Message&, const FIX::SessionID&);
    void toApp(FIX::Message&, const FIX::SessionID&)
        throw(FIX::DoNotSend);
    void fromAdmin( const FIX::Message&, const FIX::SessionID&)
        throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);
    void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)
        throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

    // MessageCracker overloads
    void onMessage(const FIX40::NewOrderSingle&, const FIX::SessionID&);
    void onMessage(const FIX41::NewOrderSingle&, const FIX::SessionID&);
    void onMessage(const FIX44::NewOrderSingle&, const FIX::SessionID&);
    void onMessage(const FIX50::NewOrderSingle&, const FIX::SessionID&);
   
    // Message cracker overloads capk server
    // FIX42 Messages
    void onMessage(const FIX42::SecurityStatusRequest& message, const FIX::SessionID& sessionID);
    void onMessage(const FIX42::TradingSessionStatusRequest& message, const FIX::SessionID& sessionID);
    //void onMessage(const FIX42::SecurityDefinition& message, const FIX::SessionID& sessionID);
    void onMessage(const FIX42::TestRequest& message, const FIX::SessionID& sessionID);
    void onMessage(const FIX42::Heartbeat& message, const FIX::SessionID& sessionID);
    void onMessage(const FIX42::Logout& message, const FIX::SessionID& sessionID);
    void onMessage(const FIX42::Logon& message, const FIX::SessionID& sessionID);
    // FIX42 Market Data
    virtual void onMessage(const FIX42::MarketDataRequest& message, const FIX::SessionID& sessionID);
    //void onMessage(const FIX42::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID);
    //void onMessage(const FIX42::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID);
    //void onMessage(const FIX42::MarketDataRequestReject& message, const FIX::SessionID& sessionID);
    // FIX42 Order Entry
    void onMessage( const FIX42::NewOrderSingle&, const FIX::SessionID& );

    // FIX43 Messages
    void onMessage(const FIX43::SecurityStatusRequest& message, const FIX::SessionID& sessionID);
    void onMessage(const FIX43::TradingSessionStatusRequest& message, const FIX::SessionID& sessionID);
    //void onMessage(const FIX43::SecurityDefinition& message, const FIX::SessionID& sessionID);
    void onMessage(const FIX43::TestRequest& message, const FIX::SessionID& sessionID);
    void onMessage(const FIX43::Heartbeat& message, const FIX::SessionID& sessionID);
    void onMessage(const FIX43::Logout& message, const FIX::SessionID& sessionID);
    void onMessage(const FIX43::Logon& message, const FIX::SessionID& sessionID);
    // FIX43 Market data
    void onMessage(const FIX43::MarketDataRequest& message, const FIX::SessionID& sessionID);
    //void onMessage(const FIX43::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID);
    //void onMessage(const FIX43::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID);
    //void onMessage(const FIX43::MarketDataRequestReject& message, const FIX::SessionID& sessionID);
    // FIX43 Order entry
    void onMessage( const FIX43::NewOrderSingle&, const FIX::SessionID& );

    std::string genOrderID() {
        std::stringstream stream;
        stream << ++m_orderID;
        return stream.str();
    }

    std::string genExecID() {
        std::stringstream stream;
        stream << ++m_execID;
        return stream.str();
    }


    void setReplayLog(const std::string& log);
    void setReplayVolatility(const double vol);
    void setReplaySpeed(const double sp);

private:
    int m_orderID, m_execID;
    const ApplicationConfig& _config;
    std::string _replayLog;
    double _replayVolatility;
    double _replayTimeDiv;
    std::ifstream _inLog;
    MP _msgPump;
    //std::unordered_map<FIX::SessionID, SessionInfo*> _sessionInfo;
    typedef std::map<const FIX::SessionID, SessionInfo*> SessionMap;
    SessionMap _sessionMap;
    //google::dense_hash_map<const FIX::SessionID, SessionInfo*> _test;
    bool _isReplay;
};

#endif
