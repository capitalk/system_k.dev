/* -*- C++ -*- */

/****************************************************************************
** Copyright (c) quickfixengine.org	All rights reserved.
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

#ifndef TRADECLIENT_APPLICATION_H
#define TRADECLIENT_APPLICATION_H

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Values.h"
#include "quickfix/Mutex.h"

#include "quickfix/fix42/Logout.h"
#include "quickfix/fix42/Logon.h"
#include "quickfix/fix42/Heartbeat.h"
#include "quickfix/fix42/SequenceReset.h"
#include "quickfix/fix42/ResendRequest.h"
#include "quickfix/fix42/TradingSessionStatus.h"
#include "quickfix/fix42/NewOrderSingle.h"
#include "quickfix/fix42/ExecutionReport.h"
#include "quickfix/fix42/OrderCancelRequest.h"
#include "quickfix/fix42/OrderCancelReject.h"
#include "quickfix/fix42/OrderCancelReplaceRequest.h"

#include "quickfix/fix43/Logout.h"
#include "quickfix/fix43/Logon.h"
#include "quickfix/fix43/Heartbeat.h"
#include "quickfix/fix43/SequenceReset.h"
#include "quickfix/fix43/ResendRequest.h"
#include "quickfix/fix43/TradingSessionStatus.h"
#include "quickfix/fix43/NewOrderSingle.h"
#include "quickfix/fix43/ExecutionReport.h"
#include "quickfix/fix43/OrderCancelRequest.h"
#include "quickfix/fix43/OrderCancelReject.h"
#include "quickfix/fix43/OrderCancelReplaceRequest.h"

#include "quickfix/fix44/Logout.h"
#include "quickfix/fix44/Logon.h"
#include "quickfix/fix44/Heartbeat.h"
#include "quickfix/fix44/SequenceReset.h"
#include "quickfix/fix44/ResendRequest.h"
#include "quickfix/fix44/TradingSessionStatus.h"
#include "quickfix/fix44/NewOrderSingle.h"
#include "quickfix/fix44/ExecutionReport.h"
#include "quickfix/fix44/OrderCancelRequest.h"
#include "quickfix/fix44/OrderCancelReject.h"
#include "quickfix/fix44/OrderCancelReplaceRequest.h"

#include "quickfix/fixt11/Logout.h"
#include "quickfix/fixt11/Logon.h"
#include "quickfix/fixt11/Heartbeat.h"
#include "quickfix/fixt11/SequenceReset.h"
#include "quickfix/fixt11/ResendRequest.h"
#include "quickfix/fixt11/Message.h"
#include "quickfix/fixt11/MessageCracker.h"
#include "quickfix/fix50/TradingSessionStatus.h"
#include "quickfix/fix50/NewOrderSingle.h"
#include "quickfix/fix50/ExecutionReport.h"
#include "quickfix/fix50/OrderCancelRequest.h"
#include "quickfix/fix50/OrderCancelReject.h"
#include "quickfix/fix50/OrderCancelReplaceRequest.h"

#include <string>
#include <queue>
#include <boost/iostreams/device/file.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/make_shared.hpp>

#include <zmq.hpp>

#include "timing.h"
#include "logging.h"


enum FIXVersion {
	FIX_42 = 42,
	FIX_43 = 43,
	FIX_44 = 44,
	FIX_50 = 50
};

struct ApplicationConfig { 
	std::string mic_code; 
	std::string username; 
	std::string password; 
	bool sendPasswordInRawDataField;
	bool aggregatedBook;  
	bool sendIndividualMarketDataRequests;
	//FIXVersion version;
	std::string version;
	bool printDebug; 
    int marketDepth;
	std::string zmq_bind_addr;
	std::string account;
	int handlInst;
}; 

class Application :
			public FIX::Application,
			public FIX::MessageCracker
{
public:
	Application(bool bReset, const ApplicationConfig& config);
	~Application(); 

	void* run();
	void test();

    void setZMQContext(zmq::context_t* c) { this->_pzmq_context = c;}
    void setZMQSocket(zmq::socket_t* s) { this->_pzmq_socket = s;}
    void setPublishing(bool isPublishing) { this->_isPublishing = isPublishing;}
    inline bool isPublishing() { return _isPublishing;}
    zmq::socket_t* getZMQSocket() { return this->_pzmq_socket;}
    zmq::context_t* getZMQContext() { return this->_pzmq_context;}

	inline void setAccount1(const std::string& account) { this->_account = account; };
	
	inline void setHandlInst21(const char handlInst) { this->_handlInst = handlInst; };

	inline void setUseCurrency15(const bool useCurrency) { this->_useCurrency = useCurrency; };

	inline void setLimitOrderChar40(const char limitOrderChar) { this->_limitOrderChar = limitOrderChar; };


private:

//   template <typename T> 
//   void incremental_update_template(const T& message, const FIX::SessionID& sessionID); 

	void onCreate(const FIX::SessionID&); 
	void onLogon(const FIX::SessionID& sessionID);
	void onLogout(const FIX::SessionID& sessionID);
	void toAdmin(FIX::Message&, const FIX::SessionID&); 
	void toApp(FIX::Message&, const FIX::SessionID& )
	    throw(FIX::DoNotSend);
	void fromAdmin(const FIX::Message&, const FIX::SessionID& )
	    throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);
	void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID )
	    throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

	void onMessage(const FIX::Message&, const FIX::SessionID&);

	void onMessage(const FIX42::Logon&, const FIX::SessionID&);
	void onMessage(const FIX42::Logout&, const FIX::SessionID&);
	void onMessage(const FIX42::Heartbeat& message, const FIX::SessionID& sessionID);
	void onMessage(const FIX42::TradingSessionStatus&, const FIX::SessionID&);
	void onMessage(const FIX42::ExecutionReport&, const FIX::SessionID&);
	void onMessage(const FIX42::OrderCancelReject&, const FIX::SessionID&);

	void onMessage(const FIX43::Logon&, const FIX::SessionID&);
	void onMessage(const FIX43::Logout&, const FIX::SessionID&);
	void onMessage(const FIX43::Heartbeat& message, const FIX::SessionID& sessionID);
	void onMessage(const FIX43::TradingSessionStatus&, const FIX::SessionID&);
	void onMessage(const FIX43::ExecutionReport&, const FIX::SessionID&);
	void onMessage(const FIX43::OrderCancelReject&, const FIX::SessionID&);

	void onMessage(const FIX44::Logon&, const FIX::SessionID&);
	void onMessage(const FIX44::Logout&, const FIX::SessionID&);
	void onMessage(const FIX44::Heartbeat& message, const FIX::SessionID& sessionID);
	void onMessage(const FIX44::TradingSessionStatus&, const FIX::SessionID&);
	void onMessage(const FIX44::ExecutionReport&, const FIX::SessionID&);
	void onMessage(const FIX44::OrderCancelReject&, const FIX::SessionID&);

	void onMessage(const FIXT11::Logon&, const FIX::SessionID&);
	void onMessage(const FIXT11::Logout&, const FIX::SessionID&);
	void onMessage(const FIXT11::Heartbeat& message, const FIX::SessionID& sessionID);
	void onMessage(const FIX50::TradingSessionStatus&, const FIX::SessionID&);
	void onMessage(const FIX50::ExecutionReport&, const FIX::SessionID&);
	void onMessage(const FIX50::OrderCancelReject&, const FIX::SessionID&);

	// TESTING FUNCTIONS
	FIX42::NewOrderSingle queryNewOrderSingle42();
	FIX43::NewOrderSingle queryNewOrderSingle43();
	FIX44::NewOrderSingle queryNewOrderSingle44();
	FIX50::NewOrderSingle queryNewOrderSingle50();

	FIX42::OrderCancelRequest queryOrderCancelRequest42();
	FIX43::OrderCancelRequest queryOrderCancelRequest43();
	FIX44::OrderCancelRequest queryOrderCancelRequest44();
	FIX50::OrderCancelRequest queryOrderCancelRequest50();

	FIX42::OrderCancelReplaceRequest queryCancelReplaceRequest42();
	FIX43::OrderCancelReplaceRequest queryCancelReplaceRequest43();
	FIX44::OrderCancelReplaceRequest queryCancelReplaceRequest44();
	FIX50::OrderCancelReplaceRequest queryCancelReplaceRequest50();

	void queryHeader(FIX::Header&);
	char queryAction();
	bool queryConfirm(const std::string&);
	void queryEnterOrder();
	void queryCancelOrder();
	void queryReplaceOrder();
	FIX::OrdType queryOrdType();
	FIX::OrigClOrdID queryOrigClOrdID();
	int queryVersion();
	FIX::SenderCompID querySenderCompID();
	FIX::TargetCompID queryTargetCompID();
	FIX::TargetSubID queryTargetSubID();
	FIX::ClOrdID queryClOrdID();
	FIX::Symbol querySymbol();
	FIX::Side querySide();
	FIX::OrderQty queryOrderQty();
	FIX::Price queryPrice();
	FIX::StopPx queryStopPx();
	FIX::TimeInForce queryTimeInForce();

	
	FIX::SessionID _sessionID;

	boost::filesystem::path _pathToLog;

	bool _loggedIn;
	bool _loggedOut;
	unsigned int _loginCount;
    unsigned int _appMsgCount;
	const ApplicationConfig& _config; 
	bool _resetSequence;

    zmq::socket_t* _pzmq_socket;
    zmq::context_t* _pzmq_context;
    bool _isPublishing;

	std::string _account;
	int _handlInst;
	bool _useCurrency;
	char _limitOrderChar;
	

};

#endif
