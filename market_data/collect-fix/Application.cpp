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

#include <boost/lexical_cast.hpp>

#include "proto/spot_fx_md_1.pb.h"

namespace fs = boost::filesystem; 
namespace dt = boost::gregorian; 

Application::Application(const ApplicationConfig& config) 
         :  _loggedIn(false), 
            _loggedOut(true), 
            _loginCount(0), 
            _appMsgCount(0), 
            _config(config) 
{
#ifdef LOG
    pan::log_DEBUG("Application()");
#endif
}

void Application::onLogon(const FIX::SessionID& sessionID )
{
	_loggedIn = true;
	_loggedOut = false;
	_sessionID = sessionID;

#ifdef LOG
    pan::log_DEBUG("onLogon - session ID:", sessionID.toString().c_str());
#endif
	_loginCount++;
	//sendTestRequest();
	run();				
}

void Application::onLogout(const FIX::SessionID& sessionID )
{
	_loggedIn = false;
	_loggedOut = true;

#ifdef LOG
    pan::log_DEBUG("onLogout - session ID:", sessionID.toString().c_str());
#endif
}

void Application::onCreate(const FIX::SessionID& sessionID )
{
#ifdef LOG
    pan::log_DEBUG("onCreate - session ID:", sessionID.toString().c_str());
#endif
}

void Application::fromAdmin(const FIX::Message& message, const FIX::SessionID& sessionID)
	throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon )
{
	FIX::BeginString beginString;
	message.getHeader().getField(beginString);
    if (_config.print_debug)  {
#ifdef LOG
        pan::log_DEBUG("fromAdmin: ", message.toString().c_str()); 	
#endif
    }
	if (beginString == FIX::BeginString_FIX42) {
		((FIX42::MessageCracker&)(*this)).crack((const FIX42::Message&) message, sessionID);
	}
	else if (beginString == FIX::BeginString_FIX43) {
		((FIX43::MessageCracker&)(*this)).crack((const FIX43::Message&) message, sessionID);
	}
	else if (beginString == FIX::BeginString_FIX44) {
		((FIX44::MessageCracker&)(*this)).crack((const FIX44::Message&) message, sessionID);
	}
}

void Application::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID )
throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
	if (_config.print_debug) { 
#ifdef LOG
        pan::log_DEBUG("fromApp: ", message.toString().c_str()); 	
#endif
	}
	else { 
        _appMsgCount++;
        if (_appMsgCount % 10000 == 0) {
            std::cout << "Received " << _appMsgCount << " application messages\n";
        }
	} 
	FIX::BeginString beginString;
	message.getHeader().getField(beginString);
	if (beginString == FIX::BeginString_FIX42) {
		((FIX42::MessageCracker&)(*this)).crack((const FIX42::Message&) message, sessionID);
	}
	else if (beginString == FIX::BeginString_FIX43) {
		((FIX43::MessageCracker&)(*this)).crack((const FIX43::Message&) message, sessionID);
	}
	else if (beginString == FIX::BeginString_FIX44) {
		((FIX44::MessageCracker&)(*this)).crack((const FIX44::Message&) message, sessionID);
	}
}

void Application::toApp(FIX::Message& message, const FIX::SessionID& sessionID )
throw(FIX::DoNotSend )
{
    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("toApp: ", message.toString().c_str()); 	
#endif
    }
	try {
		// Don't send potenially duplicate requests for market data
		FIX::PossDupFlag possDupFlag;
		if (message.isSetField(possDupFlag)) {
			message.getHeader().getField(possDupFlag );
			if (possDupFlag ) throw FIX::DoNotSend();
		}
	}
	catch (FIX::FieldNotFound& e) {
		std::cerr << "toApp - EXCEPTION(" << e.what()  << e.detail << " (" << e.field << ")" << ")" << "\n";	
	}
	catch (FIX::Exception& e) {
		std::cerr << e.what() << "\n";	
	}

}

template <typename T> 
void 
Application::trading_session_status_template(const T& message, const FIX::SessionID& sessionID)  
{
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

void Application::onMessage(const FIX44::TradingSessionStatus& message, const FIX::SessionID& sessionID) 
{
	
    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX44::TradingSessionStatus& message...)");
#endif
    }
    trading_session_status_template<FIX44::TradingSessionStatus>(message, sessionID);
}

void Application::onMessage(const FIX43::TradingSessionStatus& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX43::TradingSessionStatus& message...)");
#endif
    }
    trading_session_status_template<FIX43::TradingSessionStatus>(message, sessionID);
}

void Application::onMessage(const FIX42::TradingSessionStatus& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX42::TradingSessionStatus& message...)");
#endif
    }
    trading_session_status_template<FIX42::TradingSessionStatus>(message, sessionID);
}

void Application::onMessage(const FIX44::Heartbeat& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX44::Heartbeat& message, ...)");
#endif
    }
}

void Application::onMessage(const FIX43::Heartbeat& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX43::Heartbeat& message, ...)");
#endif
    }
}

void Application::onMessage(const FIX42::Heartbeat& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX42::Heartbeat& message, ...)");
#endif
    }
}

void Application::onMessage(const FIX44::Logout& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX44::Logout& message, ...)");
#endif
    }
}

void Application::onMessage(const FIX43::Logout& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX43::Logout& message, ...)");
#endif
    }
}

void Application::onMessage(const FIX42::Logout& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX42::Logout& message, ...)");
#endif
    }
}

void Application::onMessage(const FIX44::Logon& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX44::Logon& message, ...)");
#endif
    }
}

void Application::onMessage(const FIX43::Logon& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX43::Logon& message, ...)");
#endif
    }
}

void Application::onMessage(const FIX42::Logon& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX42::Logon& message, ...)");
#endif
    }
}

/**
 * Full refresh of order book - all existing orders at all levels 
 * 35=W
 */
void 
Application::onMessage(const FIX44::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) {
	    std::cout << "Application::onMessage(const FIX44::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID)" << "\n";
    }
    this->full_refresh_template<FIX44::MarketDataIncrementalRefresh>(message,sessionID); 
}

/*
 * Full refresh of order book - all existing orders at all levels 
 * 35=W
 */
void 
Application::onMessage(const FIX43::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) {
	    std::cout << "Application::onMessage(const FIX43::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID)" << "\n";
    }
    this->full_refresh_template<FIX43::MarketDataIncrementalRefresh>(message,sessionID); 
}

/*
 * Full refresh of order book - all existing orders at all levels 
 * 35=W
 */
void 
Application::onMessage(const FIX42::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID) 
{
	if (_config.print_debug) { 
	    std::cout << "Application::onMessage(const FIX42::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID)(" << message.toString() << ")" << "\n";
    }
    this->full_refresh_template<FIX42::MarketDataIncrementalRefresh>(message,sessionID); 
}

template <typename T> 
void 
Application::full_refresh_template(const T& message, const FIX::SessionID& sessionID) 
{
    if (_config.print_debug) { 
	    std::cout << "Application::full_refresh_template()" 
            << message.toString() 
            << "\n";
    }
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
		catch (std::exception& e) {
			std::cerr << e.what() << "\n";
		}
        if (nEntries > 0) {
            // clear the book since we have a complete refresh
			if (NULL != (pBook = getBook(symbol.getValue())))  {
                pBook->clear();
            }
        }

		typename T::NoMDEntries mdEntries;
		// Group indexed on 1 not 0
		for (int i = 0; i< nEntries; i++) {
			message.getGroup(i+1, mdEntries);
	        if (mdEntries.isSetField(symbol)) {
		        mdEntries.getField(symbol);	
	        }
			if (mdEntries.isSetField(mdEntryType)) {
				mdEntries.getField(mdEntryType);
			}
            /** 
             * Below is required for some ECNs that don't really know what they are doing
            FIX::QuoteCondition condition;
            if (mdEntries.isSetField(condition)) {
				mdEntries.getField(condition);
                std::cerr << "QUOTE COND ====================> "  << condition.getValue() << std::endl;
            }
            */
			if (mdEntries.isSetField(mdEntryID)) {
				mdEntries.getField(mdEntryID);
                std::cerr << "MD ENTRY ID ====================> "  << mdEntryID.getValue() << std::endl;
			}
			if (mdEntries.isSetField(mdEntryPx)) {
				mdEntries.getField(mdEntryPx);
			}
			else {
				std::cerr << "NO MDEntryPrice SET IN SNAPSHOT" << "\n";	
			}
			if (mdEntries.isSetField(mdEntrySize)) {
				mdEntries.getField(mdEntrySize);	
			}
			else {
				std::cerr << "NO MDEntrySize SET IN SNAPSHOT" << "\n";	
			}
			if (mdEntries.isSetField(quoteType)) {
				mdEntries.getField(quoteType);	
                if (_config.print_debug) {
				    std::cout << "QuoteType: " << quoteType.getValue() << "\n"; 
                }
			}
			if (mdEntries.isSetField(mdEntryOriginator)) {
				mdEntries.getField(mdEntryOriginator);	
                if (_config.print_debug) {
				    std::cerr << "MDEntryOriginator: " << mdEntryOriginator.getValue() << "\n"; 
                }
			}
			if (mdEntries.isSetField(minQty)) {
				mdEntries.getField(minQty);	
                if (_config.print_debug) {
				    std::cout << "MinQty: " << minQty.getValue() << "\n"; 
                }
			}
			if (mdEntries.isSetField(mdEntryPositionNo)) {
				mdEntries.getField(mdEntryPositionNo);	
                if (_config.print_debug) {
				    std::cout << "MDEntryPositionNo: " << mdEntryPositionNo.getValue() << "\n"; 
                }
			}
			if (mdEntries.isSetField(execInst)) {
				mdEntries.getField(execInst);	
			}
			if (mdEntries.isSetField(quoteEntryID)) {
				mdEntries.getField(quoteEntryID);	
			}
			//if (message.isSetField(securityType)) {
				//message.getField(securityType);
			//}
			//if (message.isSetField(maturityMonthYear)) {
				//message.getField(maturityMonthYear);
			//}

            ///capk::KBook* pBook;
			///if (NULL != (pBook = getBook(symbol.getValue())))  {
				FIX::UtcTimeStamp time = FIX::UtcTimeStamp(sendingTime); 
				char side = mdEntryType.getValue(); 
                capk::Side_t nside = char2side_t(side);
				std::string id = mdEntryID.getValue(); 
                int nid = hashlittle(id.c_str(), id.size(), 0);

                /* since we sometimes don't get entry IDs in snapshots, 
                   try using the quote entry ID instead 
                 */ 
                if (id.length() == 0) {
                    id = quoteEntryID.getValue(); 
                    nid = hashlittle(id.c_str(), id.size(), 0);
                }
				double price = mdEntryPx.getValue(); 
				unsigned int size = mdEntrySize.getValue(); 
				//capitalk::PriceDepthEntry* entry = 
				//	new capitalk::PriceDepthEntry(time, time, side, id, price, size); 
				
                //pBook->add(entry); 
                timespec evtTime, sndTime;
                clock_gettime(CLOCK_MONOTONIC, &evtTime);
                FIXConvertors::UTCTimeStampToTimespec(time, &sndTime);
                std::cerr << "EVTTIME ON FULL REFRESH: " << evtTime << std::endl;
                pBook->add(nid, nside, size, price, evtTime, sndTime);
                if (_config.print_debug) {
                    std::cout << "[TEMPLATE: Full Refresh] Adding ID=" << id << " price=" << price <<  " size=" << size << "\n";   
                }
			///}
		}
        // At this point all entries in the message are processed
        // i.e. added to the book. We can now broadcast BBO and 
        // Broadcast and log orderbook
        double bbid = pBook->bestPrice(capk::BID);
        double bask = pBook->bestPrice(capk::ASK);
        if(bbid > bask) {
            std::cerr << "XXXXXXXXXXXXXXXX CROSSED BOOK " << pBook->getName() << " (" << (double) bbid <<  ", " << (double) bask << ") XXXXXXXXXXXXXXXXX\n";
            //assert(false);
        }

        double bbsize = pBook->bestPriceVolume(capk::BID);
        double basize = pBook->bestPriceVolume(capk::ASK);
			    
        std::cerr << "========>FULL REFRESH BOOK BBO: " << bbid << "(" << bbsize << ")" << "@" << bask << "(" << basize << ")" << std::endl;
        ptime time_start(microsec_clock::local_time());
        broadcast_bbo_book(_pzmq_socket, 
                symbol.getValue().c_str(),
                bbid,
                bask,
                bbsize, 
                basize, 
                _config.venue_id);
// write log if needed.
		pLog = getStream(symbol.getValue());
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
    }
}


void 
Application::broadcast_bbo_book(void* bcast_socket, const char* symbol, const double best_bid, const double best_ask, const double bbsize, const double basize, const capk::venue_id_t venue_id)
{
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
            std::cerr << "WARNING: buf too small for protobuf serialization!" << std::endl;
        }
        bbo.SerializeToArray(msgbuf, msgsize);

        zmq_msg_init_size(&msg, msgsize);
        memcpy(zmq_msg_data(&msg), msgbuf, msgsize);
        if (_config.print_debug) {
            std::cerr << "Sending "  << msgsize << " bytes\n";
            std::cerr << "Protobuf:\n" << bbo.DebugString().c_str() << "\n" << std::endl;
        }
        zmq_send(bcast_socket, &msg, 0);
    }
}


/* 
 * Incremental refresh message - started once initial orderbook has been built
 * This is the FIX 35=X message
 */
template <typename T> 
void 
Application::incremental_update_template(const T& message, const FIX::SessionID& sessionID) 
{
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
    }
    else {
#ifdef LOG
        pan::log_WARNING("SYMBOL field not set in BODY of MarketDataIncrementalRefresh msg (35=X)"); ;
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
            }
            else {
#ifdef LOG
                pan::log_WARNING("MDENTRYID field not set in BODY of MarketDataIncrementalRefresh msg (35=X)"); ;
#endif
            }
			if (mdEntries.isSetField(symbol)) {
				mdEntries.getField(symbol);
			}
            else {
#ifdef LOG
                pan::log_WARNING("SYMBOL field not set in FIELD of MarketDataIncrementalRefresh msg (35=X)"); ;
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
			}
			else {
#ifdef LOG
                pan::log_WARNING("MDENTRYPX field not set in FIELD of MarketDataIncrementalRefresh msg (35=X)"); ;
#endif
			}
			if (mdEntries.isSetField(mdEntrySize)) {
				mdEntries.getField(mdEntrySize);
			}
			// KTK - Added to support MDEntrySize not being set when a delete is sent without a size
			else {
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

                if (_config.print_debug) {
                    std::cerr << "***** ORIG ID: " << id << ", HASH ID: " << nid << "\n";
                }
				int action = mdUpdateAction.getValue(); 
				if (action == FIX::MDUpdateAction_NEW) { // new 
                    price = mdEntryPx.getValue();
                    if (_config.new_replaces) {
                        if (_config.print_debug) {
                            std::cerr << "***** DEL BEFORE ADD: " << nid << "\n";
                        }
                        int removeOk = pBook->remove(nid, evtTime, sndTime);
                        if (_config.print_debug) {
                            if (removeOk == 0) {
                                std::cerr << "Order: " << nid << " not found - so just add \n"; 
                            }
                            std::cerr << "***** ADD: " << nid << ", " << size  << "@" << price << "\n";
                        }
                    }

                    if (pBook->add(nid, nside, size, price, evtTime, sndTime) !=1) {
                        std::cerr << "Book add failed!!!!!!!!!!\n (" << nid << nside << "(" << side << ")" << size << price << evtTime << sndTime << "\n";
                    }
                    if (_config.is_logging) {
                        *pLog << "A," << nside << "," << std::setiosflags(std::ios_base::fixed) << size << "," << std::setprecision(5) << price << "," << evtTime << "\n"; 
                    }
                    if (_config.print_debug) {
                        std::cerr << "A," << nside << "," << std::setiosflags(std::ios_base::fixed) << size << "," << std::setprecision(5) << price << "," << evtTime << "\n"; 
                        pBook->dbg();
                    }
				} else if (action == FIX::MDUpdateAction_CHANGE) { // change  
                    price = mdEntryPx.getValue();
                    size = mdEntrySize.getValue(); 
                    /* if the message contains a MDEntryRefID 
                     * then we're renaming an existing entity so we delete 
                     * the old one (with MDEntryRefID and re-add with mdEntryID) 
                     */ 
			        if (mdEntries.isSetField(mdEntryRefID)) {
			    	    mdEntries.getField(mdEntryRefID);
                        const std::string& refId = mdEntryRefID.getValue(); 
                        if (_config.print_debug) {
                            std::cerr << "*********** Using (mdEntryRefID as mdEntryID) " << mdEntryRefID << ", " << mdEntryID << "\n";
                            if (mdEntryRefID.getValue() != mdEntryID.getValue()) {
                                std::cerr << "********** MISMATCH ENTRY IDS" << "\n";
                            }
                        }
                        
                        uint32_t nrefId = hashlittle(refId.c_str(), refId.size(), 0);
                    
                        if (_config.print_debug) {
                            //std::cerr << "Synthetic modify: " << nrefId << " and re-adding " << "(" << nid << ", " << nside << ", " << size << "@" << price << ")\n";
                            //std::cerr << "M," << nside << "," << std::setiosflags(std::ios_base::fixed) << size << "," << std::setprecision(7) << price << "," << evtTime << "\n"; 
                        }
                        if (_config.is_logging) {
                            *pLog << "M," << nside << "," << std::setiosflags(std::ios_base::fixed) << size << "," << std::setprecision(5) << price << "," << evtTime << "\n"; 
                        }
                        pBook->remove(nrefId, evtTime, sndTime);
                        pBook->add(nid, nside, size, price, evtTime, sndTime);
			        } else { 
                        if (_config.print_debug) {
                            std::cerr << "***** MOD: " << nid << " to size " << size << "\n";
                        }
                        //std::cerr << "M," << pBook->getOrder(nid)->getSide() << "," << size << "," << std::setprecision(5) << pBook->getOrder(nid)->getPrice() << "," << evtTime << "\n"; 
                        capk::pKOrder pOrder = pBook->getOrder(nid);
                        if (pOrder) {
                            if (_config.is_logging) {
                            *pLog << 
                            "M," << 
                            pBook->getOrder(nid)->getSide() << 
                            "," << 
                            std::setiosflags(std::ios_base::fixed) << size << 
                            "," << 
                            std::setprecision(5) << pBook->getOrder(nid)->getPrice() << 
                            "," <<  
                            evtTime << 
                            "\n"; 
                            }
                            if (_config.print_debug) {
                            std::cerr << 
                            "M," << 
                            pBook->getOrder(nid)->getSide() << 
                            "," << 
                            std::setiosflags(std::ios_base::fixed) << size << 
                            "," << 
                            std::setprecision(5) << pBook->getOrder(nid)->getPrice() << 
                            "," <<  
                            evtTime << 
                            "\n"; 
                            }
    					    int modifyOk = pBook->modify(nid, size, evtTime, sndTime);
                            if (modifyOk != 1) {
                                std::cerr << "***** MODIFY FAILED: MdEntryID: " << nid << " does not exist in book\n"; 
                            }
                        }
                        else {
                            std::cerr << "***** (M) CAN'T FIND ORDERID: " << nid << " for orig_id: " << id << "\n";
                            std::cerr << *pBook;
                        }
					}
				} else if (action == FIX::MDUpdateAction_DELETE) { // delete  
                    //modifiedBook = pBook->remove(id, time, true); 
                    if (_config.print_debug) { 
                        std::cerr << "***** DEL: " << nid << "\n";
                    }
                    capk::pKOrder pOrder = pBook->getOrder(nid);
                    if (pOrder) {
                        if (_config.is_logging) {
                        *pLog << 
                            "D," << 
                            pBook->getOrder(nid)->getSide() << 
                            "," << 
                            std::setprecision(0) << 
                            std::setiosflags(std::ios_base::fixed) << pBook->getOrder(nid)->getSize() << 
                            "," << 
                            std::setprecision(5) << pBook->getOrder(nid)->getPrice() << 
                            "," << 
                            evtTime << 
                            "\n"; 
                        }
                        if (_config.print_debug) {
                            std::cerr << 
                            "D," << 
                            pBook->getOrder(nid)->getSide() << 
                            "," << 
                            std::setprecision(0) << 
                            std::setiosflags(std::ios_base::fixed) << pBook->getOrder(nid)->getSize() << 
                            "," << 
                            std::setprecision(5) << pBook->getOrder(nid)->getPrice() << 
                            "," << 
                            evtTime << 
                            "\n"; 
                        } 
                        int removeOk = pBook->remove(nid, evtTime, sndTime);
                        if (removeOk != 1) {
                            std::cerr << "***** DELETE FAILED: MdEntryID: " << nid << " does not exist in book\n"; 
                        }
                    }
                    else { 
                        std::cerr << "***** (D) CAN'T FIND ORDERID: " << nid << " for orig_id: " << id << "\n";
                        pBook->dbg();
                        assert(0);
                    }

				}
                else { 
                    std::cerr << __FILE__ << ":" <<  __LINE__ << "Unknown action type: " << action << "\n"; 
                    exit(1); 
                }
			}
			else {
				std::cerr << __FILE__ << ":" << __LINE__ << "Can't find orderbook - book is null!" << "\n";
			}
		}

        // Broadcast and log orderbook
        double bbid = pBook->bestPrice(capk::BID);
        double bask = pBook->bestPrice(capk::ASK);
        if(bbid > bask) {
            std::cerr << "XXXXXXXXXXXXXXXX CROSSED BOOK " << pBook->getName() << " (" << (double) bbid <<  ", " << (double) bask << ") XXXXXXXXXXXXXXXXX\n";
            //assert(false);
        }

        double bbsize = pBook->bestPriceVolume(capk::BID);
        double basize = pBook->bestPriceVolume(capk::ASK);
			    
        ptime time_start(microsec_clock::local_time());
        broadcast_bbo_book(_pzmq_socket, 
                symbol.getValue().c_str(),
                bbid,
                bask,
                bbsize, 
                basize, 
                _config.venue_id);

#ifdef DEBUG
		if (pLog == NULL) {
		    std::cerr << __FILE__ <<  ":"  << __LINE__ << "Can't find log - log is null!" << "\n";
        } 
#endif
        if (_config.is_logging && pLog != NULL) {
            // KTK MOVED TO MATCH CONFLUENCE 12/9/2011
            *pLog << "OB," 
                << pBook->getName() 
                << "," 
                << pBook->getEventTime() 
                << "," 
                << pBook->getExchangeSendTime() 
                << "\n";
		    *pLog << *pBook;
        }
        
        ptime time_end(microsec_clock::local_time());
        time_duration duration(time_end - time_start);
        dsum += duration.total_microseconds();
        dcount ++ ;
        dbar = dsum / dcount;
        std::cout << "Mean time(us) to broadcast and write log: " << dbar << "\n";
        std::cout << "SANITY CHECK: " << to_simple_string(duration) << "\n";
	}
}

/* 
 * Incremental refresh message - started once initial orderbook has been built
 * This is the FIX 35=X message
 */
void 
Application::onMessage(const FIX44::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID) 
{

	if (_config.print_debug) { 
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX44::MarketDataIncrementalRefresh& message, ...)\n", message.toString(), "\n");
#endif
	}
    this->incremental_update_template<FIX44::MarketDataIncrementalRefresh>(message,sessionID); 
}

void 
Application::onMessage(const FIX43::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID) 
{

	if (_config.print_debug) { 
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX43::MarketDataIncrementalRefresh& message, ...)\n", message.toString(), "\n");
#endif
	}
    this->incremental_update_template<FIX43::MarketDataIncrementalRefresh>(message,sessionID); 
}

void 
Application::onMessage(const FIX42::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID) 
{

	if (_config.print_debug) { 
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX42::MarketDataIncrementalRefresh& message, ...)\n", message.toString(), "\n");
#endif
	}
    this->incremental_update_template<FIX42::MarketDataIncrementalRefresh>(message,sessionID); 
}


void 
Application::onMessage(const FIX44::MarketDataRequestReject& message, const FIX::SessionID& sessionID) 
{
	if (_config.print_debug) { 
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX44::MarketDataRequestReject& message, ...)\n", message.toString(), "\n");
#endif
    }
}

void 
Application::onMessage(const FIX43::MarketDataRequestReject& message, const FIX::SessionID& sessionID) 
{
	if (_config.print_debug) { 
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX43::MarketDataRequestReject& message, ...)\n", message.toString(), "\n");
#endif
    }
}

void 
Application::onMessage(const FIX42::MarketDataRequestReject& message, const FIX::SessionID& sessionID) 
{
	if (_config.print_debug) { 
#ifdef LOG
        pan::log_DEBUG("onMessage(const FIX42::MarketDataRequestReject& message, ...)\n", message.toString(), "\n");
#endif
    }
}


void
Application::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID) 
{
	FIX::MsgType msgType;
	message.getHeader().getField(msgType);
	if (msgType.getValue() == "1") { 
		if (_config.print_debug) { 
			std::cerr << "Sending TestRequest" << "\n";
		}
	}	
	if (msgType.getValue() == "2") {
		if (_config.print_debug) {
			std::cerr << "Sending ResendRequest" << "\n";
		}
	}
	if (msgType.getValue() == "3") {
		if (_config.print_debug) {
            std::cerr << "*******************************************************\n";
            std::cerr << "*******************************************************\n";
            std::cerr << "*******************************************************\n";
            std::cerr << "*******************************************************\n";
            std::cerr << "*******************************************************\n";
			std::cerr << "Sending Reject" << "\n";
		}
    }
	if (msgType.getValue() == "0") { 
		if (_config.print_debug) { 
			std::cerr << "Sending Heartbeat" << "\n";
		}
	}	
	if (msgType.getValue() == "4") {
		if (_config.print_debug) { 
			std::cerr << "Sending SequenceReset" << "(" << message << ")" << "\n";
		}
	}
	// logon message 
	if (msgType.getValue() == "A") {
		FIX::Header& header = message.getHeader(); 

		// some exchanges want both a password and username,
		// put them in the fields designated by FIX4.3, 
		// others want just a password in the FIX4.3 field 554
		// and others want a password in the raw data field 
		if (_config.username.length() > 0) { 
			header.setField(FIX::FIELD::Username, _config.username);
		}
		if (_config.sendPasswordInRawDataField) { 
			//otherwise put the password in FIX4.2's rawdata field 
			header.setField(FIX::RawData(_config.password.c_str()));	
			header.setField(FIX::RawDataLength(_config.password.length()));
		} else { 
            if (_config.password != "") {
			    header.setField(FIX::FIELD::Password, _config.password); 
            }
		} 
		if (_config.reset_seq_nums) {
			std::cerr << "Sending reset sequence number request (141=Y)" << std::endl;
            FIX::ResetSeqNumFlag flag = FIX::ResetSeqNumFlag_YES;
			message.setField(FIX::FIELD::ResetSeqNumFlag, "Y");
		}
	}
	if (_config.print_debug) { 
        pan::log_DEBUG("Sending: ", message.toString());
	}
}

std::string date_to_string(const dt::date& d)
{
  dt::date_facet* facet = new dt::date_facet("%Y_%m_%d"); 
  std::stringstream ss;
  ss.imbue(std::locale(std::cout.getloc(), facet));
  ss << d;
  return ss.str();
}

bool is_bad_filename_char(char c) { 
    return c == '/' || c == '\\' || c == ' ';
} 

std::string  remove_bad_filename_chars(const std::string& str) { 
    std::string s(str.c_str());
    s.erase(remove_if(s.begin(), s.end(), is_bad_filename_char), s.end()); 
    return s; 
}

void 
Application::run()
{
	std::cout << "Application::run()" << "\n";

	std::vector<std::string>::const_iterator it = _symbols.begin();

    std::string logFileName;
	std::ofstream* pLog;
    capk::KBook* pBook = NULL;
	std::string MIC_prefix = _config.mic_string.length() > 0 ? _config.mic_string + "_" : ""; 
	std::string symbol; 
	fs::path fullPathToLog;
    boost::gregorian::date today;
    std::string dateToday;
    fs::path datePath;

    if (_config.is_logging) {
        today = boost::gregorian::day_clock::universal_day();
        dateToday = date_to_string(today); 
        //	std::string dateToday =  to_iso_string(d); 
	    _pathToLog = fs::path(_config.order_books_output_dir);
        datePath = _pathToLog / fs::path(dateToday); 
        if (!fs::exists(datePath)) { 
            fs::create_directory(datePath); 
        }
    }


	// KTK - Create hash of orderbooks here - OK
	// Create for each symbol:
	// 1) OrderBook
	// 2) Log file	
	while(it != _symbols.end() && *it != "") {
        bool isRestart = false;
		symbol = *it; 
		//pBook = new capitalk::PriceDepthOrderBook(symbol, 5);	
		pBook = new capk::KBook(symbol.c_str(), _config.market_depth);	
		std::cerr << 
            "Created new order book: " 
            << symbol 
            << "(" << pBook->getDepth() << ")" 
            << "\n";
        if (_config.is_logging) {
		    logFileName = MIC_prefix + remove_bad_filename_chars(symbol) + "_" + dateToday;
		    logFileName.append(".csv");
            fullPathToLog = datePath / fs::path(logFileName);
            if (fs::exists(fullPathToLog)) {
                isRestart = true; 
            }
		    pLog =  new std::ofstream(fullPathToLog.string(), std::ios::app | std::ios::out);
            if (isRestart) { 
                timespec evtTime;
                clock_gettime(CLOCK_MONOTONIC, &evtTime);
                if (_config.is_logging) {
                    *pLog << "RESTART: " << evtTime << "\n";
                }
		            std::cout << "Appening to log for: " 
                        << symbol  
                        << " as (" 
                        << fullPathToLog.string() 
                        << ") " 
                        << pLog->is_open()   
                        << "\n";
            }
            else {
		        std::cout << "Created new log for: " 
                    << symbol  
                    << " as (" << fullPathToLog.string() 
                    << ") " 
                    << pLog->is_open() 
                    << "\n";
                timespec evtTime;
                clock_gettime(CLOCK_MONOTONIC, &evtTime);
                ///if (_config.is_logging) {
                *pLog << pBook->getOutputVersionString() << "," << pBook->getName() << "," << pBook->getDepth() << "," << evtTime << "\n"; 
                ///}
            }
		    addStream(symbol, pLog);
        } 
		addBook(symbol, pBook);
		std::cout << "NUM BOOKS: " << _symbolToBook.size() << "\n";
		it++;
	}


	// Some venues require each market data subscription to be sent in a different message 
	// - i.e. when we send 35=V we are ONLY allowed to send 146(NoRelatedSymbols)=1 so we send multiple 
	// 35=V requests
	
	if (_config.sendIndividualMarketDataRequests) {
		for (std::vector<std::string>::const_iterator it = _symbols.begin(); it != _symbols.end(); it++) {
			if (*it != "") {
				querySingleMarketDataRequest(*it);		
			}
		}
	}
	else {
		queryMarketDataRequest(_symbols);
	}
}

void 
Application::sendTestRequest()
{
	if (_config.print_debug) { 
		std::cout << "Application::sendTestRequest()" << "\n";
	}

	FIX::Message testRequestMessage;
	switch(_config.version) {
	case 42: testRequestMessage = sendTestRequest42();
		break;
	case 43: testRequestMessage = sendTestRequest43();
		break;
	case 44: testRequestMessage = sendTestRequest44();
		break;
	default: 
		throw std::runtime_error("Unsupported FIX version"); 
	}
	FIX::Session::sendToTarget(testRequestMessage, _sessionID.getSenderCompID(), _sessionID.getTargetCompID());
}

FIX44::TestRequest 
Application::sendTestRequest44() 
{
	if (_config.print_debug) { 
		std::cout << "Application::sendTestRequest44()" << "\n";
	}
	FIX44::TestRequest tr;
	FIX::TestReqID trid("TestRequest");
	tr.setField(trid);
	return tr;
}

FIX43::TestRequest 
Application::sendTestRequest43() 
{
	if (_config.print_debug) { 
		std::cout << "Application::sendTestRequest43()" << "\n";
	}
	FIX43::TestRequest tr;
	FIX::TestReqID trid("TestRequest");
	tr.setField(trid);
	return tr;
}

FIX42::TestRequest 
Application::sendTestRequest42() 
{
	if (_config.print_debug) { 
		std::cout << "Application::sendTestRequest42()" << "\n";
	}
	FIX42::TestRequest tr;
	FIX::TestReqID trid("TestRequest");
	tr.setField(trid);
	return tr;
}

void 
Application::querySingleMarketDataRequest(const std::string& requestSymbol)
{
	if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("querySingleMarketDataRequest(...)");
#endif
    }
	FIX::Message md;
	switch(_config.version) {
	case 42: md = querySingleMarketDataRequest42(requestSymbol);
		break;
	case 43: md = querySingleMarketDataRequest43(requestSymbol);
		break;
	case 44: md = querySingleMarketDataRequest44(requestSymbol);
		break;
	default: 
		throw std::runtime_error("Unsupported FIX version"); 
	}
	FIX::Session::sendToTarget(md);
}

FIX44::MarketDataRequest 
Application::querySingleMarketDataRequest44(const std::string& requestSymbol)
{

	if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("querySingleMarketDataRequest44(", requestSymbol.c_str(), ")");
#endif
    }
    std::string reqID("CAPK-");
    reqID += requestSymbol;
	FIX::MDReqID mdReqID(reqID.c_str());

	FIX::SubscriptionRequestType 
	  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
    // KTK TODO - pull the depth into symbols file
	FIX::MarketDepth marketDepth(_config.market_depth);
	FIX44::MarketDataRequest message(mdReqID, subType, marketDepth);
	
	if (_config.is_aggregated_book) { 
        message.set(FIX::AggregatedBook(true));
	} 
    else {
        message.set(FIX::AggregatedBook(false));
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

FIX43::MarketDataRequest 
Application::querySingleMarketDataRequest43(const std::string& requestSymbol)
{

	if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("querySingleMarketDataRequest43(", requestSymbol.c_str(), ")");
#endif
    }
    std::string reqID("CAPK-");
    reqID += requestSymbol;
	FIX::MDReqID mdReqID(reqID.c_str());

	FIX::SubscriptionRequestType 
	  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
    // KTK TODO - pull the depth into symbols file
	FIX::MarketDepth marketDepth(_config.market_depth);
	FIX43::MarketDataRequest message(mdReqID, subType, marketDepth);
	
	if (_config.is_aggregated_book) { 
        message.set(FIX::AggregatedBook(true));
	} 
    else {
        message.set(FIX::AggregatedBook(false));
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

FIX42::MarketDataRequest 
Application::querySingleMarketDataRequest42(const std::string& requestSymbol)
{
	if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("querySingleMarketDataRequest42(", requestSymbol.c_str(), ")");
#endif
    }
    std::string reqID("CAPK-");
    reqID += requestSymbol;
	FIX::MDReqID mdReqID(reqID.c_str());

	FIX::SubscriptionRequestType 
	  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
// KTK TODO - pull the depth into symbols file
	FIX::MarketDepth marketDepth(_config.market_depth);
	FIX42::MarketDataRequest message(mdReqID, subType, marketDepth);

	if (_config.is_aggregated_book) { 
          message.set(FIX::AggregatedBook(true));
	} 
    else {
        message.set(FIX::AggregatedBook(false));
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


void 
Application::queryMarketDataRequest(const std::vector<std::string>& symbols)
{
    if (_config.print_debug) {
	    std::cout << "Application::queryMarketDataRequest(const std::vector<std::string>& symbols)" << "\n";
    }
	FIX::Message md;
	switch(_config.version) {
	case 42: 
        md = queryMarketDataRequest42(symbols);
		break;
	case 43: 
        md = queryMarketDataRequest43(symbols);
		break;
	case 44: 
        md = queryMarketDataRequest44(symbols);
		break;
	default:
		throw std::runtime_error("Unsupported FIX version"); 
	}
	FIX::Session::sendToTarget(md);
}

FIX44::MarketDataRequest 
Application::queryMarketDataRequest44(const std::vector<std::string>& symbols)
{

    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("queryMarketDataRequest44(", pan::integer(symbols.size()), " symbols)");
#endif
    }
	FIX::MDReqID mdReqID("CAPK");

	FIX::SubscriptionRequestType 
	  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
	FIX::MarketDepth marketDepth(_config.market_depth);
	FIX44::MarketDataRequest message(mdReqID, subType, marketDepth);
	
    message.set(FIX::AggregatedBook(_config.is_aggregated_book));
	
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
	for (std::vector<std::string>::const_iterator it = symbols.begin(); it != symbols.end(); ++it) {
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


FIX43::MarketDataRequest 
Application::queryMarketDataRequest43(const std::vector<std::string>& symbols)
{

    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("queryMarketDataRequest43(", pan::integer(symbols.size()), " symbols)");
#endif
    }
	FIX::MDReqID mdReqID("CAPK");

	FIX::SubscriptionRequestType 
	  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
	FIX::MarketDepth marketDepth(_config.market_depth);
	FIX43::MarketDataRequest message(mdReqID, subType, marketDepth);
	
    message.set(FIX::AggregatedBook(_config.is_aggregated_book));
	
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
	for (std::vector<std::string>::const_iterator it = symbols.begin(); it != symbols.end(); ++it) {
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

FIX42::MarketDataRequest 
Application::queryMarketDataRequest42(const std::vector<std::string>& symbols)
{

    if (_config.print_debug) {
#ifdef LOG
        pan::log_DEBUG("queryMarketDataRequest42(", pan::integer(symbols.size()), " symbols)");
#endif
    }
	FIX::MDReqID mdReqID("CAPK");

	FIX::SubscriptionRequestType 
	  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
	FIX::MarketDepth marketDepth(_config.market_depth);
	FIX42::MarketDataRequest message(mdReqID, subType, marketDepth);
	
    message.set(FIX::AggregatedBook(_config.is_aggregated_book));
	
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
	for (std::vector<std::string>::const_iterator it = symbols.begin(); it != symbols.end(); ++it) {
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

void 
Application::addSymbols(const std::vector<std::string>& symbols)
{
	_symbols = symbols;
}

const std::vector<std::string>& 
Application::getSymbols()
{
	return _symbols;
}

void 
Application::addStream(const std::string& symbol, std::ostream* log) 
{
	_symbolToLogStream[symbol] = log;
}

std::ostream*
Application::getStream(const std::string& symbol) 
{
	symbolToLogStreamIterator it = _symbolToLogStream.find(symbol);
	if (it == _symbolToLogStream.end()) {
		return NULL;
	}		
	else {
		return (it->second);
	}
}

void 
Application::addBook(const std::string& symbol, capk::KBook* book) 
{
#ifdef LOG
    pan::log_DEBUG("Adding book for symbol: ", symbol.c_str());
#endif
	_symbolToBook[symbol] = book;
}

capk::KBook*
Application::getBook(const std::string& symbol) 
{
	symbolToBookIterator it = _symbolToBook.find(symbol);
	if (it == _symbolToBook.end()) {
		return NULL;
	}		
	else {
		return (it->second);
	}
}

void 
Application::flushLogs()
{
    if (_config.print_debug)  {
#ifdef LOG
        pan::log_DEBUG("Flushing streams");
#endif
    }
	symbolToLogStreamIterator streamIter = _symbolToLogStream.begin();
	while(streamIter != _symbolToLogStream.end()) {
		std::ostream* stream = streamIter->second;
		if (stream) {
			std::flush(*stream);
			delete stream;
		}
		streamIter++;
	}
}

void
Application::deleteBooks()
{
    if (_config.print_debug)  {
#ifdef LOG
        pan::log_DEBUG("Deleting books");
#endif
    }
	symbolToBookIterator books = _symbolToBook.begin();
	while(books != _symbolToBook.end()) {
		if (books->second) {
			delete books->second;
		}
		books++;
	}	
}

Application::~Application() 
{
    deleteBooks();
    flushLogs();
}
