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
#include "quickfix/SocketInitiator.h"
#include "quickfix/ThreadedSocketInitiator.h"

#include "quickfix/fix42/ExecutionReport.h"
#include "quickfix/fix42/MarketDataIncrementalRefresh.h"
#include "quickfix/fix42/MarketDataRequest.h"
#include "quickfix/fix42/MarketDataRequestReject.h"
#include "quickfix/fix42/MarketDataSnapshotFullRefresh.h"
#include "quickfix/fix42/MarketDataRequest.h"
#include "quickfix/fix42/Logout.h"
#include "quickfix/fix42/Logon.h"
#include "quickfix/fix42/Heartbeat.h"
#include "quickfix/fix42/SequenceReset.h"
#include "quickfix/fix42/ResendRequest.h"
#include "quickfix/fix42/TestRequest.h"
#include "quickfix/fix42/TradingSessionStatus.h"
#include "quickfix/fix42/SecurityStatus.h"
#include "quickfix/fix42/SecurityDefinition.h"
#include "quickfix/fix42/SecurityDefinitionRequest.h"

#include "quickfix/fix43/ExecutionReport.h"
#include "quickfix/fix43/MarketDataIncrementalRefresh.h"
#include "quickfix/fix43/MarketDataRequest.h"
#include "quickfix/fix43/MarketDataRequestReject.h"
#include "quickfix/fix43/MarketDataSnapshotFullRefresh.h"
#include "quickfix/fix43/MarketDataRequest.h"
#include "quickfix/fix43/Logout.h"
#include "quickfix/fix43/Logon.h"
#include "quickfix/fix43/Heartbeat.h"
#include "quickfix/fix43/SequenceReset.h"
#include "quickfix/fix43/ResendRequest.h"
#include "quickfix/fix43/TestRequest.h"
#include "quickfix/fix43/TradingSessionStatus.h"
#include "quickfix/fix43/SecurityStatus.h"
#include "quickfix/fix43/SecurityDefinition.h"
#include "quickfix/fix43/SecurityDefinitionRequest.h"

#include <string>
#include <queue>
#include <boost/iostreams/device/file.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/make_shared.hpp>

#include <zmq.hpp>
#include "order_book/KBook.h"


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
	std::string username; 
	std::string password; 
	bool sendPasswordInRawDataField;
	bool aggregatedBook;  
	bool sendIndividualMarketDataRequests;
	FIXVersion version;
	bool printDebug; 
    int marketDepth;
}; 

class Application :
			public FIX::Application,
			public FIX::MessageCracker
{
public:
	Application(const bool bReset, const ApplicationConfig& config) 
         :  _loggedIn(false), _loggedOut(false), _loginCount(0), 
            _appMsgCount(0), _config(config) {

  	   std::cout << "Application::Application()" << std::endl;
		_resetSequence = bReset;
        _querySecurityDefinitionComplete = false;
	}
	~Application(); //{ std::flush(_bookLog); std::flush(_msgLog); };
	void run();

	//void addBook(const std::string& symbol, capitalk::PriceDepthOrderBook* book);
	//capitalk::PriceDepthOrderBook*  getBook(const std::string& symbol);
	void addBook(const std::string& symbol, KBook* book);
	KBook*  getBook(const std::string& symbol);

	void addStream(const std::string& symbol, std::ostream* log);
	std::ostream*  getStream(const std::string& symbol);

	void addSymbols(const std::vector<std::string>& symbols);
	const std::vector<std::string>& getSymbols();
	
	//void addStream(const std::string& symbol, boost::shared_ptr<std::ostream> log);
	//boost::shared_ptr<std::ostream> getStream(const std::string& symbol);

private:
    char menu();

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

	void onMessage(const FIX42::TradingSessionStatus&, const FIX::SessionID&);
	void onMessage(const FIX42::SecurityStatus&, const FIX::SessionID&);
	void onMessage(const FIX42::SecurityDefinition&, const FIX::SessionID&);
	void onMessage (const FIX42::Heartbeat& message, const FIX::SessionID& sessionID);
	void onMessage(const FIX42::Logout&, const FIX::SessionID&);
	void onMessage(const FIX42::Logon&, const FIX::SessionID&);

	void onMessage(const FIX43::TradingSessionStatus&, const FIX::SessionID&);
	void onMessage(const FIX43::SecurityStatus&, const FIX::SessionID&);
	//void onMessage(const FIX43::SecurityDefinition&, const FIX::SessionID&);
	void onMessage(const FIX43::Heartbeat& message, const FIX::SessionID& sessionID);
	void onMessage(const FIX43::Logout&, const FIX::SessionID&);
	void onMessage(const FIX43::Logon&, const FIX::SessionID&);
    
	
	void logout();
	FIX42::Logout logout42();
	FIX43::Logout logout43();


	void querySecurityDefinitionRequest(const std::string& securityReqID);
	FIX42::SecurityDefinitionRequest querySecurityDefinitionRequest42(const std::string& securityReqID);
	FIX43::SecurityDefinitionRequest querySecurityDefinitionRequest43(const std::string& securityReqID);

	FIX::SessionID _sessionID;

	boost::filesystem::path _pathToLog;

	std::vector<std::string> _symbols;	

    int _nValidSymbols;
	std::vector<std::string> _validSymbols;	

	bool _loggedIn;
	bool _loggedOut;
	unsigned int _loginCount;
    unsigned int _appMsgCount;
	const ApplicationConfig& _config; 
	bool _resetSequence;

    bool _querySecurityDefinitionComplete;
    

};

#endif
