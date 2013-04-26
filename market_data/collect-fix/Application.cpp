/* -*- C++ -*- */


#ifdef _MSC_VER
#pragma warning(disable : 4503 4355 4786 )
#endif

#include "Application.h"
#include "quickfix/Session.h"
#include "quickfix/FieldConvertors.h"
#include "utils/time_utils.h"
#include "utils/types.h"
#include "utils/fix_convertors.h"
#include "utils/jenkins_hash.h"
#include "utils/logging.h"

#include <iostream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "proto/spot_fx_md_1.pb.h"

namespace fs = boost::filesystem;
namespace dt = boost::gregorian;

// KTK TODO move to utils
std::string date_to_string(const dt::date& d) {
  dt::date_facet* facet = new dt::date_facet("%Y_%m_%d");
  std::stringstream ss;
  ss.imbue(std::locale(std::cout.getloc(), facet));
  ss << d;
  return ss.str();
}

bool is_bad_filename_char(char c) {
    return c == '/' || c == '\\' || c == ' ';
}

std::string remove_bad_filename_chars(const std::string& str) {
    std::string s(str.c_str());
    s.erase(remove_if(s.begin(), s.end(), is_bad_filename_char), s.end());
    return s;
}

void printCrossedBookNotification(const char* book_name,
  double bbsize, double bbid, double basize, double bask) {
#ifdef DEBUG
  if (_config.print_debug) {
    pan::log_INFORMATIONAL("Crossed Book: ",
    pBook->getName(),
    " (" ,
    pan::real(bbid),
    ", "
    pan::real(bask),
    ")\n");
  }
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



Application::Application(const ApplicationConfig& config)
           :_loginCount(0),
            _appMsgCount(0),
            _config(config) {
#ifdef DEBUG
    pan::log_DEBUG("Application()");
#endif
}

void Application::onLogon(const FIX::SessionID& sessionID) {
#ifdef DEBUG
    pan::log_DEBUG("onLogon - session ID:", sessionID.toString().c_str());
#endif
    _sessionID = sessionID;
    _loginCount++;
    run();
}

void Application::onLogout(const FIX::SessionID& sessionID) {
#ifdef DEBUG
    pan::log_DEBUG("onLogout - session ID:", sessionID.toString().c_str());
#endif
}

void Application::onCreate(const FIX::SessionID& sessionID) {
#ifdef DEBUG
    pan::log_DEBUG("onCreate - session ID:", sessionID.toString().c_str());
#endif
}

void Application::fromAdmin(const FIX::Message& message,
        const FIX::SessionID& sessionID)
        throw(FIX::FieldNotFound,
            FIX::IncorrectDataFormat,
            FIX::IncorrectTagValue,
            FIX::RejectLogon ) {
#ifdef DEBUG
    if (_config.print_debug)  {
        pan::log_DEBUG("fromAdmin: ", message.toString().c_str());
    }
#endif

    FIX::BeginString beginString;
    message.getHeader().getField(beginString);
    if (beginString == FIX::BeginString_FIX42) {
        ((FIX42::MessageCracker&)(*this)).crack((const FIX42::Message&) message,
        sessionID);
    } else if (beginString == FIX::BeginString_FIX43) {
        ((FIX43::MessageCracker&)(*this)).crack((const FIX43::Message&) message,
        sessionID);
    } else if (beginString == FIX::BeginString_FIX44) {
        ((FIX44::MessageCracker&)(*this)).crack((const FIX44::Message&) message,
        sessionID);
    } else if (beginString == FIX::BeginString_FIX50) {
        ((FIX50::MessageCracker&)(*this)).crack((const FIX50::Message&) message,
        sessionID);
    }
}

void Application::fromApp(const FIX::Message& message,
        const FIX::SessionID& sessionID)
        throw(FIX::FieldNotFound,
            FIX::IncorrectDataFormat,
            FIX::IncorrectTagValue,
            FIX::UnsupportedMessageType) {
#ifdef DEBUG
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
    FIX::BeginString beginString;
    message.getHeader().getField(beginString);
    if (beginString == FIX::BeginString_FIX42) {
        ((FIX42::MessageCracker&)(*this)).crack(message, sessionID);
    } else if (beginString == FIX::BeginString_FIX43) {
        ((FIX43::MessageCracker&)(*this)).crack(message, sessionID);
    } else if (beginString == FIX::BeginString_FIX44) {
        ((FIX44::MessageCracker&)(*this)).crack(message, sessionID);
    } else if (beginString == FIX::BeginString_FIX50) {
        ((FIX50::MessageCracker&)(*this)).crack(message, sessionID);
    }
}

void Application::toApp(FIX::Message& message,
        const FIX::SessionID& sessionID)
        throw(FIX::DoNotSend) {
#ifdef DEBUG
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
#ifdef DEBUG
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
#ifdef DEBUG
        pan::log_CRITICAL("Exception: ", e.what());
#endif
        std::cerr << e.what() << std::endl;
    }
}

template <typename T>
void Application::trading_session_status_template(const T& message,
        const FIX::SessionID& sessionID) {
    FIX::TradingSessionID tradingSessionID;
    FIX::TradSesStatus tradSesStatus;
    if (message.isSetField(tradingSessionID)) {
        message.getField(tradingSessionID);
    }
    if (message.isSetField(tradSesStatus)) {
        message.getField(tradSesStatus);
        if (tradSesStatus.getValue() == FIX::TradSesStatus_OPEN) {
#ifdef DEBUG
            pan::log_INFORMATIONAL("Trading sessions status is OPEN");
#endif
        }
        if (tradSesStatus.getValue() == FIX::TradSesStatus_HALTED) {
#ifdef DEBUG
            pan::log_INFORMATIONAL("Trading sessions status is HALTED");
#endif
        }
        if (tradSesStatus.getValue() == FIX::TradSesStatus_CLOSED) {
#ifdef DEBUG
            pan::log_INFORMATIONAL("Trading sessions status is CLOSED");
#endif
        }
        if (tradSesStatus.getValue() == FIX::TradSesStatus_PREOPEN) {
#ifdef DEBUG
            pan::log_INFORMATIONAL("Trading sessions status is PREOPEN");
#endif
        }
    }
}

void Application::onMessage(const FIX44::TradingSessionStatus& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX44::TradingSessionStatus& message...)");
    }
#endif
    trading_session_status_template<FIX44::TradingSessionStatus>(message,
            sessionID);
}

void Application::onMessage(const FIX43::TradingSessionStatus& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX43::TradingSessionStatus& message...)");
    }
#endif
    trading_session_status_template<FIX43::TradingSessionStatus>(message,
            sessionID);
}

void Application::onMessage(const FIX42::TradingSessionStatus& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX42::TradingSessionStatus& message...)");
    }
#endif
    trading_session_status_template<FIX42::TradingSessionStatus>(message,
            sessionID);
}

void Application::onMessage(const FIX44::Logon& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX44::Logon& message, ...)");
    }
#endif
}

void Application::onMessage(const FIX43::Logon& message,
        const FIX::SessionID& sessionID) {
    if (_config.print_debug) {
#ifdef DEBUG
        pan::log_DEBUG("FIX43::Logon& message, ...)");
#endif
    }
}

void Application::onMessage(const FIX42::Logon& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX42::Logon& message, ...)");
    }
#endif
}

/**
 * Full refresh of order book - all existing orders at all levels
 * 35=W
 */
void Application::onMessage(const FIX44::MarketDataSnapshotFullRefresh& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX44::MarketDataSnapshotFullRefresh& message, ...)",
                "\n(", message.toString().c_str(), ")");
    }
#endif
    this->full_refresh_template<FIX44::MarketDataIncrementalRefresh>(message,
            sessionID);
}

void Application::onMessage(const FIX43::MarketDataSnapshotFullRefresh& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX43::MarketDataSnapshotFullRefresh& message, ...",
                "\n(", message.toString().c_str(), ")");
    }
#endif
    this->full_refresh_template<FIX43::MarketDataIncrementalRefresh>(message,
            sessionID);
}

void Application::onMessage(const FIX42::MarketDataSnapshotFullRefresh& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX42::MarketDataSnapshotFullRefresh& message, ...",
                "\n(", message.toString().c_str(), ")");
    }
#endif
    this->full_refresh_template<FIX42::MarketDataIncrementalRefresh>(message,
            sessionID);
}

template <typename T>
void Application::full_refresh_template(const T& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("Application::full_refresh_template()",
                message.toString().c_str());
    }
#endif
    std::ostream* pLog = NULL;
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
#ifdef DEBUG
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

            if (_config.print_debug) {
                pan::log_DEBUG("Adding ID=",
                    id.c_str(),
                    " price=",
                    pan::real(price),
                    " size=",
                    pan::integer(size));
            }
        }

        // At this point all entries in the message are processed
        // i.e. added to the book. We can now broadcast BBO and
        // Broadcast and log orderbook
        double bbid = pBook->bestPrice(capk::BID);
        double bask = pBook->bestPrice(capk::ASK);
        double bbsize = pBook->bestPriceVolume(capk::BID);
        double basize = pBook->bestPriceVolume(capk::ASK);
        if (bbid > bask) {
          printCrossedBookNotification(book->getName(),
            bbsize,
            bbid,
            basize,
            bask);
        }

        ptime time_start(microsec_clock::local_time());
        broadcast_bbo_book(_pzmq_socket,
                symbol.getValue().c_str(),
                bbid,
                bask,
                bbsize,
                basize,
                _config.venue_id);

        pLog = getStream(symbol.getValue());

        if (pLog == NULL) {
            std::cerr << __FILE__
                <<  ":"
                << __LINE__
                << "Can't find log - log is null!"
                << "\n";
        }
        if (_config.is_logging && pLog != NULL) {
            *pLog << "OB,"
                << pBook->getName()
                << ","
                << pBook->getEventTime()
                << ","
                << pBook->getExchangeSendTime()
                << "\n"
                << *pBook;
        }
    }
}


void Application::broadcast_bbo_book(void* bcast_socket,
        const char* symbol,
        const double best_bid,
        const double best_ask,
        const double bbsize,
        const double basize,
        const capk::venue_id_t venue_id) {
    zmq_msg_t msg;
    char msgbuf[256];
    capkproto::instrument_bbo bbo;

    if (_config.is_publishing) {
        bbo.set_symbol(symbol);
        bbo.set_bid_price(best_bid);
        bbo.set_ask_price(best_ask);
        bbo.set_bid_size(bbsize);
        bbo.set_ask_size(basize);
        bbo.set_bid_venue_id(venue_id);
        bbo.set_ask_venue_id(venue_id);

        size_t msgsize = bbo.ByteSize();
        assert(msgsize < sizeof(msgbuf));
        if (msgsize > sizeof(msgbuf)) {
#ifdef DEBUG
            pan::log_CRITICAL("Buffer too small for protobuf serializaion");
#endif
            std::cerr << "CRITIAL: Buffer too small for protobuf serialization"
                << std::endl;
        }
        bbo.SerializeToArray(msgbuf, msgsize);

        zmq_msg_init_size(&msg, msgsize);
        memcpy(zmq_msg_data(&msg), msgbuf, msgsize);
#ifdef DEBUG
        if (_config.print_debug) {
            pan::log_DEBUG("Sending ", pan::integer(msgsize), " bytes\n");
            pan::log_DEBUG("Protobuf:\n", bbo.DebugString().c_str(), "\n");
        }
#endif
        zmq_send(bcast_socket, &msg, 0);
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
    std::ostream* pLog = NULL;

    static double dbar, dcount, dsum;

    if (message.isSetField(mdReqID)) {
        message.getField(mdReqID);
    }

    if (message.isSetField(symbol)) {
        message.getField(symbol);
    } else {
#ifdef DEBUG
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
#ifdef DEBUG
                pan::log_WARNING("MDENTRYID field not set in BODY of "
                       "MarketDataIncrementalRefresh msg (35=X)");
#endif
            }
            if (mdEntries.isSetField(symbol)) {
                mdEntries.getField(symbol);
            } else {
#ifdef DEBUG
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
#ifdef DEBUG
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
            pLog = getStream(symbol.getValue());

            if (NULL != (pBook = getBook(symbol.getValue())))  {
                char side = mdEntryType.getValue();
                capk::Side_t nside = char2side_t(side);
                double price = 0.0;
                size = mdEntrySize.getValue();

                const std::string& id = mdEntryID.getValue();

                uint32_t nid = hashlittle(id.c_str(), id.size(), 0);
#ifdef DEBUG
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
#ifdef DEBUG
                        if (_config.print_debug) {
                            pan::log_DEBUG("Deleting old: ",
                                pan::integer(nid),
                                " before adding new "
                                "(new_replaces enabled in conifg)");
                        }
#endif

                        int removeOk = pBook->remove(nid, evtTime, sndTime);
#ifdef DEBUG
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
#ifdef DEBUG
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
                    if (_config.is_logging) {
                        *pLog << "A,"
                            << nside << ","
                            << std::setiosflags(std::ios_base::fixed)
                            << size << ","
                            << std::setprecision(5)
                            << price << ","
                            << evtTime << "\n";
                    }
#ifdef DEBUG
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
                        if (_config.print_debug) {
                            pan::log_WARNING("Using mdEntryRefID: ",
                                     mdEntryRefID.getString().c_str(),
                                     " AS mdEntryID: ",
                                     mdEntryID.getString().c_str());

                            if (mdEntryRefID.getValue() != mdEntryID.getValue()) {
                                pan::log_WARNING("mdEntryRefID != mdEntryID");
                            }
                        }

                        uint32_t nrefId = hashlittle(refId.c_str(), refId.size(), 0);

                        if (_config.is_logging) {
                            *pLog << "M,"
                                << nside << ","
                                << std::setiosflags(std::ios_base::fixed)
                                << size << "," << std::setprecision(5)
                                << price << ","
                                << evtTime << "\n";
                        }
                        pBook->remove(nrefId, evtTime, sndTime);
                        pBook->add(nid, nside, size, price, evtTime, sndTime);
                    } else {
#ifdef DEBUG
                        if (_config.print_debug) {
                            pan::log_DEBUG("Modify: ",
                                    pan::integer(nid),
                                    " to size ",
                                    pan::integer(size));
                        }
#endif
                        capk::pKOrder pOrder = pBook->getOrder(nid);
                        if (pOrder) {
                            if (_config.is_logging) {
                                *pLog << "M,"
                                    << pBook->getOrder(nid)->getSide() << ","
                                    << std::setiosflags(std::ios_base::fixed)
                                    << size << ","
                                    << std::setprecision(5)
                                    << pBook->getOrder(nid)->getPrice() << ","
                                    << evtTime << "\n";
                            }
#ifdef DEBUG
                            if (_config.print_debug) {
                                pan::log_DEBUG("Modify ",
                                pan::integer(nside), ",",
                                pan::integer(size), ",",
                                pan::real(price), ",", "\n");
                            }
#endif
                            int modifyOk = pBook->modify(nid, size, evtTime, sndTime);

                            if (modifyOk != 1) {
#ifdef DEBUG
                                pan::log_WARNING("Modify failed. MdEntryID: ",
                                       pan::integer(nid),
                                       " does not exist in book");
#endif
                                std::cerr << "Modify failed. MdEntryID: "
                                    << nid
                                    << " does not exist in book\n";
                           }
                        } else {
#ifdef DEBUG
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
                    if (_config.print_debug) {
                        pan::log_DEBUG("Delete: ",
                               pan::integer(nid));
                    }
                    capk::pKOrder pOrder = pBook->getOrder(nid);
                    if (pOrder) {
                      if (_config.is_logging) {
                          *pLog << "D,"
                              << pBook->getOrder(nid)->getSide() << ","
                              << std::setprecision(0)
                              << std::setiosflags(std::ios_base::fixed)
                              << pBook->getOrder(nid)->getSize() << ","
                              << std::setprecision(5)
                              << pBook->getOrder(nid)->getPrice() << ","
                              << evtTime << "\n";
                        }
#ifdef DEBUG
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
#ifdef DEBUG
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
          printCrossedBookNotification(pBook->getName().c_str(),
                                bbsize,
                                bbid,
                                basize,
                                bask);


        ptime time_start(microsec_clock::local_time());
        if (_config.is_publishing) {
            broadcast_bbo_book(_pzmq_socket,
                    symbol.getValue().c_str(),
                    bbid,
                    bask,
                    bbsize,
                    basize,
                    _config.venue_id);
        }

#ifdef DEBUG
        if (pLog == NULL) {
            std::cerr << __FILE__ <<  ":"  << __LINE__ << "Can't find log - log is null!" << "\n";
        }
#endif
        if (_config.is_logging && pLog != NULL) {
            *pLog << "OB,"
                << pBook->getName()
                << ","
                << pBook->getEventTime()
                << ","
                << pBook->getExchangeSendTime()
                << "\n";
            *pLog << *pBook;
        }

        // Timing stats
        ptime time_end(microsec_clock::local_time());
        time_duration duration(time_end - time_start);
        dsum += duration.total_microseconds();
        dcount++;
        dbar = dsum / dcount;
        std::cout << "Mean time(us) to broadcast and write log: " << dbar << "\n";
        std::cout << "SANITY CHECK: " << to_simple_string(duration) << "\n";
    }
}

/*
 * Incremental refresh message - started once initial orderbook has been built
 * This is the FIX 35=X message
 */
void Application::onMessage(const FIX44::MarketDataIncrementalRefresh& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX44::MarketDataIncrementalRefresh& message, ...)\n",
                message.toString().c_str());
    }
#endif
    this->incremental_update_template<FIX44::MarketDataIncrementalRefresh>(
            message, sessionID);
}

void Application::onMessage(const FIX43::MarketDataIncrementalRefresh& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX43::MarketDataIncrementalRefresh& message, ...)\n",
                message.toString().c_str());
    }
#endif
    this->incremental_update_template<FIX43::MarketDataIncrementalRefresh>(
            message, sessionID);
}

void Application::onMessage(const FIX42::MarketDataIncrementalRefresh& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX42::MarketDataIncrementalRefresh& message, ...)\n",
                message.toString().c_str());
    }
#endif
    this->incremental_update_template<FIX42::MarketDataIncrementalRefresh>(
            message, sessionID);
}


void Application::onMessage(const FIX44::MarketDataRequestReject& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX44::MarketDataRequestReject& message, ...)\n",
                message.toString().c_str());
    }
#endif
}

void Application::onMessage(const FIX43::MarketDataRequestReject& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX43::MarketDataRequestReject& message, ...)\n",
                message.toString().c_str());
    }
#endif
}

void Application::onMessage(const FIX42::MarketDataRequestReject& message,
        const FIX::SessionID& sessionID) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("FIX42::MarketDataRequestReject& message, ...)\n",
                message.toString().c_str());
    }
#endif
}


void Application::toAdmin(FIX::Message& message,
        const FIX::SessionID& sessionID) {
    FIX::MsgType msgType;
    message.getHeader().getField(msgType);
    if (msgType.getValue() == "1") {  // Test Request
#ifdef DEBUG
        if (_config.print_debug) {
            pan::log_DEBUG("Sending TestRequest (35=1)");
        }
#endif
    }
    if (msgType.getValue() == "2") {  // Resend Request
#ifdef DEBUG
        if (_config.print_debug) {
            pan::log_DEBUG("Sending ResendRequest (35=2)");
        }
#endif
    }
    if (msgType.getValue() == "3") {  // Reject
#ifdef DEBUG
        if (_config.print_debug) {
            pan::log_ERROR("Sending Reject (35=3)");
        }
#endif
    }
    if (msgType.getValue() == "0") {  // Heartbeat
#ifdef DEBUG
        if (_config.print_debug) {
            pan::log_DEBUG("Sending Heartbeat (35=0)");
        }
#endif
    }
    if (msgType.getValue() == "4") {  // Sequence Reset
#ifdef DEBUG
        if (_config.print_debug) {
            pan::log_WARNING("Sending SequenceReset (35=4)");
        }
#endif
    }
    if (msgType.getValue() == "A") {  // Logon
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
#ifdef DEBUG
            pan::log_DEBUG("Sending reset sequence number request (141=Y)");
#endif
            FIX::ResetSeqNumFlag flag = FIX::ResetSeqNumFlag_YES;
            message.setField(FIX::FIELD::ResetSeqNumFlag, "Y");
        }
    }

#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("toAdmin sent: ", message.toString());
    }
#endif
}

void Application::run() {
#ifdef DEBUG
    pan::log_DEBUG("Application::run()");
#endif

    std::vector<std::string>::const_iterator it = _symbols.begin();
    std::string logFileName;
    std::ofstream* pLog;
    capk::KBook* pBook = NULL;
    std::string MIC_prefix =
        _config.mic_string.length() > 0 ? _config.mic_string + "_" : "";
    std::string symbol;
    fs::path fullPathToLog;
    boost::gregorian::date today;
    std::string dateToday;
    fs::path datePath;

    if (_config.is_logging) {
        today = boost::gregorian::day_clock::universal_day();
        dateToday = date_to_string(today);
        //  std::string dateToday =  to_iso_string(d);
        _pathToLog = fs::path(_config.order_books_output_dir);
        datePath = _pathToLog / fs::path(dateToday);
        if (!fs::exists(datePath)) {
            fs::create_directory(datePath);
        }
    }

    /**
     * Create for each symbol:
     * 1) OrderBook
     * 2) Log file
     */
    while (it != _symbols.end() && *it != "") {
        bool isRestart = false;
        symbol = *it;
        pBook = new capk::KBook(symbol.c_str(), _config.market_depth);
#ifdef DEBUG
        pan::log_DEBUG("Created new order book:\n",
            "symbol  : ", symbol.c_str(), "\n",
            "depth   : ", pan::integer(pBook->getDepth()));
#endif
        if (_config.is_logging) {
            logFileName = MIC_prefix
                          + remove_bad_filename_chars(symbol)
                          + "_"
                          + dateToday;
            logFileName.append(".csv");
            fullPathToLog = datePath / fs::path(logFileName);
            if (fs::exists(fullPathToLog)) {
                isRestart = true;
            }

            pLog =  new std::ofstream(fullPathToLog.string(),
                                      std::ios::app | std::ios::out);

            if (isRestart) {
                timespec evtTime;
                clock_gettime(CLOCK_MONOTONIC, &evtTime);
                if (_config.is_logging) {
                    *pLog << "RESTART: " << evtTime << "\n";
                }
#ifdef DEBUG
                pan::log_DEBUG("Re-opened tick log for:\n",
                "symbol  : ", symbol.c_str(), "\n",
                "file    : ", fullPathToLog.string().c_str(), "\n",
                "open    : ", pan::integer(pLog->is_open()));
#endif
            } else {
#ifdef DEBUG
                pan::log_DEBUG("Created tick log for:\n",
                "symbol  : ", symbol.c_str(), "\n",
                "file    : ", fullPathToLog.string().c_str(), "\n",
                "open    : ", pan::integer(pLog->is_open()));
#endif
                timespec evtTime;
                clock_gettime(CLOCK_MONOTONIC, &evtTime);
                *pLog << pBook->getOutputVersionString()
                    << "," << pBook->getName()
                    << "," << pBook->getDepth()
                    << "," << evtTime
                    << "\n";
            }
            addStream(symbol, pLog);
        }
        addBook(symbol, pBook);
        it++;
    }
    std::cerr << "Total books created: "
              << _symbolToBook.size() << "\n";
    std::cerr << "Total tick logs created: "
              << _symbolToLogStream.size() << "\n";



     // Some venues require each market data subscription to be sent
     // in a different message - i.e. when we send 35=V we are ONLY
     // allowed to send 146(NoRelatedSymbols)=1 so we send multiple
     // 35=V requests

    if (_config.sendIndividualMarketDataRequests) {
        for (std::vector<std::string>::const_iterator it = _symbols.begin();
                it != _symbols.end();
                it++) {
            if (*it != "") {
                querySingleMarketDataRequest(*it);
            }
        }
    } else {
        // Send a single market data request (35=V) with all symbols
        queryMarketDataRequest(_symbols);
    }
}

void Application::sendTestRequest() {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("Application::sendTestRequest()");
    }
#endif

    FIX::Message testRequestMessage;
    switch (_config.version) {
    case FIX_42:
        testRequestMessage = sendTestRequest42();
        break;
    case FIX_43:
        testRequestMessage = sendTestRequest43();
        break;
    case FIX_44:
        testRequestMessage = sendTestRequest44();
        break;
    case FIX_50:
    case FIX_50SP1:
    case FIX_50SP2:
        testRequestMessage = sendTestRequest50();
        break;
    default:
        throw std::runtime_error("Unsupported FIX version");
    }
    FIX::Session::sendToTarget(testRequestMessage,
            _sessionID.getSenderCompID(),
            _sessionID.getTargetCompID());
}

FIXT11::TestRequest Application::sendTestRequest50() {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("Application::sendTestRequest50()");
    }
#endif
    FIXT11::TestRequest tr;
    FIX::TestReqID trid("TestRequest");
    tr.setField(trid);
    return tr;
}


FIX44::TestRequest Application::sendTestRequest44() {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("Application::sendTestRequest44()");
    }
#endif
    FIX44::TestRequest tr;
    FIX::TestReqID trid("TestRequest");
    tr.setField(trid);
    return tr;
}

FIX43::TestRequest Application::sendTestRequest43() {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("Application::sendTestRequest43()");
    }
#endif
    FIX43::TestRequest tr;
    FIX::TestReqID trid("TestRequest");
    tr.setField(trid);
    return tr;
}

FIX42::TestRequest Application::sendTestRequest42() {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("Application::sendTestRequest42()");
    }
#endif
    FIX42::TestRequest tr;
    FIX::TestReqID trid("TestRequest");
    tr.setField(trid);
    return tr;
}

void Application::querySingleMarketDataRequest(
                const std::string& requestSymbol) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("querySingleMarketDataRequest(...)");
    }
#endif

    FIX::Message md;
    switch (_config.version) {
    case FIX_42:
        md = querySingleMarketDataRequest42(requestSymbol);
        break;
    case FIX_43:
        md = querySingleMarketDataRequest43(requestSymbol);
        break;
    case FIX_44:
        md = querySingleMarketDataRequest44(requestSymbol);
        break;
    case FIX_50:
    case FIX_50SP1:
    case FIX_50SP2:
        md = querySingleMarketDataRequest50(requestSymbol);
        break;
    default:
        throw std::runtime_error("Unsupported FIX version");
    }
    FIX::Session::sendToTarget(md);
}

FIX50SP2::MarketDataRequest Application::querySingleMarketDataRequest50(
                const std::string& requestSymbol) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("querySingleMarketDataRequest50(",
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


FIX44::MarketDataRequest Application::querySingleMarketDataRequest44(
                const std::string& requestSymbol) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("querySingleMarketDataRequest44(",
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

FIX43::MarketDataRequest Application::querySingleMarketDataRequest43(
      const std::string& requestSymbol) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("querySingleMarketDataRequest43(",
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

FIX42::MarketDataRequest Application::querySingleMarketDataRequest42(
    const std::string& requestSymbol) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("querySingleMarketDataRequest42(",
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


void Application::queryMarketDataRequest(
    const std::vector<std::string>& symbols) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("Application::queryMarketDataRequest(...)");
    }
#endif
    FIX::Message md;
    switch (_config.version) {
    case FIX_42:
        md = queryMarketDataRequest42(symbols);
        break;
    case FIX_43:
        md = queryMarketDataRequest43(symbols);
        break;
    case FIX_44:
        md = queryMarketDataRequest44(symbols);
        break;
    case FIX_50:
    case FIX_50SP1:
    case FIX_50SP2:
        md = queryMarketDataRequest50(symbols);
        break;
    default:
        throw std::runtime_error("Unsupported FIX version");
    }
    FIX::Session::sendToTarget(md);
}

FIX50SP2::MarketDataRequest Application::queryMarketDataRequest50(
    const std::vector<std::string>& symbols) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("queryMarketDataRequest50(...)");
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


FIX44::MarketDataRequest Application::queryMarketDataRequest44(
    const std::vector<std::string>& symbols) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("queryMarketDataRequest44(...)");
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


FIX43::MarketDataRequest Application::queryMarketDataRequest43(
    const std::vector<std::string>& symbols) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("queryMarketDataRequest43(...)");
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

FIX42::MarketDataRequest Application::queryMarketDataRequest42(
    const std::vector<std::string>& symbols) {
#ifdef DEBUG
    if (_config.print_debug) {
        pan::log_DEBUG("queryMarketDataRequest42(...)");
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

void Application::addSymbols(const std::vector<std::string>& symbols) {
    _symbols = symbols;
}

const std::vector<std::string>& Application::getSymbols() {
    return _symbols;
}

void Application::addStream(const std::string& symbol, std::ostream* log) {
    _symbolToLogStream[symbol] = log;
}

std::ostream* Application::getStream(const std::string& symbol) {
    symbolToLogStreamIterator it = _symbolToLogStream.find(symbol);
    if (it == _symbolToLogStream.end()) {
        return NULL;
    } else {
        return (it->second);
    }
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

void Application::deleteLogs() {
#ifdef DEBUG
    if (_config.print_debug)  {
        pan::log_DEBUG("Flushing and deleting streams");
    }
#endif

    symbolToLogStreamIterator streamIter = _symbolToLogStream.begin();
    while (streamIter != _symbolToLogStream.end()) {
        std::ostream* stream = streamIter->second;
        if (stream) {
            std::flush(*stream);
            delete stream;
        }
        streamIter++;
    }
}

void Application::deleteBooks() {
#ifdef DEBUG
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

Application::~Application() {
    deleteBooks();
    deleteLogs();
}
