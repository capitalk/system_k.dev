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

#include "quickfix/fix44/ExecutionReport.h"
#include "quickfix/fix44/MarketDataIncrementalRefresh.h"
#include "quickfix/fix44/MarketDataRequest.h"
#include "quickfix/fix44/MarketDataRequestReject.h"
#include "quickfix/fix44/MarketDataSnapshotFullRefresh.h"
#include "quickfix/fix44/MarketDataRequest.h"
#include "quickfix/fix44/Logout.h"
#include "quickfix/fix44/Logon.h"
#include "quickfix/fix44/Heartbeat.h"
#include "quickfix/fix44/SequenceReset.h"
#include "quickfix/fix44/ResendRequest.h"
#include "quickfix/fix44/TestRequest.h"
#include "quickfix/fix44/TradingSessionStatus.h"
#include "quickfix/fix44/SecurityStatus.h"
#include "quickfix/fix44/SecurityDefinition.h"
#include "quickfix/fix44/SecurityDefinitionRequest.h"

#include <string>
#include <queue>
#include <boost/iostreams/device/file.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/make_shared.hpp>

#include <zmq.hpp>
#include "order_book.v2/order_book.h"
#include "order_book.v2/book_types.h"
#include "utils/types.h"
#include "utils/time_utils.h"
#include "utils/logging.h"

enum FIXVersion {
	FIX_42 = 42,
	FIX_43 = 43,
	FIX_44 = 44,
	FIX_50 = 50
};

struct ApplicationConfig { 
	std::string mic_string; 
    int venue_id;
	std::string username; 
	std::string password; 
	bool sendPasswordInRawDataField;
	bool aggregatedBook;  
	bool sendIndividualMarketDataRequests;
	FIXVersion version;
	bool printDebug; 
    int marketDepth;
    std::string loggingBroadcastAddr;
}; 

class Application :
			public FIX::Application,
			public FIX::MessageCracker
{
public:
	Application(bool bReset, const ApplicationConfig& config) 
         :  _loggedIn(false), 
            _loggedOut(false), _loginCount(0), 
            _appMsgCount(0), _resetSequence(bReset), _config(config) {

		std::cout << "Application::Application()" << std::endl;
	      /* _msgLog.rdbuf(&_msgBuf); _msgBuf.open("msgBuf.log");
	       *_bookLog.rdbuf(&_bookBuf);
	       *_bookLog.rdbuf(&_bookBuf); _bookBuf.open("baxterOrderBook.log");*/
	}
	~Application(); //{ std::flush(_bookLog); std::flush(_msgLog); };
	void run();

	//void addBook(const std::string& symbol, capitalk::PriceDepthOrderBook* book);
	//capitalk::PriceDepthOrderBook*  getBook(const std::string& symbol);
	void addBook(const std::string& symbol, capk::KBook* book);
    capk::KBook*  getBook(const std::string& symbol);

	void addStream(const std::string& symbol, std::ostream* log);
	std::ostream*  getStream(const std::string& symbol);

	void addSymbols(const std::vector<std::string>& symbols);
	const std::vector<std::string>& getSymbols();
	
	void setDataPath(const std::string&);
	void setUpdateType(const long updateType);
    void deleteBooks();
    void flushLogs();

    void setZMQContext(void* c) { this->_pzmq_context = c;}
    void setZMQSocket(void* s) { this->_pzmq_socket = s;}
    void setPublishing(bool isPublishing) { this->_isPublishing = isPublishing;}
    void setLogging(bool isLogging) { this->_isLogging = isLogging;}
    void* getZMQSocket() { return this->_pzmq_socket;}
    void* getZMQContext() { return this->_pzmq_context;}

    
	//void addStream(const std::string& symbol, boost::shared_ptr<std::ostream> log);
	//boost::shared_ptr<std::ostream> getStream(const std::string& symbol);

private:
    void broadcast_bbo_book(void* bcast_socket, const char* symbol, const double best_bid, const double best_ask, const double bbsize, const double basize, const capk::venue_id_t venue_id);
        

    template <typename T> 
    void full_refresh_template(const T& message, const FIX::SessionID& sessionID); 

    template <typename T> 
    void incremental_update_template(const T& message, const FIX::SessionID& sessionID); 

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
	void onMessage(const FIX42::TestRequest&, const FIX::SessionID&);
	void onMessage (const FIX42::Heartbeat& message, const FIX::SessionID& sessionID);
	//void onMessage(const FIX42::ResendRequest&, const FIX::SessionID&);
	void onMessage(const FIX42::Logout&, const FIX::SessionID&);
	void onMessage(const FIX42::Logon&, const FIX::SessionID&);
	void onMessage(const FIX42::MarketDataRequest&, const FIX::SessionID&);
	void onMessage (const FIX42::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID);
	void onMessage (const FIX42::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID);
	void onMessage (const FIX42::MarketDataRequestReject& message, const FIX::SessionID& sessionID);

	void onMessage(const FIX43::TradingSessionStatus&, const FIX::SessionID&);
	void onMessage(const FIX43::SecurityStatus&, const FIX::SessionID&);
	void onMessage(const FIX43::TestRequest&, const FIX::SessionID&);
	void onMessage(const FIX43::Heartbeat& message, const FIX::SessionID& sessionID);
	//void onMessage(const FIX43::ResendRequest&, const FIX::SessionID&);
	void onMessage(const FIX43::Logout&, const FIX::SessionID&);
	void onMessage(const FIX43::Logon&, const FIX::SessionID&);
	void onMessage(const FIX43::MarketDataRequest&, const FIX::SessionID&);
	void onMessage (const FIX43::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID);
	void onMessage (const FIX43::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID);
	void onMessage (const FIX43::MarketDataRequestReject& message, const FIX::SessionID& sessionID);
	
	void onMessage(const FIX44::TradingSessionStatus&, const FIX::SessionID&);
	void onMessage(const FIX44::SecurityStatus&, const FIX::SessionID&);
	void onMessage(const FIX44::TestRequest&, const FIX::SessionID&);
	void onMessage(const FIX44::Heartbeat& message, const FIX::SessionID& sessionID);
	//void onMessage(const FIX44::ResendRequest&, const FIX::SessionID&);
	void onMessage(const FIX44::Logout&, const FIX::SessionID&);
	void onMessage(const FIX44::Logon&, const FIX::SessionID&);
	void onMessage(const FIX44::MarketDataRequest&, const FIX::SessionID&);
	void onMessage (const FIX44::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID);
	void onMessage (const FIX44::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID);
	void onMessage (const FIX44::MarketDataRequestReject& message, const FIX::SessionID& sessionID);

	void sendTestRequest();
	FIX42::TestRequest sendTestRequest42();
	FIX43::TestRequest sendTestRequest43();
	FIX44::TestRequest sendTestRequest44();

	void querySingleMarketDataRequest(const std::string& symbols)
	throw(std::exception);

	FIX42::MarketDataRequest querySingleMarketDataRequest42(const std::string& symbol);
	FIX43::MarketDataRequest querySingleMarketDataRequest43(const std::string& symbol);
	FIX44::MarketDataRequest querySingleMarketDataRequest44(const std::string& symbol);

	void queryMarketDataRequest(const std::vector<std::string>& symbols);
	FIX42::MarketDataRequest queryMarketDataRequest42(const std::vector<std::string>& symbols);
	FIX43::MarketDataRequest queryMarketDataRequest43(const std::vector<std::string>& symbols);
	FIX44::MarketDataRequest queryMarketDataRequest44(const std::vector<std::string>& symbols);

	FIX::SessionID _sessionID;

	boost::filesystem::path _pathToLog;

	std::vector<std::string> _symbols;	

	bool _loggedIn;
	bool _loggedOut;
	unsigned int _loginCount;
    unsigned int _appMsgCount;
	bool _resetSequence;
	const ApplicationConfig& _config; 

    void* _pzmq_socket;
    void* _pzmq_context;
    bool _isPublishing;
    bool _isLogging;

	int _MDUpdateType;
    
	//std::ostream _bookLog;
	//std::ostream _msgLog;

	//boost::iostreams::stream_buffer<boost::iostreams::file_sink> _msgBuf;
	//boost::iostreams::stream_buffer<boost::iostreams::file_sink> _bookBuf;

	std::map<std::string, std::ostream* > _symbolToLogStream;
	typedef std::map<std::string, std::ostream* >::iterator symbolToLogStreamIterator;

	//std::map<std::string, capitalk::PriceDepthOrderBook* > _symbolToBook;
	//typedef std::map<std::string, capitalk::PriceDepthOrderBook* >::iterator symbolToBookIterator;
	std::map<std::string, capk::KBook* > _symbolToBook;
	typedef std::map<std::string, capk::KBook* >::iterator symbolToBookIterator;
};

#endif
