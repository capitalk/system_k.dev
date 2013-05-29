/* -*- C++ -*- */


#ifdef _MSC_VER
#pragma warning(disable : 4503 4355 4786 )
#endif

#include "md_parser_qf.h"
#include "md_broadcast.h"

#include <iostream>
#include <string>
#include <vector>

#include "quickfix/Session.h"
#include "quickfix/FieldConvertors.h"
#include "utils/time_utils.h"
#include "utils/types.h"
#include "utils/fix_convertors.h"
#include "utils/jenkins_hash.h"
#ifdef LOG
#include "utils/logging.h"
#endif

namespace fs = boost::filesystem;
namespace dt = boost::gregorian;

void printCrossedBookNotification(const char* book_name,
                                  double bbsize,
                                  double bbid,
                                  double basize,
                                  double bask) {
#ifdef LOG
    pan::log_INFORMATIONAL("Crossed Book: ",
        book_name,
        " (" ,
        pan::real(bbid),
        ", ",
        pan::real(bask),
        ")\n");
#endif

  std::cout << "Crossed book: "
    << book_name
    << " ("
    << static_cast<double>(bbsize)
    << "-"
    << static_cast<double>(bbid)
    <<  "@"
    << static_cast<double>(basize)
    << "-"
    << static_cast<double>(bask)
    << ")"
    << std::endl;
}

/**
 * Application constructor (Quickfix)
 */

Application::Application(const ApplicationConfig& config)
    :_loginCount(0),
    _appMsgCount(0),
    _config(config) {
#ifdef LOG
  pan::log_DEBUG("Application()");
#endif
}

/**
 * Application dtor (Quickfix)
 */
Application::~Application() {
  deleteBooks();
}

void Application::toAdmin(FIX::Message& message,
                          const FIX::SessionID& sessionID) {
  FIX::MsgType msgType;
  message.getHeader().getField(msgType);
  if (msgType.getValue() == FIX::MsgType_TestRequest) { 
#ifdef LOG
    if (_config.print_debug) {
      pan::log_DEBUG("Sending TestRequest (35=1)");
    }
#endif
  }
  if (msgType.getValue() == FIX::MsgType_ResendRequest) {  
#ifdef LOG
    if (_config.print_debug) {
      pan::log_DEBUG("Sending ResendRequest (35=2)");
    }
#endif
  }
  if (msgType.getValue() == FIX::MsgType_Reject) {  
#ifdef LOG
    if (_config.print_debug) {
      pan::log_ERROR("Sending Reject (35=3)");
    }
#endif
  }
  if (msgType.getValue() == FIX::MsgType_Heartbeat) {  
#ifdef LOG
    if (_config.print_debug) {
      pan::log_DEBUG("Sending Heartbeat (35=0)");
    }
#endif
  }
  if (msgType.getValue() == FIX::MsgType_SequenceReset) {  
#ifdef LOG
    if (_config.print_debug) {
      pan::log_WARNING("Sending SequenceReset (35=4)");
    }
#endif
  }
  if (msgType.getValue() == FIX::MsgType_Logon) {  
    FIX::Header& header = message.getHeader();

    //
    // Some exchanges want both a password and username,
    // put them in the fields designated by FIX4.3,
    // others want just a password in the FIX4.3 field 554
    // and others want a password in the raw data field
    //
    if (_config.username.length() > 0) {
      header.setField(FIX::FIELD::Username, _config.username);
    }

    if (_config.sendPasswordInRawDataField) {
      header.setField(FIX::RawData(_config.password.c_str()));
      header.setField(FIX::RawDataLength(_config.password.length()));
    } else {
      if (_config.password != "") {
        header.setField(FIX::FIELD::Password, _config.password);
      }
    }
    if (_config.reset_seq_nums) {
#ifdef LOG
      pan::log_DEBUG("Sending reset sequence number request (141=Y)");
#endif
      FIX::ResetSeqNumFlag flag = FIX::ResetSeqNumFlag_YES;
      message.setField(FIX::FIELD::ResetSeqNumFlag, "Y");
    }
  }

#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("toAdmin sent: ", message.toString());
  }
#endif
}

void Application::run() {
#ifdef LOG
  pan::log_DEBUG("Application::run()");
#endif

  std::vector<std::string>::const_iterator it = _symbols.begin();
  capk::KBook* pBook = NULL;
  std::string MIC_prefix =
    _config.mic_string.length() > 0 ? _config.mic_string + "_" : "";
  std::string symbol;
    
  /**
   * Create for each symbol:
   * 1) OrderBook
   * 2) Log file
   */
  while (it != _symbols.end() && *it != "") {
    //bool isRestart = false;
    symbol = *it;
    pBook = new capk::KBook(symbol.c_str(), _config.market_depth);
#ifdef LOG
    pan::log_DEBUG("Created new order book for symbol: ", symbol.c_str(),
                   " with depth: ", pan::integer(pBook->getDepth()));
#endif
    addBook(symbol, pBook);
    it++;
  }
  std::cerr << "Total books created: "
            << _symbolToBook.size() << "\n";
    
  // Some venues require each market data subscription to be sent
  // in a different message - i.e. when we send 35=V we are ONLY
  // allowed to send 146(NoRelatedSymbols)=1 so we send multiple
  // 35=V requests

  if (_config.sendIndividualMarketDataRequests) {
    for (std::vector<std::string>::const_iterator it = _symbols.begin();
         it != _symbols.end();
         it++) {
      if (*it != "") {
        sendSingleMarketDataRequest(*it);
      }
    }
  } else {
    // Send a single market data request (35=V) with all symbols
    sendMarketDataRequest(_symbols);
  }
}

void Application::addSymbols(const std::vector<std::string>& symbols) {
  _symbols = symbols;
}

const std::vector<std::string>& Application::getSymbols() {
  return _symbols;
}

void Application::addBook(const std::string& symbol, capk::KBook* book) {
  _symbolToBook[symbol] = book;
}

capk::KBook* Application::getBook(const std::string& symbol) {
  symbolToBookIterator it = _symbolToBook.find(symbol);
  if (it == _symbolToBook.end()) {
    return NULL;
  } else {
    return (it->second);
  }
}

void Application::deleteBooks() {
#ifdef LOG
  if (_config.print_debug)  {
    pan::log_DEBUG("Deleting books");
  }
#endif
  symbolToBookIterator books = _symbolToBook.begin();
  while (books != _symbolToBook.end()) {
    if (books->second) {
      delete books->second;
    }
    books++;
  }
}


void Application::sendTestRequest() {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("Application::sendTestRequest()");
  }
#endif

  FIX::Message testRequestMessage;
  switch (_config.version) {
#ifdef USE_FIX_42
  case FIX_42:
    testRequestMessage = sendTestRequest42();
    break;
#endif //  USE_FIX_42

#ifdef USE_FIX_43
  case FIX_43:
    testRequestMessage = sendTestRequest43();
    break;
#endif //  USE_FIX_43

#ifdef USE_FIX_44
  case FIX_44:
    testRequestMessage = sendTestRequest44();
    break;
#endif //  USE_FIX_44

#ifdef USE_FIX_50
  case FIX_50:
    testRequestMessage = sendTestRequestFIXT11();
    break;
#endif //  USE_FIX_50

#ifdef USE_FIX_50SP2
  case FIX_50SP2:
    testRequestMessage = sendTestRequestFIXT11();
    break;
#endif //  USE_FIX_50SP2
  default:
    throw std::runtime_error("Unsupported FIX version");
  }
  FIX::Session::sendToTarget(testRequestMessage,
                             _sessionID.getSenderCompID(),
                             _sessionID.getTargetCompID());
}

void Application::sendSingleMarketDataRequest(
  const std::string& requestSymbol) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("sendSingleMarketDataRequest(...)");
  }
#endif

  FIX::Message md;
  switch (_config.version) {
#ifdef USE_FIX_42
  case FIX_42:
    md = sendSingleMarketDataRequest42(requestSymbol);
    break;
#endif //  USE_FIX_42

#ifdef USE_FIX_43
  case FIX_43:
    md = sendSingleMarketDataRequest43(requestSymbol);
    break;
#endif //  USE_FIX_43

#ifdef USE_FIX_44
  case FIX_44:
    md = sendSingleMarketDataRequest44(requestSymbol);
    break;
#endif //  USE_FIX_44

#ifdef USE_FIX_50
  case FIX_50:
    md = sendSingleMarketDataRequest50(requestSymbol);
    break;
#endif //  USE_FIX_50

#ifdef USE_FIX_50SP2
  case FIX_50SP2:
    md = sendSingleMarketDataRequest50SP2(requestSymbol);
    break;
#endif //  USE_FIX_50PSP2
  default:
    throw std::runtime_error("Unsupported FIX version");
  }
  FIX::Session::sendToTarget(md);
}

void Application::sendMarketDataRequest(
  const std::vector<std::string>& symbols) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("Application::sendMarketDataRequest(...)");
  }
#endif
  FIX::Message md;
  switch (_config.version) {
#ifdef USE_FIX_42
  case FIX_42:
    md = sendMarketDataRequest42(symbols);
    break;
#endif //  USE_FIX_42

#ifdef USE_FIX_43
  case FIX_43:
    md = sendMarketDataRequest43(symbols);
    break;
#endif //  USE_FIX_43

#ifdef USE_FIX_44
  case FIX_44:
    md = sendMarketDataRequest44(symbols);
    break;
#endif // USE_FIX_44
    
#ifdef USE_FIX_50
  case FIX_50:
    md = sendMarketDataRequest50(symbols);
    break;
#endif //  USE_FIX_50

#ifdef USE_FIX_50SP2
  case FIX_50SP2:
    md = sendMarketDataRequest50SP2(symbols);
    break;
#endif //  USE_FIX_50SP2
  default:
    throw std::runtime_error("Unsupported FIX version");
  }
  FIX::Session::sendToTarget(md);
}

/**
 * Callbacks for FIX actions 
 */
void Application::onLogon(const FIX::SessionID& sessionID) {
#ifdef LOG
  pan::log_DEBUG("onLogon - session ID:", sessionID.toString().c_str());
#endif
  _sessionID = sessionID;
  _loginCount++;
  run();
}

void Application::onLogout(const FIX::SessionID& sessionID) {
#ifdef LOG
  pan::log_DEBUG("onLogout - session ID:", sessionID.toString().c_str());
#endif
}

void Application::onCreate(const FIX::SessionID& sessionID) {
#ifdef LOG
  pan::log_DEBUG("onCreate - session ID:", sessionID.toString().c_str());
#endif
}

void Application::fromAdmin(const FIX::Message& message,
                            const FIX::SessionID& sessionID)
throw(FIX::FieldNotFound,
      FIX::IncorrectDataFormat,
      FIX::IncorrectTagValue,
      FIX::RejectLogon ) {
#ifdef LOG
  if (_config.print_debug)  {
    pan::log_DEBUG("fromAdmin: ", message.toString().c_str());
  }
#endif
  crack(message, sessionID);
}

void Application::fromApp(const FIX::Message& message,
                          const FIX::SessionID& sessionID)
throw(FIX::FieldNotFound,
      FIX::IncorrectDataFormat,
      FIX::IncorrectTagValue,
      FIX::UnsupportedMessageType) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("fromApp: ", message.toString().c_str());
  }
#endif

  _appMsgCount++;
  if (_appMsgCount % 10000 == 0) {
    std::cout << "Received "
              << _appMsgCount
              << " application messages"
              << std::endl;
  }

  crack(message, sessionID);
}

void Application::toApp(FIX::Message& message,
                        const FIX::SessionID& sessionID)
throw(FIX::DoNotSend) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("toApp: ", message.toString().c_str());
  }
#endif

  try {
    // Don't send potenially duplicate requests for market data
    FIX::PossDupFlag possDupFlag;
    if (message.isSetField(possDupFlag)) {
      message.getHeader().getField(possDupFlag);
      if (possDupFlag) {
        throw FIX::DoNotSend();
      }
    }
  }
  catch(const FIX::FieldNotFound& e) {
#ifdef LOG
    pan::log_CRITICAL("FieldNotFound:\n",
                      e.what(), "\n",
                      e.detail.c_str(), "\n",
                      " (", pan::integer(e.field), ")");
#endif
    std::cerr << "FieldNotFound: "
              << e.what()
              << e.detail
              << " (" << e.field << ")"
              << std::endl;
  }
  catch(FIX::Exception& e) {
#ifdef LOG
    pan::log_CRITICAL("Exception: ", e.what());
#endif
    std::cerr << e.what() << std::endl;
  }
}

/**
 * Templates for handling common messages
 */
template <typename T> void Application::trading_session_status_template(const T& message,
    const FIX::SessionID& sessionID) {
  FIX::TradingSessionID tradingSessionID;
  FIX::TradSesStatus tradSesStatus;
  if (message.isSetField(tradingSessionID)) {
    message.getField(tradingSessionID);
  }
  if (message.isSetField(tradSesStatus)) {
    message.getField(tradSesStatus);
    if (tradSesStatus.getValue() == FIX::TradSesStatus_OPEN) {
#ifdef LOG
      pan::log_INFORMATIONAL("Trading sessions status is OPEN");
#endif
    }
   if (tradSesStatus.getValue() == FIX::TradSesStatus_HALTED) {
#ifdef LOG
      pan::log_INFORMATIONAL("Trading sessions status is HALTED");
#endif
    }
    if (tradSesStatus.getValue() == FIX::TradSesStatus_CLOSED) {
#ifdef LOG
      pan::log_INFORMATIONAL("Trading sessions status is CLOSED");
#endif
    }
    if (tradSesStatus.getValue() == FIX::TradSesStatus_PREOPEN) {
#ifdef LOG
      pan::log_INFORMATIONAL("Trading sessions status is PREOPEN");
#endif
    }
  }
}

template <typename T>
void Application::full_refresh_template(const T& message,
                                        const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("Application::full_refresh_template()",
                   message.toString().c_str());
  }
#endif
  
/*  std::ostream* pLog = NULL; */
  FIX::MsgType msgType;
  message.getHeader().getField(msgType);
  FIX::SendingTime sendingTime;
  message.getHeader().getField(sendingTime);
  FIX::NoMDEntries noMDEntries;
  FIX::MDReqID mdReqID;
  FIX::MDEntryType mdEntryType;
  FIX::QuoteType quoteType;
  FIX::MDEntryID mdEntryID;
  FIX::MDEntryPx mdEntryPx;
  FIX::MDEntrySize mdEntrySize;
  FIX::ExecInst execInst;
  FIX::QuoteEntryID quoteEntryID;
  FIX::MDEntryOriginator mdEntryOriginator;
  FIX::MinQty minQty;
  FIX::MDEntryPositionNo mdEntryPositionNo;
  FIX::MaturityMonthYear maturityMonthYear;
  FIX::Symbol symbol;
  FIX::SecurityType securityType;
  int nEntries = 0;
  capk::KBook* pBook = NULL;

  if (message.isSetField(symbol)) {
    message.getField(symbol);
  }
  if (message.isSetField(mdReqID)) {
    message.getField(mdReqID);
  }
  if (message.isSetField(noMDEntries)) {
    try {
      message.getField(noMDEntries);
      nEntries = noMDEntries.getValue();
    }
    catch(const std::exception& e) {
#ifdef LOG
      pan::log_DEBUG("Exception: ", e.what());
#endif
      std::cerr << e.what() << "\n";
    }
    if (nEntries > 0) {
      // clear the book since we have a complete refresh
      if (NULL != (pBook = getBook(symbol.getValue())))  {
        pBook->clear();
      }
    }

    typename T::NoMDEntries mdEntries;
    // NB: group indexed on 1 not 0
    for (int i = 0; i< nEntries; i++) {
      message.getGroup(i+1, mdEntries);
      if (mdEntries.isSetField(symbol)) {
        mdEntries.getField(symbol);
      }
      if (mdEntries.isSetField(mdEntryType)) {
        mdEntries.getField(mdEntryType);
      }

      // Quote condition is required for some ECNs
#ifdef USE_QUOTE_CONDITION
      FIX::QuoteCondition condition;
      if (mdEntries.isSetField(condition)) {
        mdEntries.getField(condition);
      }
#endif
      if (mdEntries.isSetField(mdEntryID)) {
        mdEntries.getField(mdEntryID);
      }
      if (mdEntries.isSetField(mdEntryPx)) {
        mdEntries.getField(mdEntryPx);
      }
      if (mdEntries.isSetField(mdEntrySize)) {
        mdEntries.getField(mdEntrySize);
      }
      if (mdEntries.isSetField(quoteType)) {
        mdEntries.getField(quoteType);
      }
      if (mdEntries.isSetField(mdEntryOriginator)) {
        mdEntries.getField(mdEntryOriginator);
      }
      if (mdEntries.isSetField(minQty)) {
        mdEntries.getField(minQty);
      }
      if (mdEntries.isSetField(mdEntryPositionNo)) {
        mdEntries.getField(mdEntryPositionNo);
      }
      if (mdEntries.isSetField(execInst)) {
        mdEntries.getField(execInst);
      }
      if (mdEntries.isSetField(quoteEntryID)) {
        mdEntries.getField(quoteEntryID);
      }

      FIX::UtcTimeStamp time = FIX::UtcTimeStamp(sendingTime);
      char side = mdEntryType.getValue();
      capk::Side_t nside = char2side_t(side);
      std::string id = mdEntryID.getValue();
      int nid = hashlittle(id.c_str(), id.size(), 0);


      // Since we sometimes don't get entry IDs in snapshots,
      // try using the quote entry ID instead

      if (id.length() == 0) {
        id = quoteEntryID.getValue();
        nid = hashlittle(id.c_str(), id.size(), 0);
      }
      double price = mdEntryPx.getValue();
      unsigned int size = mdEntrySize.getValue();

      timespec evtTime, sndTime;
      clock_gettime(CLOCK_MONOTONIC, &evtTime);
      FIXConvertors::UTCTimeStampToTimespec(time, &sndTime);
      // Add to orderbook
      pBook->add(nid, nside, size, price, evtTime, sndTime);

#ifdef LOG
      if (_config.print_debug) {
        pan::log_DEBUG("Adding ID=",
                       id.c_str(),
                       " price=",
                       pan::real(price),
                       " size=",
                       pan::integer(size));
      }
#endif
    }

    // At this point all entries in the message are processed
    // i.e. added to the book. We can now broadcast BBO and
    // Broadcast and log orderbook
    double bbid = pBook->bestPrice(capk::BID);
    double bask = pBook->bestPrice(capk::ASK);
    double bbsize = pBook->bestPriceVolume(capk::BID);
    double basize = pBook->bestPriceVolume(capk::ASK);
    if (bbid > bask) {
      printCrossedBookNotification(pBook->getName(),
                                   bbsize,
                                   bbid,
                                   basize,
                                   bask);
    }

    ptime time_start(microsec_clock::local_time());
    capk::broadcast_bbo_book(_pzmq_socket,
                       symbol.getValue().c_str(),
                       bbid,
                       bask,
                       bbsize,
                       basize,
                       _config.venue_id);
  }
}

/*
 * Incremental refresh message - started once initial orderbook has been built
 * This is the FIX 35=X message
 */
template <typename T>
void Application::incremental_update_template(const T& message,
    const FIX::SessionID& sessionID) {
  FIX::MsgType msgType;
  message.getHeader().getField(msgType);
  FIX::SendingTime sendingTime;
  message.getHeader().getField(sendingTime);
  FIX::MDReqID mdReqID;
  FIX::NoMDEntries noMDEntries;

  FIX::MDUpdateAction mdUpdateAction;
  FIX::MDEntryType mdEntryType;
  FIX::MDEntryID mdEntryID;
  FIX::MDEntryRefID mdEntryRefID;
  FIX::Symbol symbol;
  FIX::SecurityType securityType;
  FIX::MaturityMonthYear maturityMonthYear;
  FIX::MDEntryPx mdEntryPx;
  FIX::MDEntrySize mdEntrySize;
  FIX::ExecInst execInst;
  FIX::QuoteEntryID quoteEntryID;
  FIX::MDEntryPositionNo mdEntryPositionNo;
  FIX::SecurityExchange securityExchange;
  int nEntries = 0;
  capk::KBook* pBook = NULL;
/* std::ostream* pLog = NULL; */

  static double dbar, dcount, dsum;

  if (message.isSetField(mdReqID)) {
    message.getField(mdReqID);
  }

  if (message.isSetField(symbol)) {
    message.getField(symbol);
  } else {
#ifdef LOG
    pan::log_WARNING("SYMBOL field not set in BODY of "
                     "MarketDataIncrementalRefresh msg (35=X)");
#endif
  }

  if (message.isSetField(noMDEntries)) {
    message.getField(noMDEntries);
    nEntries = noMDEntries.getValue();

    typename T::NoMDEntries mdEntries;
    for (int i = 0; i< nEntries; i++) {
      message.getGroup(i+1, mdEntries);
      if (mdEntries.isSetField(mdUpdateAction)) {
        mdEntries.getField(mdUpdateAction);
      }
      if (mdEntries.isSetField(mdEntryType)) {
        mdEntries.getField(mdEntryType);
      }
      if (mdEntries.isSetField(mdEntryID)) {
        mdEntries.getField(mdEntryID);
      } else {
#ifdef LOG
        pan::log_WARNING("MDENTRYID field not set in BODY of "
                         "MarketDataIncrementalRefresh msg (35=X)");
#endif
      }
      if (mdEntries.isSetField(symbol)) {
        mdEntries.getField(symbol);
      } else {
#ifdef LOG
        pan::log_WARNING("SYMBOL field not set in FIELD of "
                         "MarketDataIncrementalRefresh msg (35=X)");
#endif
      }
      if (mdEntries.isSetField(securityType)) {
        mdEntries.getField(securityType);
      }
      if (mdEntries.isSetField(maturityMonthYear)) {
        mdEntries.getField(maturityMonthYear);
      }
      if (mdEntries.isSetField(mdEntryPx)) {
        mdEntries.getField(mdEntryPx);
      } else {
#ifdef LOG
        pan::log_WARNING("MdEntryPx field not set in FIELD of "
                         "MarketDataIncrementalRefresh msg (35=X)");
#endif
      }
      if (mdEntries.isSetField(mdEntrySize)) {
        mdEntries.getField(mdEntrySize);
      } else {
        // KTK - Added to support MDEntrySize not being set
        // when a delete is sent without a size
        mdEntrySize = FIX::MDEntrySize(0);
      }
      if (mdEntries.isSetField(execInst)) {
        mdEntries.getField(execInst);
      }
      if (mdEntries.isSetField(mdEntryPositionNo)) {
        mdEntries.getField(mdEntryPositionNo);
      }
      if (mdEntries.isSetField(securityExchange)) {
        mdEntries.getField(securityExchange);
      }

      unsigned int size;
      timespec evtTime, sndTime;
      FIX::UtcTimeStamp time = FIX::UtcTimeStamp(sendingTime);
      FIXConvertors::UTCTimeStampToTimespec(time, &sndTime);
      clock_gettime(CLOCK_MONOTONIC, &evtTime);
/*      pLog = getStream(symbol.getValue()); */

      if (NULL != (pBook = getBook(symbol.getValue())))  {
        char side = mdEntryType.getValue();
        capk::Side_t nside = char2side_t(side);
        double price = 0.0;
        size = mdEntrySize.getValue();

        const std::string& id = mdEntryID.getValue();

        uint32_t nid = hashlittle(id.c_str(), id.size(), 0);
#ifdef LOG
        if (_config.print_debug) {
          pan::log_DEBUG("Original MDEntryID: ",
                         id.c_str(),
                         " => HASH ID: ",
                         pan::integer(nid));
        }
#endif
        int action = mdUpdateAction.getValue();
        if (action == FIX::MDUpdateAction_NEW) {
          price = mdEntryPx.getValue();
          if (_config.new_replaces) {
#ifdef LOG
            if (_config.print_debug) {
              pan::log_DEBUG("Deleting old: ",
                             pan::integer(nid),
                             " before adding new "
                             "(new_replaces enabled in conifg)");
            }
#endif
            int removeOk = pBook->remove(nid, evtTime, sndTime);
            if (removeOk == 0) {
              std::cerr << "Order: " 
                  << nid 
                  << "not found - so just adding";
            }
#ifdef LOG
            if (_config.print_debug) {
              if (removeOk == 0) {
                pan::log_DEBUG("Order: ",
                               pan::integer(nid),
                               " not found - so just add");
              }
              pan::log_DEBUG("Adding: ",
                             pan::integer(nid),
                             ", ",
                             pan::integer(size),
                             "@",
                             pan::real(price));
            }
#endif
        }

          if (pBook->add(nid,
                         nside,
                         size,
                         price,
                         evtTime,
                         sndTime) !=1) {
#ifdef LOG
            pan::log_WARNING("Book add failed",
                             pan::integer(nid),
                             " ",
                             pan::integer(side),
                             " ",
                             pan::integer(size),
                             " ",
                             pan::real(price));
#endif
            std::cerr << "WARNING: Book add failed\n ("
                      << nid << " "
                      << nside << " "
                      << side << " "
                      << size << " "
                      << price << " "
                      << evtTime << " "
                      << sndTime << " "
                      << "\n";
          }
#ifdef LOG
          if (_config.print_debug) {
            pan::log_DEBUG("Add ",
                           pan::integer(nside), ",",
                           pan::integer(size), ",",
                           pan::real(price), ",", "\n");
            pBook->dbg();
          }
#endif
        } else if (action == FIX::MDUpdateAction_CHANGE) {
          price = mdEntryPx.getValue();
          size = mdEntrySize.getValue();

          // If the message contains a MDEntryRefID
          // then we're renaming an existing entity so we delete
          // the old one (with MDEntryRefID and
          // re-add with mdEntryID)

          if (mdEntries.isSetField(mdEntryRefID)) {
            mdEntries.getField(mdEntryRefID);
            const std::string& refId = mdEntryRefID.getValue();
#ifdef LOG
            if (_config.print_debug) {
              pan::log_WARNING("Using mdEntryRefID: ",
                               mdEntryRefID.getString().c_str(),
                               " AS mdEntryID: ",
                               mdEntryID.getString().c_str());

              if (mdEntryRefID.getValue() != mdEntryID.getValue()) {
                pan::log_WARNING("mdEntryRefID != mdEntryID");
              }
            }
#endif
            uint32_t nrefId = hashlittle(refId.c_str(), refId.size(), 0);
            pBook->remove(nrefId, evtTime, sndTime);
            pBook->add(nid, nside, size, price, evtTime, sndTime);
          } else {
#ifdef LOG
            if (_config.print_debug) {
              pan::log_DEBUG("Modify: ",
                             pan::integer(nid),
                             " to size ",
                             pan::integer(size));
            }
#endif
            capk::pKOrder pOrder = pBook->getOrder(nid);
            // If we found the order referenced by mdEntryRefID
            // update it with new information
            if (pOrder) {
#ifdef LOG
              if (_config.print_debug) {
                pan::log_DEBUG("Modify ",
                               pan::integer(nside), ",",
                               pan::integer(size), ",",
                               pan::real(price), ",", "\n");
              }
#endif
              int modifyOk = pBook->modify(nid, size, evtTime, sndTime);

              if (modifyOk != 1) {
#ifdef LOG
                pan::log_WARNING("Modify failed. MdEntryID: ",
                                 pan::integer(nid),
                                 " does not exist in book");
#endif
                std::cerr << "Modify failed. MdEntryID: "
                          << nid
                          << " does not exist in book\n";
              }
            } else {
#ifdef LOG
              pan::log_WARNING("Can't find orderID: ",
                               pan::integer(nid),
                               " for original id: ",
                               id.c_str());
#endif

              std::cerr << "(M) CAN'T FIND ORDERID: "
                        << nid
                        << " for orig_id: "
                        << id
                        << "\n"
                        << *pBook;
            }
          }
        } else if (action == FIX::MDUpdateAction_DELETE) {
#ifdef LOG
          if (_config.print_debug) {
            pan::log_DEBUG("Delete: ",
                           pan::integer(nid));
          }
#endif
          capk::pKOrder pOrder = pBook->getOrder(nid);
          if (pOrder) {
#ifdef LOG
            if (_config.print_debug) {
              pan::log_DEBUG("Delete ",
                             pan::integer(nside), ",",
                             pan::integer(size), ",",
                             pan::real(price), ",", "\n");
            }
#endif
            int removeOk = pBook->remove(nid, evtTime, sndTime);
            if (removeOk != 1) {
              std::cerr << "Delete failed. MdEntryID: "
                        << nid
                        << " does not exist in book\n";
#ifdef LOG
              pan::log_ERROR("Delete failed. MdEntryID: ",
                             pan::integer(nid),
                             " does not exist in book");
#endif
            }
          } else {
            std::cerr << "(D) CAN'T FIND ORDERID: "
                      << nid
                      << " for orig_id: "
                      << id
                      << "\n";

            pBook->dbg();
            assert(0);
          }

        } else {
          std::cerr << __FILE__
                    << ":"
                    <<  __LINE__
                    << "Unknown action type: "
                    << action
                    << "\n";
          exit(1);
        }
      } else {
        std::cerr << __FILE__
                  << ":"
                  << __LINE__
                  << "Can't find orderbook - book is null!"
                  << "\n";
      }
    }

    // Broadcast and log orderbook
    double bbid = pBook->bestPrice(capk::BID);
    double bask = pBook->bestPrice(capk::ASK);
    double bbsize = pBook->bestPriceVolume(capk::BID);
    double basize = pBook->bestPriceVolume(capk::ASK);

    if (bbid > bask) {
      printCrossedBookNotification(pBook->getName(),
                                   bbsize,
                                   bbid,
                                   basize,
                                   bask);

    }

    if (_config.is_publishing) {
      ptime time_start(microsec_clock::local_time());
      capk::broadcast_bbo_book(_pzmq_socket,
                         symbol.getValue().c_str(),
                         bbid,
                         bask,
                         bbsize,
                         basize,
                         _config.venue_id);
      // Timing stats
      ptime time_end(microsec_clock::local_time());
      time_duration duration(time_end - time_start);
      dsum += duration.total_microseconds();
      dcount++;
      dbar = dsum / dcount;
      std::cout << "Mean time(us) to broadcast and write log: " << dbar << "\n";
    }
  }
}


/******************************************************************************
 * FIX 42 
 *****************************************************************************/
#ifdef USE_FIX_42

FIX42::MarketDataRequest Application::sendSingleMarketDataRequest42(
  const std::string& requestSymbol) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("sendSingleMarketDataRequest42(",
                   requestSymbol.c_str(),
                   ")");
  }
#endif

  std::string reqID("CAPK-");
  reqID += requestSymbol;
  FIX::MDReqID mdReqID(reqID.c_str());

  FIX::SubscriptionRequestType
  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
  // KTK TODO - pull the depth into symbols file
  FIX::MarketDepth marketDepth(_config.market_depth);
  FIX42::MarketDataRequest message(mdReqID, subType, marketDepth);

  if (_config.want_aggregated_book) {
    message.set(FIX::AggregatedBook(true));
  }

  FIX42::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
  // Set bid request
  FIX::MDEntryType mdEntryTypeBid(FIX::MDEntryType_BID);
  marketDataEntryGroup.set(mdEntryTypeBid);
  message.addGroup(marketDataEntryGroup);

  // Set ask request
  FIX::MDEntryType mdEntryTypeOffer(FIX::MDEntryType_OFFER);
  marketDataEntryGroup.set(mdEntryTypeOffer);
  message.addGroup(marketDataEntryGroup);

  // Set symbols to subscribe to
  // IN THIS CASE IT MUST BE ONLY ONE SYMBOL!!!
  FIX42::MarketDataRequest::NoRelatedSym symbolGroup;
  FIX::Symbol symbol(requestSymbol);
  symbolGroup.set(symbol);
  message.addGroup(symbolGroup);

  FIX::MDUpdateType updateType(_config.update_type);
  message.set(updateType);

  message.getHeader().setField(_sessionID.getSenderCompID());
  message.getHeader().setField(_sessionID.getTargetCompID());

  return message;
}


FIX42::MarketDataRequest Application::sendMarketDataRequest42(
  const std::vector<std::string>& symbols) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("sendMarketDataRequest42(...)");
  }
#endif

  FIX::MDReqID mdReqID("CAPK");

  FIX::SubscriptionRequestType
  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
  FIX::MarketDepth marketDepth(_config.market_depth);
  FIX42::MarketDataRequest message(mdReqID, subType, marketDepth);

  if (_config.want_aggregated_book) {
    message.set(FIX::AggregatedBook(true));
  }

  FIX42::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
  // Set bid request
  FIX::MDEntryType mdEntryTypeBid(FIX::MDEntryType_BID);
  marketDataEntryGroup.set(mdEntryTypeBid);
  message.addGroup(marketDataEntryGroup);

  // Set ask request
  FIX::MDEntryType mdEntryTypeOffer(FIX::MDEntryType_OFFER);
  marketDataEntryGroup.set(mdEntryTypeOffer);
  message.addGroup(marketDataEntryGroup);

  // Set symbols to subscribe to
  FIX42::MarketDataRequest::NoRelatedSym symbolGroup;
  for (std::vector<std::string>::const_iterator it = symbols.begin();
       it != symbols.end();
       ++it) {
    if (*it != "") {
      FIX::Symbol symbol(*it);
      symbolGroup.set(symbol);
      message.addGroup(symbolGroup);
    }
  }

  FIX::MDUpdateType updateType(_config.update_type);
  message.set(updateType);

  message.getHeader().setField(_sessionID.getSenderCompID());
  message.getHeader().setField(_sessionID.getTargetCompID());

  return message;
}


void Application::onMessage(const FIX42::TradingSessionStatus& message,
                            const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX42::TradingSessionStatus& message...)");
  }
#endif
  trading_session_status_template<FIX42::TradingSessionStatus>(message,
      sessionID);
}

void Application::onMessage(const FIX42::Logon& message,
                            const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX42::Logon& message, ...)");
  }
#endif
}

void Application::onMessage(const FIX42::MarketDataSnapshotFullRefresh& message,
                            const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX42::MarketDataSnapshotFullRefresh& message, ...",
                   "\n(", message.toString().c_str(), ")");
  }
#endif
  this->full_refresh_template<FIX42::MarketDataSnapshotFullRefresh>(message,
      sessionID);
}


void Application::onMessage(const FIX42::MarketDataIncrementalRefresh& message,
  const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX42::MarketDataIncrementalRefresh& message, ...)\n",
                   message.toString().c_str());
  }
#endif
  this->incremental_update_template<FIX42::MarketDataIncrementalRefresh>(
    message, sessionID);
}

void Application::onMessage(const FIX42::MarketDataRequestReject& message,
  const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX42::MarketDataRequestReject& message, ...)\n",
                   message.toString().c_str());
  }
#endif
}

FIX42::TestRequest Application::sendTestRequest42() {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("Application::sendTestRequest42()");
  }
#endif
  FIX42::TestRequest tr;
  FIX::TestReqID trid("TestRequest");
  tr.setField(trid);
  return tr;
}

#endif // USE_FIX_42

/******************************************************************************
 * FIX 43
 *****************************************************************************/
#ifdef USE_FIX_43

FIX43::MarketDataRequest Application::sendMarketDataRequest43(
  const std::vector<std::string>& symbols) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("sendMarketDataRequest43(...)");
  }
#endif

  FIX::MDReqID mdReqID("CAPK");

  FIX::SubscriptionRequestType
  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
  FIX::MarketDepth marketDepth(_config.market_depth);
  FIX43::MarketDataRequest message(mdReqID, subType, marketDepth);

  if (_config.want_aggregated_book) {
    message.set(FIX::AggregatedBook(true));
  }

  FIX43::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
  // Set bid request
  FIX::MDEntryType mdEntryTypeBid(FIX::MDEntryType_BID);
  marketDataEntryGroup.set(mdEntryTypeBid);
  message.addGroup(marketDataEntryGroup);

  // Set ask request
  FIX::MDEntryType mdEntryTypeOffer(FIX::MDEntryType_OFFER);
  marketDataEntryGroup.set(mdEntryTypeOffer);
  message.addGroup(marketDataEntryGroup);

  // Set symbols to subscribe to
  FIX43::MarketDataRequest::NoRelatedSym symbolGroup;
  for (std::vector<std::string>::const_iterator it = symbols.begin();
       it != symbols.end();
       ++it) {
    if (*it != "") {
      FIX::Symbol symbol(*it);
      symbolGroup.set(symbol);
      message.addGroup(symbolGroup);
    }
  }

  FIX::MDUpdateType updateType(_config.update_type);
  message.set(updateType);

  message.getHeader().setField(_sessionID.getSenderCompID());
  message.getHeader().setField(_sessionID.getTargetCompID());

  return message;
}

FIX43::MarketDataRequest Application::sendSingleMarketDataRequest43(
  const std::string& requestSymbol) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("sendSingleMarketDataRequest43(",
                   requestSymbol.c_str(),
                   ")");
  }
#endif

  std::string reqID("CAPK-");
  reqID += requestSymbol;
  FIX::MDReqID mdReqID(reqID.c_str());

  FIX::SubscriptionRequestType
  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
  // KTK TODO - pull the depth into symbols file
  FIX::MarketDepth marketDepth(_config.market_depth);
  FIX43::MarketDataRequest message(mdReqID, subType, marketDepth);

  if (_config.want_aggregated_book) {
    message.set(FIX::AggregatedBook(true));
  }

  FIX43::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
  // Set bid request
  FIX::MDEntryType mdEntryTypeBid(FIX::MDEntryType_BID);
  marketDataEntryGroup.set(mdEntryTypeBid);
  message.addGroup(marketDataEntryGroup);

  // Set ask request
  FIX::MDEntryType mdEntryTypeOffer(FIX::MDEntryType_OFFER);
  marketDataEntryGroup.set(mdEntryTypeOffer);
  message.addGroup(marketDataEntryGroup);

  // Set symbols to subscribe to
  // IN THIS CASE IT MUST BE ONLY ONE SYMBOL!!!
  FIX43::MarketDataRequest::NoRelatedSym symbolGroup;
  FIX::Symbol symbol(requestSymbol);
  symbolGroup.set(symbol);
  message.addGroup(symbolGroup);

  FIX::MDUpdateType updateType(_config.update_type);
  message.set(updateType);

  message.getHeader().setField(_sessionID.getSenderCompID());
  message.getHeader().setField(_sessionID.getTargetCompID());

  return message;
}

void Application::onMessage(const FIX43::TradingSessionStatus& message,
                            const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX43::TradingSessionStatus& message...)");
  }
#endif
  trading_session_status_template<FIX43::TradingSessionStatus>(message,
      sessionID);
}

void Application::onMessage(const FIX43::Logon& message,
                            const FIX::SessionID& sessionID) {
  if (_config.print_debug) {
#ifdef LOG
    pan::log_DEBUG("FIX43::Logon& message, ...)");
#endif
  }
}

void Application::onMessage(const FIX43::MarketDataSnapshotFullRefresh& message,
                            const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX43::MarketDataSnapshotFullRefresh& message, ...",
                   "\n(", message.toString().c_str(), ")");
  }
#endif
  this->full_refresh_template<FIX43::MarketDataSnapshotFullRefresh>(message,
      sessionID);
}

void Application::onMessage(const FIX43::MarketDataIncrementalRefresh& message,
  const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX43::MarketDataIncrementalRefresh& message, ...)\n",
                   message.toString().c_str());
  }
#endif
  this->incremental_update_template<FIX43::MarketDataIncrementalRefresh>(
    message, sessionID);
}

void Application::onMessage(const FIX43::MarketDataRequestReject& message,
  const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX43::MarketDataRequestReject& message, ...)\n",
                   message.toString().c_str());
  }
#endif
}


FIX43::TestRequest Application::sendTestRequest43() {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("Application::sendTestRequest43()");
  }
#endif
  FIX43::TestRequest tr;
  FIX::TestReqID trid("TestRequest");
  tr.setField(trid);
  return tr;
}

#endif // USE_FIX_43

/******************************************************************************
 * FIX 44
 *****************************************************************************/
#ifdef USE_FIX_44

FIX44::MarketDataRequest Application::sendMarketDataRequest44(
  const std::vector<std::string>& symbols) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("sendMarketDataRequest44(...)");
  }
#endif

  FIX::MDReqID mdReqID("CAPK");

  FIX::SubscriptionRequestType
  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
  FIX::MarketDepth marketDepth(_config.market_depth);
  FIX44::MarketDataRequest message(mdReqID, subType, marketDepth);

  if (_config.want_aggregated_book) {
    message.set(FIX::AggregatedBook(true));
  }

  FIX44::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
  // Set bid request
  FIX::MDEntryType mdEntryTypeBid(FIX::MDEntryType_BID);
  marketDataEntryGroup.set(mdEntryTypeBid);
  message.addGroup(marketDataEntryGroup);

  // Set ask request
  FIX::MDEntryType mdEntryTypeOffer(FIX::MDEntryType_OFFER);
  marketDataEntryGroup.set(mdEntryTypeOffer);
  message.addGroup(marketDataEntryGroup);

  // Set symbols to subscribe to
  FIX44::MarketDataRequest::NoRelatedSym symbolGroup;
  for (std::vector<std::string>::const_iterator it = symbols.begin();
       it != symbols.end();
       ++it) {
    if (*it != "") {
      FIX::Symbol symbol(*it);
      symbolGroup.set(symbol);
      message.addGroup(symbolGroup);
    }
  }

  FIX::MDUpdateType updateType(_config.update_type);
  message.set(updateType);

  message.getHeader().setField(_sessionID.getSenderCompID());
  message.getHeader().setField(_sessionID.getTargetCompID());

  return message;
}

FIX44::MarketDataRequest Application::sendSingleMarketDataRequest44(
  const std::string& requestSymbol) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("sendSingleMarketDataRequest44(",
                   requestSymbol.c_str(),
                   ")");
  }
#endif

  std::string reqID("CAPK-");
  reqID += requestSymbol;
  FIX::MDReqID mdReqID(reqID.c_str());

  FIX::SubscriptionRequestType
  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
  // KTK TODO - pull the depth into symbols file
  FIX::MarketDepth marketDepth(_config.market_depth);
  FIX44::MarketDataRequest message(mdReqID, subType, marketDepth);

  if (_config.want_aggregated_book) {
    message.set(FIX::AggregatedBook(true));
  }

  FIX44::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
  // Set bid request
  FIX::MDEntryType mdEntryTypeBid(FIX::MDEntryType_BID);
  marketDataEntryGroup.set(mdEntryTypeBid);
  message.addGroup(marketDataEntryGroup);

  // Set ask request
  FIX::MDEntryType mdEntryTypeOffer(FIX::MDEntryType_OFFER);
  marketDataEntryGroup.set(mdEntryTypeOffer);
  message.addGroup(marketDataEntryGroup);

  // Set symbols to subscribe to
  // IN THIS CASE IT MUST BE ONLY ONE SYMBOL!!!
  FIX44::MarketDataRequest::NoRelatedSym symbolGroup;
  FIX::Symbol symbol(requestSymbol);
  symbolGroup.set(symbol);
  message.addGroup(symbolGroup);

  FIX::MDUpdateType updateType(_config.update_type);
  message.set(updateType);

  message.getHeader().setField(_sessionID.getSenderCompID());
  message.getHeader().setField(_sessionID.getTargetCompID());

  return message;
}

void Application::onMessage(const FIX44::TradingSessionStatus& message,
                            const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX44::TradingSessionStatus& message...)");
  }
#endif
  trading_session_status_template<FIX44::TradingSessionStatus>(message,
      sessionID);
}

void Application::onMessage(const FIX44::Logon& message,
                            const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX44::Logon& message, ...)");
  }
#endif
}

void Application::onMessage(const FIX44::MarketDataSnapshotFullRefresh& message,
                            const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX44::MarketDataSnapshotFullRefresh& message, ...)",
                   "\n(", message.toString().c_str(), ")");
  }
#endif
  this->full_refresh_template<FIX44::MarketDataSnapshotFullRefresh>(message,
      sessionID);
}

void Application::onMessage(const FIX44::MarketDataIncrementalRefresh& message,
  const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX44::MarketDataIncrementalRefresh& message, ...)\n",
                   message.toString().c_str());
  }
#endif
  this->incremental_update_template<FIX44::MarketDataIncrementalRefresh>(
    message, sessionID);
}

void Application::onMessage(const FIX44::MarketDataRequestReject& message,
  const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX44::MarketDataRequestReject& message, ...)\n",
                   message.toString().c_str());
  }
#endif
}

FIX44::TestRequest Application::sendTestRequest44() {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("Application::sendTestRequest44()");
  }
#endif
  FIX44::TestRequest tr;
  FIX::TestReqID trid("TestRequest");
  tr.setField(trid);
  return tr;
}

#endif // USE_FIX_44

/******************************************************************************
 * FIX 50
 *****************************************************************************/
#ifdef USE_FIX_50
FIX50::MarketDataRequest Application::sendMarketDataRequest50(
  const std::vector<std::string>& symbols) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("sendMarketDataRequest50(...)");
  }
#endif

  FIX::MDReqID mdReqID("CAPK");

  FIX::SubscriptionRequestType
  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
  FIX::MarketDepth marketDepth(_config.market_depth);
  FIX50::MarketDataRequest message(mdReqID, subType, marketDepth);

  if (_config.want_aggregated_book) {
    message.set(FIX::AggregatedBook(true));
  }

  FIX50::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
  // Set bid request
  FIX::MDEntryType mdEntryTypeBid(FIX::MDEntryType_BID);
  marketDataEntryGroup.set(mdEntryTypeBid);
  message.addGroup(marketDataEntryGroup);

  // Set ask request
  FIX::MDEntryType mdEntryTypeOffer(FIX::MDEntryType_OFFER);
  marketDataEntryGroup.set(mdEntryTypeOffer);
  message.addGroup(marketDataEntryGroup);

  // Set symbols to subscribe to
  FIX50::MarketDataRequest::NoRelatedSym symbolGroup;
  for (std::vector<std::string>::const_iterator it = symbols.begin();
       it != symbols.end();
       ++it) {
    if (*it != "") {
      FIX::Symbol symbol(*it);
      symbolGroup.set(symbol);
      message.addGroup(symbolGroup);
    }
  }

  FIX::MDUpdateType updateType(_config.update_type);
  message.set(updateType);

  message.getHeader().setField(_sessionID.getSenderCompID());
  message.getHeader().setField(_sessionID.getTargetCompID());

  return message;
}

FIX50::MarketDataRequest Application::sendSingleMarketDataRequest50(
  const std::string& requestSymbol) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("sendSingleMarketDataRequest50(",
                   requestSymbol.c_str(),
                   ")");
  }
#endif
  std::string reqID("CAPK-");
  reqID += requestSymbol;
  FIX::MDReqID mdReqID(reqID.c_str());

  FIX::SubscriptionRequestType
  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
  FIX::MarketDepth marketDepth(_config.market_depth);
  FIX50::MarketDataRequest message(mdReqID, subType, marketDepth);

  if (_config.want_aggregated_book) {
    message.set(FIX::AggregatedBook(true));
  }

  FIX50::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
  // Set bid request
  FIX::MDEntryType mdEntryTypeBid(FIX::MDEntryType_BID);
  marketDataEntryGroup.set(mdEntryTypeBid);
  message.addGroup(marketDataEntryGroup);

  // Set ask request
  FIX::MDEntryType mdEntryTypeOffer(FIX::MDEntryType_OFFER);
  marketDataEntryGroup.set(mdEntryTypeOffer);
  message.addGroup(marketDataEntryGroup);

  // Set symbols to subscribe to
  // IN THIS CASE IT MUST BE ONLY ONE SYMBOL!!!
  FIX50::MarketDataRequest::NoRelatedSym symbolGroup;
  FIX::Symbol symbol(requestSymbol);
  symbolGroup.set(symbol);
  symbolGroup.set(FIX::SecurityType("FOR"));
  message.addGroup(symbolGroup);

  FIX::MDUpdateType updateType(_config.update_type);
  message.set(updateType);

  message.getHeader().setField(_sessionID.getSenderCompID());
  message.getHeader().setField(_sessionID.getTargetCompID());

  return message;
}

void Application::onMessage(const FIX50::MarketDataSnapshotFullRefresh& message,
                            const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX50::MarketDataSnapshotFullRefresh& message, ...)",
                   "\n(", message.toString().c_str(), ")");
  }
#endif
  this->full_refresh_template<FIX50::MarketDataSnapshotFullRefresh>(message,
      sessionID);
}

void Application::onMessage(const FIX50::MarketDataIncrementalRefresh& message,
  const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX50::MarketDataIncrementalRefresh& message, ...)\n",
                   message.toString().c_str());
  }
#endif
  this->incremental_update_template<FIX50::MarketDataIncrementalRefresh>(
    message, sessionID);
}

#endif // USE_FIX_50


/******************************************************************************
 * FIX 50SP2
 *****************************************************************************/
#ifdef USE_FIX_50SP2
FIX50SP2::MarketDataRequest Application::sendMarketDataRequest50SP2(
  const std::vector<std::string>& symbols) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("sendMarketDataRequest50SP2(...)");
  }
#endif

  FIX::MDReqID mdReqID("CAPK");

  FIX::SubscriptionRequestType
  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
  FIX::MarketDepth marketDepth(_config.market_depth);
  FIX50SP2::MarketDataRequest message(mdReqID, subType, marketDepth);

  if (_config.want_aggregated_book) {
    message.set(FIX::AggregatedBook(true));
  }

  FIX50SP2::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
  // Set bid request
  FIX::MDEntryType mdEntryTypeBid(FIX::MDEntryType_BID);
  marketDataEntryGroup.set(mdEntryTypeBid);
  message.addGroup(marketDataEntryGroup);

  // Set ask request
  FIX::MDEntryType mdEntryTypeOffer(FIX::MDEntryType_OFFER);
  marketDataEntryGroup.set(mdEntryTypeOffer);
  message.addGroup(marketDataEntryGroup);

  // Set symbols to subscribe to
  FIX50SP2::MarketDataRequest::NoRelatedSym symbolGroup;
  for (std::vector<std::string>::const_iterator it = symbols.begin();
       it != symbols.end();
       ++it) {
    if (*it != "") {
      FIX::Symbol symbol(*it);
      symbolGroup.set(symbol);
      message.addGroup(symbolGroup);
    }
  }

  FIX::MDUpdateType updateType(_config.update_type);
  message.set(updateType);

  message.getHeader().setField(_sessionID.getSenderCompID());
  message.getHeader().setField(_sessionID.getTargetCompID());

  return message;
}

FIX50SP2::MarketDataRequest Application::sendSingleMarketDataRequest50SP2(
  const std::string& requestSymbol) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("sendSingleMarketDataRequest50SP2(",
                   requestSymbol.c_str(),
                   ")");
  }
#endif
  std::string reqID("CAPK-");
  reqID += requestSymbol;
  FIX::MDReqID mdReqID(reqID.c_str());

  FIX::SubscriptionRequestType
  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
  FIX::MarketDepth marketDepth(_config.market_depth);
  FIX50SP2::MarketDataRequest message(mdReqID, subType, marketDepth);

  if (_config.want_aggregated_book) {
    message.set(FIX::AggregatedBook(true));
  }

  FIX50SP2::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
  // Set bid request
  FIX::MDEntryType mdEntryTypeBid(FIX::MDEntryType_BID);
  marketDataEntryGroup.set(mdEntryTypeBid);
  message.addGroup(marketDataEntryGroup);

  // Set ask request
  FIX::MDEntryType mdEntryTypeOffer(FIX::MDEntryType_OFFER);
  marketDataEntryGroup.set(mdEntryTypeOffer);
  message.addGroup(marketDataEntryGroup);

  // Set symbols to subscribe to
  // IN THIS CASE IT MUST BE ONLY ONE SYMBOL!!!
  FIX50SP2::MarketDataRequest::NoRelatedSym symbolGroup;
  FIX::Symbol symbol(requestSymbol);
  symbolGroup.set(symbol);
  symbolGroup.set(FIX::SecurityType("FOR"));
  message.addGroup(symbolGroup);

  FIX::MDUpdateType updateType(_config.update_type);
  message.set(updateType);

  message.getHeader().setField(_sessionID.getSenderCompID());
  message.getHeader().setField(_sessionID.getTargetCompID());

  return message;
}

void Application::onMessage(const FIX50SP2::MarketDataSnapshotFullRefresh& message,
                            const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX50SP2::MarketDataSnapshotFullRefresh& message, ...)",
                   "\n(", message.toString().c_str(), ")");
  }
#endif
  this->full_refresh_template<FIX50SP2::MarketDataSnapshotFullRefresh>(message,
      sessionID);
}

void Application::onMessage(const FIX50SP2::MarketDataIncrementalRefresh& message,
  const FIX::SessionID& sessionID) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("FIX50SP2::MarketDataIncrementalRefresh& message, ...)\n",
                   message.toString().c_str());
  }
#endif
  this->incremental_update_template<FIX50SP2::MarketDataIncrementalRefresh>(
    message, sessionID);
}

#endif // USE_FIX_50SP2

/******************************************************************************
 * FIXT 11 
 *****************************************************************************/
#ifdef USE_FIXT11
FIXT11::TestRequest Application::sendTestRequestFIXT11() {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("Application::sendTestRequestFIXT11()");
  }
#endif
  FIXT11::TestRequest tr;
  FIX::TestReqID trid("TestRequest");
  tr.setField(trid);
  return tr;
}

void Application::onMessage(const FIXT11::Logon&, const FIX::SessionID&) {
#ifdef LOG
  if (_config.print_debug) {
    pan::log_DEBUG("RCV FIXT11::Logon& message");
  }
#endif
}
#endif 


