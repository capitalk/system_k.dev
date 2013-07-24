/* -*- C++ -*- */

#ifndef MARKET_DATA_COLLECT_FIX_APPLICATION_H_
#define MARKET_DATA_COLLECT_FIX_APPLICATION_H_

#define USE_FIX_42
//#define USE_FIX_43
//#define USE_FIX_44
//#define USE_FIX_50
//#define USE_FIX_50SP2

#include <zmq.hpp>

#include <boost/filesystem.hpp>

#include <string>
#include <queue>
#include <map>
#include <vector>

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"

#ifdef USE_FIX_42
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
//#include "quickfix/fix42/SecurityStatus.h"
//#include "quickfix/fix42/SecurityDefinition.h"
//#include "quickfix/fix42/SecurityDefinitionRequest.h"
#endif //  USE_FIX_42

#ifdef USE_FIX_43
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
//#include "quickfix/fix43/SecurityStatus.h"
//#include "quickfix/fix43/SecurityDefinition.h"
//#include "quickfix/fix43/SecurityDefinitionRequest.h"
#endif //  USE_FIX_43

#ifdef USE_FIX_44
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
//#include "quickfix/fix44/SecurityStatus.h"
//#include "quickfix/fix44/SecurityDefinition.h"
//#include "quickfix/fix44/SecurityDefinitionRequest.h"
#endif //  USE_FIX_44

#ifdef USE_FIX_50
#include "quickfix/fix50/MarketDataIncrementalRefresh.h"
#include "quickfix/fix50/MarketDataRequest.h"
#include "quickfix/fix50/MarketDataRequestReject.h"
#include "quickfix/fix50/MarketDataSnapshotFullRefresh.h"
#include "quickfix/fix50/TradingSessionStatus.h"
//#include "quickfix/fix50/SecurityStatus.h"
//#include "quickfix/fix50/SecurityDefinition.h"
//#include "quickfix/fix50/SecurityDefinitionRequest.h"
#endif //  USE_FIX_50

#ifdef  USE_FIX_50SP2
#include "quickfix/fix50sp2/MarketDataIncrementalRefresh.h"
#include "quickfix/fix50sp2/MarketDataRequest.h"
#include "quickfix/fix50sp2/MarketDataRequestReject.h"
#include "quickfix/fix50sp2/MarketDataSnapshotFullRefresh.h"
#include "quickfix/fix50sp2/TradingSessionStatus.h"
//#include "quickfix/fix50sp2/SecurityStatus.h"
//#include "quickfix/fix50sp2/SecurityDefinition.h"
//#include "quickfix/fix50sp2/SecurityDefinitionRequest.h"
#endif //  USE_FIX50SP2

#if defined(USE_FIX_50) || defined(USE_FIX50SP2)
#define USE_FIXT11
#include "quickfix/fixt11/Logon.h"
#include "quickfix/fixt11/TestRequest.h"
#endif //  defined(USE_FIX_50) || defined(USE_FIX50SP2) 

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
    // The ISO MIC code for this market/ECN/etc.
    std::string mic_string;
    // Integer id of this venue
    int32_t venue_id;
    std::string username;
    std::string password;
    // Should we send password in raw data? 
    bool sendPasswordInRawDataField;
    // When subscribing to more than one symbol should we send one request for
    // each symbol or all in one message
    bool sendIndividualMarketDataRequests;
    FIXVersion version;
    // Should we print debug information - N.B. may only be used when 
    // built with -D LOG
    bool print_debug;
    // Broadcast addr for logging
    std::string logging_broadcast_addr;
    // Top level directory of storage hierarchy - both store and fix messge 
    // log are stored there. Directories created if they don't exist. 
    std::string root_output_dir;
    // Where to store FIX logs and sequence info if saved in file relative to
    // root_output_dir
    std::string fix_log_output_dir;
    std::string fix_store_output_dir;
    // Path to file containing symbols we want to subscribe to
    std::string symbol_file_name;
    // Name of configuration file
    std::string config_file_name;
    // Name of venue configuration file
    std::string venue_config_file_name;
    // Are we publishing order book?
    bool is_publishing;
    // Address to publish order book data
    std::string publishing_addr;
    // Address of configuration server
    std::string config_server_addr;
    // Write fix message to logging system?
    bool is_logging;
    // Depth of book to subscribe to - FIX tag 264
    int32_t market_depth;
    //  FIX AggreagatedBook (266)
    bool want_aggregated_book;
    //  FIX MDUpdateType (265)
    int32_t update_type;
    //  FIX ResetSeqNumFlag (141)
    bool reset_seq_nums;
    //   Replace entirely existing orders with same id in orderbook?
    bool new_replaces;
};

class Application : public FIX::Application, public FIX::MessageCracker {
  public:
    explicit Application(const ApplicationConfig& config);
    ~Application();

    void run();

    void setZMQContext(void* c) { this->_pzmq_context = c;}
    void setZMQSocket(void* s) { this->_pzmq_socket = s;}
    void* getZMQSocket() { return this->_pzmq_socket;}
    void* getZMQContext() { return this->_pzmq_context;}

    void deleteBooks();

    void addSymbols(const std::vector<std::string>& symbols);
    const std::vector<std::string>& getSymbols();

  private:
    void addBook(const std::string& symbol, capk::KBook* book);
    capk::KBook*  getBook(const std::string& symbol);

    void setUpdateType(const int32_t updateType);

    void sendTestRequest();
    void sendSingleMarketDataRequest(const std::string& symbols);
    void sendMarketDataRequest(const std::vector<std::string>& symbols);

    template <typename T>
    void full_refresh_template(const T& message,
            const FIX::SessionID& sessionID);

    template <typename T>
    void incremental_update_template(const T& message,
            const FIX::SessionID& sessionID);

    template <typename T>
    void trading_session_status_template(const T& message,
            const FIX::SessionID& sessionID);

    /** 
     * FIX admin message callbacks
     */
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

    /** 
     * FIX 4.2 
     */
#ifdef USE_FIX_42
    FIX42::MarketDataRequest sendMarketDataRequest42(
        const std::vector<std::string>& symbols);
    FIX42::MarketDataRequest sendSingleMarketDataRequest42(
        const std::string& symbol);
    void onMessage(const FIX42::TradingSessionStatus&, const FIX::SessionID&);
    void onMessage(const FIX42::Logon&, const FIX::SessionID&);
    void onMessage(const FIX42::MarketDataSnapshotFullRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX42::MarketDataIncrementalRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX42::MarketDataRequestReject& message,
            const FIX::SessionID& sessionID);
    FIX42::TestRequest sendTestRequest42();
#endif //  USE_FIX_42

    /** 
     * FIX 4.3 
     */
#ifdef USE_FIX_43
    FIX43::MarketDataRequest sendMarketDataRequest43(
        const std::vector<std::string>& symbols);
    FIX43::MarketDataRequest sendSingleMarketDataRequest43(
        const std::string& symbol);
    void onMessage(const FIX43::TradingSessionStatus&, const FIX::SessionID&);
    void onMessage(const FIX43::Logon&, const FIX::SessionID&);
    void onMessage(const FIX43::MarketDataSnapshotFullRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX43::MarketDataIncrementalRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX43::MarketDataRequestReject& message,
            const FIX::SessionID& sessionID);
    FIX43::TestRequest sendTestRequest43();
#endif //  USE_FIX_43

    /** 
     * FIX 4.4 
     */
#ifdef USE_FIX_44
    FIX44::MarketDataRequest sendMarketDataRequest44(
        const std::vector<std::string>& symbols);
    FIX44::MarketDataRequest sendSingleMarketDataRequest44(
        const std::string& symbol);
    void onMessage(const FIX44::TradingSessionStatus&, const FIX::SessionID&);
    void onMessage(const FIX44::Logon&, const FIX::SessionID&);
    void onMessage(const FIX44::MarketDataSnapshotFullRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX44::MarketDataIncrementalRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX44::MarketDataRequestReject& message,
            const FIX::SessionID& sessionID);
    FIX44::TestRequest sendTestRequest44();
#endif //  USE_FIX_44

    /** 
     * FIX 5.0 
     */
#ifdef USE_FIX_50
    FIX50::MarketDataRequest sendMarketDataRequest50(
        const std::vector<std::string>& symbols);
    FIX50::MarketDataRequest sendSingleMarketDataRequest50(
        const std::string& symbol);
    void onMessage(const FIX50::MarketDataSnapshotFullRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX50::MarketDataIncrementalRefresh& message,
            const FIX::SessionID& sessionID);
#endif //  USE_FIX_50

    /** 
     * FIX 5.0SP2 
     */
#ifdef USE_FIX_50SP2
    FIX50SP2::MarketDataRequest sendMarketDataRequest50SP2(
        const std::vector<std::string>& symbols);
    FIX50SP2::MarketDataRequest sendSingleMarketDataRequest50SP2(
        const std::string& symbol);
    void onMessage(const FIX50SP2::MarketDataSnapshotFullRefresh& message,
            const FIX::SessionID& sessionID);
    void onMessage(const FIX50SP2::MarketDataIncrementalRefresh& message,
            const FIX::SessionID& sessionID);
#endif //   USE_FIX_50SP2

    /**
     * FIXT 11
     */
#ifdef USE_FIXT11 
    FIXT11::TestRequest sendTestRequestFIXT11();
    void onMessage(const FIXT11::Logon& logon, const FIX::SessionID& sessionID);
#endif //  defined(USE_FIX_50) || defined(USE_FIX_50SP2) 

    /**
     * Class variables
     */
    FIX::SessionID _sessionID;
    std::vector<std::string> _symbols;
    boost::filesystem::path _pathToLog;

    unsigned int _loginCount;
    unsigned int _appMsgCount;
    bool _resetSequence;
    const ApplicationConfig& _config;

    void* _pzmq_socket;
    void* _pzmq_context;

    std::map<std::string, capk::KBook* > _symbolToBook;
    typedef std::map<std::string, capk::KBook* >::iterator
        symbolToBookIterator;
};

#endif  // MARKET_DATA_COLLECT_FIX_APPLICATION_H_
