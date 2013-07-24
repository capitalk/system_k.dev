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

#ifdef _MSC_VER
#pragma warning(disable : 4503 4355 4786)
#else
//#include "config.h"
#endif

#include "Application.h"
#include "fix_dictionary_extensions.h"
#include "quickfix/Session.h"

#include "quickfix/fix40/ExecutionReport.h"
#include "quickfix/fix41/ExecutionReport.h"
#include "quickfix/fix44/ExecutionReport.h"
#include "quickfix/fix50/ExecutionReport.h"

#include "quickfix/fix42/ExecutionReport.h"
#include "quickfix/fix42/MarketDataIncrementalRefresh.h"
#include "quickfix/fix42/MarketDataSnapshotFullRefresh.h"
//#include "quickfix/fix42/MarketDataRequestReject.h"
#include "quickfix/fix42/MarketDataRequest.h"
#include "quickfix/fix42/Logout.h"
#include "quickfix/fix42/Logon.h"
#include "quickfix/fix42/Heartbeat.h"
#include "quickfix/fix42/SequenceReset.h"
#include "quickfix/fix42/TestRequest.h"
#include "quickfix/fix42/ResendRequest.h"
#include "quickfix/fix42/TradingSessionStatus.h"
#include "quickfix/fix42/SecurityStatusRequest.h"
#include "quickfix/fix42/SecurityDefinition.h"
#include "quickfix/fix42/ExecutionReport.h"

#include "quickfix/fix43/MarketDataIncrementalRefresh.h"
#include "quickfix/fix43/MarketDataSnapshotFullRefresh.h"
//#include "quickfix/fix43/MarketDataRequestReject.h"
#include "quickfix/fix43/MarketDataRequest.h"
#include "quickfix/fix43/Logout.h"
#include "quickfix/fix43/Logon.h"
#include "quickfix/fix43/Heartbeat.h"
#include "quickfix/fix43/SequenceReset.h"
#include "quickfix/fix43/TestRequest.h"
#include "quickfix/fix43/ResendRequest.h"
#include "quickfix/fix43/TradingSessionStatus.h"
#include "quickfix/fix43/SecurityStatusRequest.h"
#include "quickfix/fix43/SecurityDefinition.h"
#include "quickfix/fix43/ExecutionReport.h"

#include <boost/algorithm/string.hpp>

#define IF_REPLAY(x, y) if (_isReplay) { x } else { y }
#define IF_DEBUG(x) if (_config.printDebug) { x }


void 
Application::setReplayVolatility(const double vol)
{
    _replayVolatility = vol;
}

void 
Application::setReplaySpeed(const double sp)
{
    _replayTimeDiv = sp;
}

void
Application::setReplayLog(const std::string& log) 
{
    IF_DEBUG(std::cerr << "Set replay log: " << log << std::endl;)
    try {
        if (_config.printDebug) {
            std::cerr << "Reading from: " << log << std::endl;
        }
        _replayLog = log;
        _isReplay = true;
/*
        _msgPump.setInputFile(log);
        if(!_msgPump.open()) {
            std::cerr << "Error opening replay log" << std::endl;
        }
        IF_DEBUG(std::cout << "isReplay: " << _isReplay << std::endl;)
        
        _msgPump.start();
*/
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}



void Application::onCreate(const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::onCreate" << std::endl;
    }
}

void Application::onLogon(const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
        std::cout << "Application::onLogon" << std::endl;
    }
    _sessionMap[sessionID] = new SessionInfo(sessionID);
    
    if (_config.printDebug) {
        std::cerr << "SessionInfo created for session :" << sessionID.toString();
    }
}

void Application::onLogout(const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::onLogout" << std::endl;
    }
    std::map<const FIX::SessionID, SessionInfo*>::iterator it;
    std::cerr << "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP\n";
    it = _sessionMap.find(sessionID);
    if (it == _sessionMap.end()) {
        if (_config.printDebug) {
            std::cerr << "SessionInfo not found for session :" << sessionID.toString();
        }
    }
    else {
        if (_config.printDebug) {
            std::cerr << "SessionInfo deleting info for:" << sessionID.toString();
        }
        
        MsgDispatcher *md = it->second->getDispatcher();
        assert(md);
        MP* mp = md->getPump();
        assert(mp);
        mp->stop();
        md->stop();


        delete (it->second);
        _sessionMap.erase(it);
    }
}

void Application::toAdmin(FIX::Message& message,
                         const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
        std::cerr << "Application::toAdmin" << std::endl;
    }
}

void Application::toApp(FIX::Message& message,
                         const FIX::SessionID& sessionID)
throw(FIX::DoNotSend)
{
    if (_config.printDebug) {
        std::cerr << "Application::toApp" << message << sessionID << std::endl;
    }
}

void Application::fromAdmin(const FIX::Message& message,
                             const FIX::SessionID& sessionID)
throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) 
{
    if (_config.printDebug) {
        std::cerr << "Application::fromAdmin" << std::endl;
    }
}

void Application::fromApp(const FIX::Message& message,
                           const FIX::SessionID& sessionID)
throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{ 
    if (_config.printDebug) {
        std::cerr << "Application::fromApp" << std::endl;
    }
    FIX::MsgType msgType;
    message.getHeader().getField(msgType);
    //std::cerr << "ABOUT TO CRACK------------------------>" << std::endl;
    /*
     * Use this special message type to initiate book replay when ready to process
     */
    //if (msgType == "CAPKREPLAY") {
        
    //}
    //else {
        crack(message, sessionID); 
    //}
}

void Application::onMessage(const FIX40::NewOrderSingle& message,
                             const FIX::SessionID& sessionID)
{


  FIX::Symbol symbol;
  FIX::Side side;
  FIX::OrdType ordType;
  FIX::OrderQty orderQty;
  FIX::Price price;
  FIX::ClOrdID clOrdID;
  FIX::Account account;

  message.get(ordType);

  if (ordType != FIX::OrdType_LIMIT)
    throw FIX::IncorrectTagValue(ordType.getField());

  message.get(symbol);
  message.get(side);
  message.get(orderQty);
  message.get(price);
  message.get(clOrdID);

  FIX40::ExecutionReport executionReport = FIX40::ExecutionReport
      (FIX::OrderID(genOrderID()),
        FIX::ExecID(genExecID()),
        FIX::ExecTransType(FIX::ExecTransType_NEW),
        FIX::OrdStatus(FIX::OrdStatus_FILLED),
        symbol,
        side,
        orderQty,
        FIX::LastShares(orderQty),
        FIX::LastPx(price),
        FIX::CumQty(orderQty),
        FIX::AvgPx(price));

  executionReport.set(clOrdID);

  if(message.isSet(account))
    executionReport.setField(message.get(account));

  try
  {
    FIX::Session::sendToTarget(executionReport, sessionID);
  }
  catch (FIX::SessionNotFound&) {}
}

void Application::onMessage(const FIX41::NewOrderSingle& message,
                             const FIX::SessionID& sessionID)
{
  FIX::Symbol symbol;
  FIX::Side side;
  FIX::OrdType ordType;
  FIX::OrderQty orderQty;
  FIX::Price price;
  FIX::ClOrdID clOrdID;
  FIX::Account account;

  message.get(ordType);

  if (ordType != FIX::OrdType_LIMIT)
    throw FIX::IncorrectTagValue(ordType.getField());

  message.get(symbol);
  message.get(side);
  message.get(orderQty);
  message.get(price);
  message.get(clOrdID);

  FIX41::ExecutionReport executionReport = FIX41::ExecutionReport
      (FIX::OrderID(genOrderID()),
        FIX::ExecID(genExecID()),
        FIX::ExecTransType(FIX::ExecTransType_NEW),
        FIX::ExecType(FIX::ExecType_FILL),
        FIX::OrdStatus(FIX::OrdStatus_FILLED),
        symbol,
        side,
        orderQty,
        FIX::LastShares(orderQty),
        FIX::LastPx(price),
        FIX::LeavesQty(0),
        FIX::CumQty(orderQty),
        FIX::AvgPx(price));

  executionReport.set(clOrdID);

  if(message.isSet(account))
    executionReport.setField(message.get(account));

  try
  {
    FIX::Session::sendToTarget(executionReport, sessionID);
  }
  catch (FIX::SessionNotFound&) {}
}

void Application::onMessage(const FIX42::NewOrderSingle& message,
                             const FIX::SessionID& sessionID)
{
  FIX::Symbol symbol;
  FIX::Side side;
  FIX::OrdType ordType;
  FIX::OrderQty orderQty;
  FIX::Price price;
  FIX::ClOrdID clOrdID;
  FIX::Account account;

  message.get(ordType);

  if (ordType != FIX::OrdType_LIMIT)
    throw FIX::IncorrectTagValue(ordType.getField());

  message.get(symbol);
  message.get(side);
  message.get(orderQty);
  message.get(price);
  message.get(clOrdID);

  FIX42::ExecutionReport executionReport = FIX42::ExecutionReport
      (FIX::OrderID(genOrderID()),
        FIX::ExecID(genExecID()),
        FIX::ExecTransType(FIX::ExecTransType_NEW),
        FIX::ExecType(FIX::ExecType_FILL),
        FIX::OrdStatus(FIX::OrdStatus_FILLED),
        symbol,
        side,
        FIX::LeavesQty(0),
        FIX::CumQty(orderQty),
        FIX::AvgPx(price));

  executionReport.set(clOrdID);
  executionReport.set(orderQty);
  executionReport.set(FIX::LastShares(orderQty));
  executionReport.set(FIX::LastPx(price));

  if(message.isSet(account))
    executionReport.setField(message.get(account));

  try
  {
    FIX::Session::sendToTarget(executionReport, sessionID);
  }
  catch (FIX::SessionNotFound&) {}
}

void Application::onMessage(const FIX43::NewOrderSingle& message,
                             const FIX::SessionID& sessionID)
{
  FIX::Symbol symbol;
  FIX::Side side;
  FIX::OrdType ordType;
  FIX::OrderQty orderQty;
  FIX::Price price;
  FIX::ClOrdID clOrdID;
  FIX::Account account;

  message.get(ordType);

  if (ordType != FIX::OrdType_LIMIT)
    throw FIX::IncorrectTagValue(ordType.getField());

  message.get(symbol);
  message.get(side);
  message.get(orderQty);
  message.get(price);
  message.get(clOrdID);

  FIX43::ExecutionReport executionReport = FIX43::ExecutionReport
      (FIX::OrderID(genOrderID()),
        FIX::ExecID(genExecID()),
        FIX::ExecType(FIX::ExecType_FILL),
        FIX::OrdStatus(FIX::OrdStatus_FILLED),
        side,
        FIX::LeavesQty(0),
        FIX::CumQty(orderQty),
        FIX::AvgPx(price));

  executionReport.set(clOrdID);
  executionReport.set(symbol);
  executionReport.set(orderQty);
  executionReport.set(FIX::LastQty(orderQty));
  executionReport.set(FIX::LastPx(price));

  if(message.isSet(account))
    executionReport.setField(message.get(account));

  try
  {
    FIX::Session::sendToTarget(executionReport, sessionID);
  }
  catch (FIX::SessionNotFound&) {}
}

void Application::onMessage(const FIX44::NewOrderSingle& message,
                             const FIX::SessionID& sessionID)
{
  FIX::Symbol symbol;
  FIX::Side side;
  FIX::OrdType ordType;
  FIX::OrderQty orderQty;
  FIX::Price price;
  FIX::ClOrdID clOrdID;
  FIX::Account account;

  message.get(ordType);

  if (ordType != FIX::OrdType_LIMIT)
    throw FIX::IncorrectTagValue(ordType.getField());

  message.get(symbol);
  message.get(side);
  message.get(orderQty);
  message.get(price);
  message.get(clOrdID);

  FIX44::ExecutionReport executionReport = FIX44::ExecutionReport
      (FIX::OrderID(genOrderID()),
        FIX::ExecID(genExecID()),
        FIX::ExecType(FIX::ExecType_FILL),
        FIX::OrdStatus(FIX::OrdStatus_FILLED),
        side,
        FIX::LeavesQty(0),
        FIX::CumQty(orderQty),
        FIX::AvgPx(price));

  executionReport.set(clOrdID);
  executionReport.set(symbol);
  executionReport.set(orderQty);
  executionReport.set(FIX::LastQty(orderQty));
  executionReport.set(FIX::LastPx(price));

  if(message.isSet(account))
    executionReport.setField(message.get(account));

  try
  {
    FIX::Session::sendToTarget(executionReport, sessionID);
  }
  catch (FIX::SessionNotFound&) {}
}

void Application::onMessage(const FIX50::NewOrderSingle& message,
                             const FIX::SessionID& sessionID)
{
  FIX::Symbol symbol;
  FIX::Side side;
  FIX::OrdType ordType;
  FIX::OrderQty orderQty;
  FIX::Price price;
  FIX::ClOrdID clOrdID;
  FIX::Account account;

  message.get(ordType);

  if (ordType != FIX::OrdType_LIMIT)
    throw FIX::IncorrectTagValue(ordType.getField());

  message.get(symbol);
  message.get(side);
  message.get(orderQty);
  message.get(price);
  message.get(clOrdID);

  FIX50::ExecutionReport executionReport = FIX50::ExecutionReport
      (FIX::OrderID(genOrderID()),
        FIX::ExecID(genExecID()),
        FIX::ExecType(FIX::ExecType_FILL),
        FIX::OrdStatus(FIX::OrdStatus_FILLED),
        side,
        FIX::LeavesQty(0),
        FIX::CumQty(orderQty));
  
  executionReport.set(clOrdID);
  executionReport.set(symbol);
  executionReport.set(orderQty);
  executionReport.set(FIX::LastQty(orderQty));
  executionReport.set(FIX::LastPx(price));
  executionReport.set(FIX::AvgPx(price));

  if(message.isSet(account))
    executionReport.setField(message.get(account));

  try
  {
    FIX::Session::sendToTarget(executionReport, sessionID);
  }
  catch (FIX::SessionNotFound&) {}
}


// FIX42 Messages
void 
Application::onMessage(const FIX42::SecurityStatusRequest& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cerr << "Application::SecurityStatus  FIX42" << std::endl;
    }
    // Returns SecurityStatus
}

void 
Application::onMessage(const FIX42::TradingSessionStatusRequest& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cerr << "Application::TradingSessionStatusRequest FIX42" << std::endl;
    }
}
/*
void 
Application::onMessage(const FIX42::SecurityDefinition& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
        std::cerr << "Application::SecurityDefinition FIX42" << std::endl;
    }
}
*/

void 
Application::onMessage(const FIX42::TestRequest& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cerr << "Application::TestRequest FIX42" << std::endl;
    }
}

void 
Application::onMessage(const FIX42::Heartbeat& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cerr << "Application::Heartbeat FIX42" << std::endl;
    }
}

void 
Application::onMessage(const FIX42::Logout& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cerr << "Application::Logout FIX42" << std::endl;
    }
    std::cerr << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
}

void 
Application::onMessage(const FIX42::Logon& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cerr << "Application::Logon FIX42" << std::endl;
    }
}

// FIX42 Market Data
void 
Application::onMessage(const FIX42::MarketDataRequest& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
        std::cerr << "Application::MarketDataRequest FIX42" << std::endl;
    }

    // MarketDepth
    //Note N>1 = best price for N levels, 0 = full book, 1 = top of book
    FIX::MarketDepth marketDepth;
    if (message.isSetField(FIX::FIELD::MarketDepth)) {
        message.getField(marketDepth);
    }
    
    // KTK TODO
    // SubscriptionType
    // Again, for simplicity - we assume that we are receiving subscriptionRequestType = 1 
    // which is Sanpshot + Updates and we do NOT support:
    // 0) Snapshot only
    // 2) Disable previous snapshot
    FIX::SubscriptionRequestType subscriptionRequestType;
    if (message.isSetField(FIX::FIELD::SubscriptionRequestType)) {
        message.getField(subscriptionRequestType);
        // MDUpdateType is requied if SubscriptionRequestType = Snapshot+Update (1)
        FIX::MDUpdateType mdUpdateType;
        // Let QF throw exception if mdUpdateType not set
        //if (message.isSetField(FIX::FIELD::MDUpdateType)) {
        message.getField(mdUpdateType);
        //}
    }

    // MDReqID
    FIX::MDReqID mdReqID;
    if (message.isSetField(FIX::FIELD::MDReqID)) {
        message.getField(mdReqID);
    }

    // KTK TODO
    // Assume MDEntryType = 0,1,2 (i.e. 0+1+2) - bid, offer, trade
    FIX::NoMDEntryTypes noMDEntryTypes;
    FIX42::MarketDataRequest::NoMDEntryTypes mdEntry;
    FIX::MDEntryType mdEntryType;
    message.getField(noMDEntryTypes);
    std::cerr << "NoMDEntryTypes: " << noMDEntryTypes << "\n";
    for (int i = 0; i< noMDEntryTypes; i++) {
        message.getGroup(i+1, mdEntry);
        std::cerr << "Entry type: " << mdEntry.getField(mdEntryType) << "\n";
    }
    std::cout << std::endl;

    FIX::NoRelatedSym noRelatedSym;
    FIX42::MarketDataRequest::NoRelatedSym relatedSym;
    FIX::Symbol symbol;
    message.getField(noRelatedSym);
    std::cout << "Symbols: " << noRelatedSym << std::endl;
    for (int i = 0; i<noRelatedSym; i++) {
        message.getGroup(i+1, relatedSym);
        std::cout << relatedSym.getField(symbol);
    }
    std::cout << std::endl;
/************************************************************/
/* TEST REPLY BEGIN  - Note: This uses msg from pump        */
/************************************************************/
#if 0
// Use the below to test responses from MessagePump
    FIX::Message m;
    FIX::MsgType mType;
    std::string rawLine;
    do {
        _msgPump.getMessage(m, rawLine);
        std::cout << "44444444(retn):" << m << std::endl;
        m.getHeader().getField(mType);
    } while(mType != FIX::MsgType_MarketDataSnapshotFullRefresh);
//std::cerr << "A" << std::endl;
        std::cout << "55555555(raw ):" << rawLine << std::endl;

    FIX::NoMDEntries noMDEntries;
    int nEntries = -1;
    if (m.isSetField(noMDEntries)) {
        m.getField(noMDEntries);
        nEntries = noMDEntries.getValue();
        m.setField(noMDEntries);
 //       std::cerr << "==> NoMDEntries: " << nEntries << std::endl;
    }
    FIX::Symbol msym; m.getField(msym);
        m.getHeader().getField(mType);
//std::cerr << "B (" << msym << ")" << " (" << mType << ")" << std::endl;
//std::cerr <<"B+ (" << m.toString(mstr) << std::endl;
    FIX::Message m2;
    FIX::Session* psession = FIX::Session::lookupSession(sessionID);
    if (!psession) {
        throw FIX::Exception("INVALID SESSION!", "psession is NULL");
    }
    //FIX::DataDictionaryProvider dp = psession->getDataDictionaryProvider();
    //FIX::DataDictionary dict = dp.getSessionDataDictionary(sessionID.getBeginString());
    //const FIX::DataDictionary& dict = dp.getSessionDataDictionary(sessionID.getBeginString());
    FIX::DataDictionary dict("./spec/FIX42.xml");
    FIX::Parser parser;
    parser.addToStream(rawLine);
    std::string parsedLine;
    parser.readFixMessage(parsedLine);
    std::cout << "66666666(pars):" << parsedLine << std::endl;
    m2.setString(parsedLine, false, &dict);
    std::cout << "m2: " << m2 << std::endl;
    FIX42::MarketDataSnapshotFullRefresh refresh(m2);
    FIX42::MarketDataSnapshotFullRefresh::NoMDEntries mde;
    for (int i = 0; i<nEntries; i++) {
        //std::cerr << "m: " << m << std::endl;
        //std::cerr << "m2: " << m2 << std::endl;
        try {
        refresh.getGroup(i+1, mde);
        FIX::MDEntryPx mdEntryPx;
        mde.getField(mdEntryPx);
        std::cerr << "==> MDEntryPx: " << mdEntryPx << std::endl;
        }
        catch (FIX::Exception& e) {
            std::cerr << e.what() << e.detail <<  std::endl;
        }
    }
//std::cerr << "C" << std::endl;

    
    
    FIX::Session::sendToTarget(m2, sessionID);
    return;
#endif 
// Use the below to test canned responses - not related to plumbing like MsgPump or MsgDispatcher 
/*
    FIX42::MarketDataSnapshotFullRefresh fullRefresh;
    fullRefresh.setField(FIX::Symbol(symbol));
    fullRefresh.setField(FIX::MDReqID(mdReqID));
    FIX42::MarketDataSnapshotFullRefresh::NoMDEntries mdEntries;
    
    // KTK TODO - remove flip flop after testing
    FIX::MDEntryType entryType;
    mdEntries.setField(FIX::MDEntryType(FIX::MDEntryType_BID));     
    mdEntries.setField(FIX::MDEntryPx(1.2345));
    mdEntries.setField(FIX::MDEntrySize(9876543));
    fullRefresh.addGroup(mdEntries);

    mdEntries.setField(FIX::MDEntryType(FIX::MDEntryType_OFFER));     
    mdEntries.setField(FIX::MDEntryPx(1.2347));
    mdEntries.setField(FIX::MDEntrySize(3456789));
    fullRefresh.addGroup(mdEntries);
   
    try {
        FIX::Session::sendToTarget(fullRefresh, sessionID);
    }
    catch (FIX::SessionNotFound&) {}
    
    return;
*/
/************************************************************/
/* TEST REPLY END                                           */
/************************************************************/
    FIX::K_ReplayFile k_replayFile;
    if (message.isSetField(k_replayFile)) {
        message.getField(k_replayFile);
        std::cerr << ">>>>>>>> K_REPLAYFILE: " << k_replayFile << std::endl;
    }
    else {
        FIX::K_ReplayFile commandLineFile(_replayLog.c_str());
        k_replayFile = commandLineFile;
    }
    FIX::K_ReplayTimeDiv k_replayTimeDiv;
    if (message.isSetField(k_replayTimeDiv)) {
        message.getField(k_replayTimeDiv);
        std::cerr << ">>>>>>>> K_REPLAYTIMEDIV: " << k_replayTimeDiv << std::endl;
    }
    else {
        FIX::K_ReplayTimeDiv commandLineReplayTimeDiv(_replayTimeDiv);
        k_replayTimeDiv = commandLineReplayTimeDiv;
    }
    FIX::K_Volatility k_volatility;
    if (message.isSetField(k_volatility)) {
        message.getField(k_volatility);
        std::cerr << ">>>>>>>> K_VOLATILITY: " << k_volatility << std::endl;
    }
    else {
        FIX::K_Volatility commandLineVolatility(_replayVolatility);
        k_volatility = commandLineVolatility;
    }
    
    // KTK TODO - check if session info exists for this session already
    // OK - DONE
    SessionMap::iterator sessionIter = _sessionMap.find(sessionID);
    if (sessionIter == _sessionMap.end()) {
       std::cerr << "Session not found" << sessionID << "\n"; 
       std::cerr << "CHECK THAT SESSSION WAS CREATED ON LOGON" << sessionID << "\n"; 
    }
    else {
       std::cerr << "Found session: " << sessionID.toString() << "\n";
        if (_sessionMap[sessionID]->hasDispatcher()) {
            std::cerr << "Session: " << sessionID.toString() << " already has dispatcher" << std::endl;
            // Add symbols
            for (int j = 0; j < noRelatedSym; j++) {
                message.getGroup(j+1, relatedSym);
                relatedSym.getField(symbol);
                _sessionMap[sessionID]->addSymbol(symbol.getString());
            }
        } 
        else {
           std::cerr << "Creating dispatcher for session: " << sessionID.toString() << "\n";
           MP* pPump = new MP();
           pPump->setInputFile(k_replayFile);
           if(!pPump->open()) {
                    std::cerr << "Error opening replay log" << std::endl;
           }
           MsgDispatcher* d = new MsgDispatcher(pPump, _sessionMap[sessionID]);//, s);
           FIX::Session* psession = FIX::Session::lookupSession(sessionID);
           if (!psession) {
                throw FIX::Exception("INVALID SESSION!", "psession is NULL");
           }
           FIX::DataDictionaryProvider dp = psession->getDataDictionaryProvider();
           FIX::DataDictionary dict = dp.getSessionDataDictionary(sessionID.getBeginString());
           d->setDataDictionary(dict);
           d->setSpeedFactor(k_replayTimeDiv);
           _sessionMap[sessionID]->setDispatcher(d);
           std::cerr << "Starting pump and dispatcher for session: " << sessionID.toString() << std::endl;
           pPump->start();
           d->start();
        }
    }
/*
    if (_isReplay) {
    // Start the dispatcher
        // KTK TODO - start the msg dispatcher but this will start for all requests which is not really what we 
        // want but for testing it is fine for now 20110602
        // KTK TODO - create the MsgPump here as well and put in the dispatcher. 
       IF_DEBUG(std::cout << "isReplay: " << _isReplay << std::endl;)
        
        if (_sessionMap[sessionID]->hasDispatcher()) {
            std::cerr << "Session: " << sessionID.toString() << " already has dispatcher" << std::endl;
        } 
        else {
            for (int j = 0; j < noRelatedSym; j++) {
                message.getGroup(j+1, relatedSym);
                relatedSym.getField(symbol);
                sessionIter->second->addSymbol(symbol.getString());
            }
        }
    }
*/

    /* KTK - before sending snapshot full refresh we need to get some data from msgPump if 
    * IF we're in replay mode
    FIX42::MarketDataSnapshotFullRefresh fullRefresh;

    fullRefresh.setField(FIX::Symbol(symbol));
    FIX42::MarketDataSnapshotFullRefresh::NoMDEntries mdEntries;
    fullRefresh.setField(FIX::FIELD::NoMDEntries, "10");
    // KTK - remove flip flopt after testing
    std::stringstream entryID;
    FIX::MDEntryType flipFlop;
    for (int k = 0; k < 5; k++) {
        k % 2 ? flipFlop = FIX::MDEntryType_BID : flipFlop = FIX::MDEntryType_OFFER;
        mdEntries.setField(FIX::MDEntryType(flipFlop));     
        mdEntries.setField(FIX::MDEntryPx(k));
        mdEntries.setField(FIX::MDEntrySize(k*10000));
        mdEntries.setField(FIX::MDReqID(mdReqID));
        //entryID.clear();
        //entryID << symbol.getString() << k; 
        //mdEntries.setField(FIX::MDEntryID(entryID.str()));
    }   
    fullRefresh.addGroup(mdEntries);
    
    try {
        FIX::Session::sendToTarget(fullRefresh, sessionID);
    }
    catch (FIX::SessionNotFound&) {}
    */
}

/*
void 
Application::onMessage(const FIX42::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::MarketDataSnapshotFullRefresh FIX42" << std::endl;
    }
}
*/
/*
void 
Application::onMessage(const FIX42::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::MarketDataIncrementalRefresh FIX42" << std::endl;
    }
}
*/
/*
void 
Application::onMessage(const FIX42::MarketDataRequestReject& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::MarketDataRequestReject FIX42" << std::endl;
    }
}
*/

// FIX43 Messages
void 
Application::onMessage(const FIX43::SecurityStatusRequest& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::SecurityStatus FIX42" << std::endl;
    }
}

void 
Application::onMessage(const FIX43::TradingSessionStatusRequest& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::TradingSessionStatusRequest FIX42" << std::endl;
    }
}

/*
void 
Application::onMessage(const FIX43::SecurityDefinition& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::SecurityDefinition FIX42" << std::endl;
    }
}
*/

void 
Application::onMessage(const FIX43::TestRequest& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::TestRequest FIX42" << std::endl;
    }
}

void 
Application::onMessage(const FIX43::Heartbeat& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::Heartbeat FIX42" << std::endl;
    }
}

void 
Application::onMessage(const FIX43::Logout& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::Logout FIX42" << std::endl;
    }
}

void 
Application::onMessage(const FIX43::Logon& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::Logon FIX42" << std::endl;
    }
}

// FIX43 Market data
void 
Application::onMessage(const FIX43::MarketDataRequest& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::MarketDataRequest FIX42" << std::endl;
    }
}
/*
void 
Application::onMessage(const FIX43::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::MarketDataSnapshotFullRefresh FIX42" << std::endl;
    }
}
*/
/*
void 
Application::onMessage(const FIX43::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::MarketDataIncrementalRefresh FIX42" << std::endl;
    }
}
*/
/*
void 
Application::onMessage(const FIX43::MarketDataRequestReject& message, const FIX::SessionID& sessionID)
{
    if (_config.printDebug) {
        std::cout << "Application::MarketDataRequestReject FIX42" << std::endl;
    }
}
*/
