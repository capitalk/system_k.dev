/* -*- C++ -*- */

/****************************************************************************
** Copyright (c) quickfixengine.org All rights reserved.
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

#ifndef MARKET_DATA_COLLECT_FIX_APPLICATION_H_
#define MARKET_DATA_COLLECT_FIX_APPLICATION_H_

#include <zmq.hpp>

#include <boost/iostreams/device/file.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/make_shared.hpp>

#include <string>
#include <queue>
#include <map>
#include <vector>

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Values.h"
#include "quickfix/Mutex.h"

#include "quickfix/fix42/ExecutionReport.h"
#include "quickfix/fix42/MarketDataIncrementalRefresh.h"
#include "quickfix/fix42/MarketDataRequest.h"
#include "quickfix/fix42/MarketDataRequestReject.h"
#include "quickfix/fix42/MarketDataSnapshotFullRefresh.h"
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

#include "quickfix/fix50sp2/ExecutionReport.h"
#include "quickfix/fix50sp2/MarketDataIncrementalRefresh.h"
#include "quickfix/fix50sp2/MarketDataRequest.h"
#include "quickfix/fix50sp2/MarketDataRequestReject.h"
#include "quickfix/fix50sp2/MarketDataSnapshotFullRefresh.h"
#include "quickfix/fixt11/Logout.h"
#include "quickfix/fixt11/Logon.h"
#include "quickfix/fixt11/Heartbeat.h"
#include "quickfix/fixt11/SequenceReset.h"
#include "quickfix/fixt11/ResendRequest.h"
#include "quickfix/fixt11/TestRequest.h"
#include "quickfix/fix50sp2/TradingSessionStatus.h"
#include "quickfix/fix50sp2/SecurityStatus.h"
#include "quickfix/fix50sp2/SecurityDefinition.h"
#include "quickfix/fix50sp2/SecurityDefinitionRequest.h"

#include "order_book.v2/order_book.h"
#include "order_book.v2/book_types.h"
#include "utils/types.h"
#include "utils/time_utils.h"

enum FIXVersion {
    BAD_FIX_VERSION = 0,
    FIX_42 = 42,
    FIX_43 = 43,
    FIX_44 = 44,
    FIX_50 = 50,
    FIX_50SP1 = 51,
    FIX_50SP2 = 52
};

struct ApplicationConfig {
    std::string mic_string;
    int32_t venue_id;
    std::string username;
    std::string password;
    bool sendPasswordInRawDataField;
    bool sendIndividualMarketDataRequests;
    FIXVersion version;
    bool print_debug;
    int32_t market_depth;
    std::string loggingBroadcastAddr;
    std::string order_books_output_dir;
    std::string fix_store_output_dir;
    std::string fix_log_output_dir;
    std::string root_output_dir;
    std::string symbol_file_name;
    std::string config_file_name;
    bool is_publishing;
    std::string publishing_addr;
    std::string config_server_addr;
    bool is_logging;
    ///  FIX AggreagatedBook (266)
    bool want_aggregated_book;
    ///  FIX MDUpdateType (265)
    int32_t update_type;
    ///  FIX ResetSeqNumFlag (141)
    bool reset_seq_nums;
    ///   Replace entirely existing orders with same id in orderbook?
    bool new_replaces;
};

class Application : public FIX::Application,
    public FIX::MessageCracker {
    public:
    explicit Application(const ApplicationConfig& config);

    ~Application();
    void run();

    void setZMQContext(void* c) { this->_pzmq_context = c;}
    void setZMQSocket(void* s) { this->_pzmq_socket = s;}
    void* getZMQSocket() { return this->_pzmq_socket;}
    void* getZMQContext() { return this->_pzmq_context;}

    void deleteBooks();
    void deleteLogs();

    void addSymbols(const std::vector<std::string>& symbols);
    const std::vector<std::string>& getSymbols();

    private:
    void addBook(const std::string& symbol, capk::KBook* book);
    capk::KBook*  getBook(const std::string& symbol);

    void addStream(const std::string& symbol, std::ostream* log);
    std::ostream*  getStream(const std::string& symbol);

    void setUpdateType(const int32_t updateType);

    void broadcast_bbo_book(void* bcast_socket,
            const char* symbol,
            const double best_bid,
            const double best_ask,
            const double bbsize,
            const double basize,
            const capk::venue_id_t venue_id);

    template <typename T>
    void full_refresh_template(const T& message,
            const FIX::SessionID& sessionID);

    template <typename T>
    void incremental_update_template(const T& message,
            const FIX::SessionID& sessionID);

    template <typename T>
    void trading_session_status_template(const T& message,
            const FIX::SessionID& sessionID);

    void onCreate(const FIX::SessionID&);

    void onLogon(const FIX::SessionID& sessionID);

    void onLogout(const FIX::SessionID& sessionID);

    void toAdmin(FIX::Message&, const FIX::SessionID&);

    void toApp(FIX::Message&, const FIX::SessionID& )
        throw(FIX::DoNotSend);

    void fromAdmin(const FIX::Message&, const FIX::SessionID& )
        throw(FIX::FieldNotFound,
                FIX::IncorrectDataFormat,
                FIX::IncorrectTagValue,
                FIX::RejectLogon);

    void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID )
        throw(FIX::FieldNotFound,
                FIX::IncorrectDataFormat,
                FIX::IncorrectTagValue,
                FIX::UnsupportedMessageType);

    void onMessage(const FIX42::TradingSessionStatus&, const FIX::SessionID&);
    void onMessage(const FIX42::Logon&, const FIX::SessionID&);
    void onMessage(const FIX42::MarketDataSnapshotFullRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX42::MarketDataIncrementalRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX42::MarketDataRequestReject& message,
            const FIX::SessionID& sessionID);

    void onMessage(const FIX43::TradingSessionStatus&, const FIX::SessionID&);
    void onMessage(const FIX43::Logon&, const FIX::SessionID&);
    void onMessage(const FIX43::MarketDataSnapshotFullRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX43::MarketDataIncrementalRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX43::MarketDataRequestReject& message,
            const FIX::SessionID& sessionID);

    void onMessage(const FIX44::TradingSessionStatus&, const FIX::SessionID&);
    void onMessage(const FIX44::Logon&, const FIX::SessionID&);
    void onMessage(const FIX44::MarketDataSnapshotFullRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX44::MarketDataIncrementalRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX44::MarketDataRequestReject& message,
            const FIX::SessionID& sessionID);

    void sendTestRequest();
    FIX42::TestRequest sendTestRequest42();
    FIX43::TestRequest sendTestRequest43();
    FIX44::TestRequest sendTestRequest44();
    FIXT11::TestRequest sendTestRequest50();

    void querySingleMarketDataRequest(const std::string& symbols);
    FIX42::MarketDataRequest querySingleMarketDataRequest42(
        const std::string& symbol);
    FIX43::MarketDataRequest querySingleMarketDataRequest43(
        const std::string& symbol);
    FIX44::MarketDataRequest querySingleMarketDataRequest44(
        const std::string& symbol);
    FIX50SP2::MarketDataRequest querySingleMarketDataRequest50(
        const std::string& symbol);

    void queryMarketDataRequest(const std::vector<std::string>& symbols);
    FIX42::MarketDataRequest queryMarketDataRequest42(
        const std::vector<std::string>& symbols);
    FIX43::MarketDataRequest queryMarketDataRequest43(
        const std::vector<std::string>& symbols);
    FIX44::MarketDataRequest queryMarketDataRequest44(
        const std::vector<std::string>& symbols);
    FIX50SP2::MarketDataRequest queryMarketDataRequest50(
        const std::vector<std::string>& symbols);

    FIX::SessionID _sessionID;
    std::vector<std::string> _symbols;
    boost::filesystem::path _pathToLog;

    unsigned int _loginCount;
    unsigned int _appMsgCount;
    bool _resetSequence;
    const ApplicationConfig& _config;

    void* _pzmq_socket;
    void* _pzmq_context;

    std::map<std::string, std::ostream* > _symbolToLogStream;
    typedef std::map<std::string, std::ostream* >::iterator
        symbolToLogStreamIterator;

    std::map<std::string, capk::KBook* > _symbolToBook;
    typedef std::map<std::string, capk::KBook* >::iterator
        symbolToBookIterator;
};

#endif  // MARKET_DATA_COLLECT_FIX_APPLICATION_H_
