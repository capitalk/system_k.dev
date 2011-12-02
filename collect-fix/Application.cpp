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

#ifdef _MSC_VER
#pragma warning(disable : 4503 4355 4786 )
#else
//#include "config.h"
#endif

#include "Application.h"
#include "KFixFields.h"
#include "quickfix/Session.h"
#include "quickfix/FieldConvertors.h"
//#include "VenueSpecificFields.h"
#include "utils/KTimeUtils.h"
#include "utils/FIXConvertors.h"
#include "utils/hash.cpp"
#include "order_book/KBook.h"
#include <iostream>

#include <boost/lexical_cast.hpp>

namespace fs = boost::filesystem; 
namespace dt = boost::gregorian; 


//using namespace capitalk;

void Application::onLogon(const FIX::SessionID& sessionID )
{
	_loggedIn = true;
	_loggedOut = false;
	_sessionID = sessionID;
	std::cout << "Logged in" << "\n";
	std::cout << "onLogon - " << sessionID << "\n";
	_loginCount++;
	//sendTestRequest();
	run();				
}

void Application::onLogout(const FIX::SessionID& sessionID )
{
	_loggedIn = false;
	_loggedOut = true;
	std::cout << "onLogout - " << sessionID << "\n";
}

void Application::onCreate(const FIX::SessionID& sessionID )
{
	std::cout << "\n" << "onCreate - " << sessionID << "\n";
}

void Application::fromAdmin(const FIX::Message& message, const FIX::SessionID& sessionID)
	throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon )
{
	FIX::BeginString beginString;
	message.getHeader().getField(beginString);
    if (_config.printDebug)  {
	    std::cout << "fromAdmin(" << message << ")" << "\n"; 	
    }
	if (beginString == FIX::BeginString_FIX42) {
		((FIX42::MessageCracker&)(*this)).crack((const FIX42::Message&) message, sessionID);
	}
	else if (beginString == FIX::BeginString_FIX43) {
		((FIX43::MessageCracker&)(*this)).crack((const FIX43::Message&) message, sessionID);
	}
}

void Application::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID )
throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
	if (_config.printDebug) { 
		std::cout << "===>fromApp(" << message << ")" << "\n";
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
}

void Application::toApp(FIX::Message& message, const FIX::SessionID& sessionID )
throw(FIX::DoNotSend )
{
    if (_config.printDebug) {
	    std::cout << "toApp(" << "Message: " << message << " Session: " << sessionID << ")" << "\n";
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
void Application::onMessage(const FIX43::SecurityDefinition& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cerr << "Application::onMessage(const FIX43::SecurityDefinition& message, const FIX::SessionID& sessionID)" << "\n";
    }
	FIX::SecurityReqID securityReqID; 
	FIX::SecurityResponseID securityResponseID; 
	FIX::SecurityResponseType securityResponseType; 
	FIX::TotalNumSecurities totalNumSecurities; 
	FIX::Symbol symbol; 
	FIX::SecurityType securityType; 

	if (message.isSetField(securityReqID)) {
		message.getField(securityReqID);
	}	
	if (message.isSetField(securityResponseID)) {
		message.getField(securityResponseID);
	}	
	if (message.isSetField(securityResponseType)) {
		message.getField(securityResponseType);
	}	
	if (message.isSetField(totalNumSecurities)) {
		message.getField(totalNumSecurities);
		std::cout << "\tReceiving " << totalNumSecurities.getValue() << "\n";
	}	
}

void Application::onMessage(const FIX42::SecurityDefinition& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Application::onMessage(const FIX42::SecurityDefinition& message, const FIX::SessionID& sessionID)" << "\n";
    }
	FIX::SecurityReqID securityReqID; 
	FIX::SecurityResponseID securityResponseID; 
	FIX::SecurityResponseType securityResponseType; 
	FIX::TotalNumSecurities totalNumSecurities; 
	FIX::Symbol symbol; 
	FIX::SecurityType securityType; 

	if (message.isSetField(securityReqID)) {
		message.getField(securityReqID);
	}	
	if (message.isSetField(securityResponseID)) {
		message.getField(securityResponseID);
	}	
	if (message.isSetField(securityResponseType)) {
		message.getField(securityResponseType);
	}	
	if (message.isSetField(totalNumSecurities)) {
		message.getField(totalNumSecurities);
		std::cout << "\tReceiving " << totalNumSecurities.getValue() << "\n";
	}	
}

void Application::onMessage(const FIX43::SecurityStatus& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Application::onMessage(const FIX43::SecurityStatus& message, const FIX::SessionID& sessionID)" << "\n";		
    }
}

void Application::onMessage(const FIX42::SecurityStatus& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Application::onMessage(const FIX42::SecurityStatus& message, const FIX::SessionID& sessionID)" << "\n";		
    }
}

void Application::onMessage(const FIX43::TradingSessionStatus& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Application::onMessage(const FIX43::TradingSessionStatus& message, const FIX::SessionID& sessionID)" << "\n";		
    }
	FIX::TradingSessionID tradingSessionID;
	FIX::TradSesStatus tradSesStatus;
	if (message.isSetField(tradingSessionID)) {
		message.getField(tradingSessionID);
	}
	if (message.isSetField(tradSesStatus)) { 
		message.getField(tradSesStatus);
		if (tradSesStatus.getValue() == FIX::TradSesStatus_OPEN) {
			std::cout << "Trading sessions status is OPEN" << "\n";
		}
		if (tradSesStatus.getValue() == FIX::TradSesStatus_HALTED) {
			std::cout << "Trading sessions status is HALTED" << "\n";
		}
		if (tradSesStatus.getValue() == FIX::TradSesStatus_CLOSED) {
			std::cout << "Trading sessions status is CLOSED" << "\n";
		}
		if (tradSesStatus.getValue() == FIX::TradSesStatus_PREOPEN) {
			std::cout << "Trading sessions status is PREOPEN" << "\n";
		}
	}
}

void Application::onMessage(const FIX42::TradingSessionStatus& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Application::onMessage(const FIX42::TradingSessionStatus& message, const FIX::SessionID& sessionID)" << "\n";		
    }
	FIX::TradingSessionID tradingSessionID;
	FIX::TradSesStatus tradSesStatus;
	if (message.isSetField(tradingSessionID)) {
		message.getField(tradingSessionID);
	}
	if (message.isSetField(tradSesStatus)) { 
		message.getField(tradSesStatus);
		if (tradSesStatus.getValue() == FIX::TradSesStatus_OPEN) {
			std::cout << "Trading sessions status is OPEN" << "\n";
		}
		if (tradSesStatus.getValue() == FIX::TradSesStatus_HALTED) {
			std::cout << "Trading sessions status is HALTED" << "\n";
		}
		if (tradSesStatus.getValue() == FIX::TradSesStatus_CLOSED) {
			std::cout << "Trading sessions status is CLOSED" << "\n";
		}
		if (tradSesStatus.getValue() == FIX::TradSesStatus_PREOPEN) {
			std::cout << "Trading sessions status is PREOPEN" << "\n";
		}
	}
}


void Application::onMessage(const FIX43::TestRequest& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Application::onMessage(const FIX43::TestRequest& message, const FIX::SessionID& sessionID)" << "\n";		
    }
}

void Application::onMessage(const FIX42::TestRequest& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << " Application::onMessage(const FIX42::TestRequest& message, const FIX::SessionID& sessionID)" << "\n";		
    }
}


void Application::onMessage(const FIX43::Heartbeat& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Application::onMessage(const FIX43::Heartbeat& message, const FIX::SessionID& sessionID)(" << message.toString() << ")" << "\n";
    }
}

void Application::onMessage(const FIX42::Heartbeat& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Application::onMessage(const FIX42::Heartbeat& message, const FIX::SessionID& sessionID)(" << message.toString() << ")" << "\n";
    }
}

void Application::onMessage(const FIX43::Logout& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Logout43(" << message.toString() << ")" << "\n";
    }
}

void Application::onMessage(const FIX42::Logout& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Logout42(" << message.toString() << ")" << "\n";
    }
}

void Application::onMessage(const FIX43::Logon& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Logon43(" << message.toString() << ")" << "\n";
    }
}

void Application::onMessage(const FIX42::Logon& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Logon42(" << message.toString() << ")" << "\n";
    }
}

void Application::onMessage(const FIX43::MarketDataRequest& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "MarketDataRequest43(" << message.toString() << ")" << "\n";
    }
}

void Application::onMessage(const FIX42::MarketDataRequest& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "MarketDataRequst42(" << message.toString() << ")" << "\n";
    }
}

void Application::onMessage(const FIX::Message& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Application::onMessage(const FIX::Message& message, const FIX::SessionID& sessionID)" << message.toString() << ")" << "\n";
    }
}
/*
 * Full refresh of order book - all existing orders at all levels 
 * 35=W
 */
void 
Application::onMessage(const FIX43::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Application::onMessage(const FIX43::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID)" << "\n"; //(" << message.toString() << ")" << "\n";
    }
	FIX::MsgType msgType;
	message.getHeader().getField(msgType);
	FIX::SendingTime sendingTime;
	message.getHeader().getField(sendingTime);
	FIX::NoMDEntries noMDEntries;
	FIX::MDEntryType mdEntryType;
	FIX::MDEntryID mdEntryID;

	FIX::MDEntryPx mdEntryPx;
	FIX::MDEntrySize mdEntrySize;
	FIX::QuoteType quoteType;
	FIX::MDEntryOriginator mdEntryOriginator;
	FIX::MinQty minQty;
	FIX::MDEntryPositionNo mdEntryPositionNo;
	FIX::MDReqID mdReqID;
	FIX::ExecInst execInst;
	FIX::QuoteEntryID quoteEntryID;
	FIX::MaturityMonthYear maturityMonthYear;
	FIX::Symbol symbol;
	FIX::SecurityType securityType;
	int nEntries = 0;

	message.getField(mdReqID);
	message.getField(symbol);
	if (message.isSetField(noMDEntries)) {
		try {
			message.getField(noMDEntries); 
			nEntries = noMDEntries.getValue();
            if (_config.printDebug) {
			    std::cout << "==> NoMDEntries: " << nEntries << "\n";
            }
		}
		catch (std::exception& e) {
			std::cerr << e.what() << "\n";
		}
		FIX43::MarketDataSnapshotFullRefresh::NoMDEntries mdEntries;
		
		// Group indexed on 1 not 0
		for (int i = 0; i< nEntries; i++) {
			message.getGroup(i+1, mdEntries);
			if (mdEntries.isSetField(mdEntryType)) {
				mdEntries.getField(mdEntryType);
			}
			if (mdEntries.isSetField(mdEntryID)) {
				mdEntries.getField(mdEntryID);
			}
			if (mdEntries.isSetField(mdEntryPx)) {
				mdEntries.getField(mdEntryPx);
//				std::cout << "====================>mdEntryPx: " << mdEntryPx << "\n"
			}
			else {
				std::cerr << "NO MDEntryPrice SET IN FIX43 SNAPSHOT" << "\n";	
			}
			if (mdEntries.isSetField(mdEntrySize)) {
				mdEntries.getField(mdEntrySize);	
//				std::cout << "====================>mdEntrySize: " << mdEntrySize << "\n";
			}
			else {
				std::cerr << "NO MDEntrySize SET IN FIX43 SNAPSHOT" << "\n";	
			}
			if (mdEntries.isSetField(quoteType)) {
				mdEntries.getField(quoteType);	
                if (_config.printDebug) {
				    std::cout << "QuoteType: " << quoteType.getValue() << "\n"; 
                }
			}
			if (mdEntries.isSetField(mdEntryOriginator)) {
				mdEntries.getField(mdEntryOriginator);	
                if (_config.printDebug) {
				    std::cout << "MDEntryOriginator: " << mdEntryOriginator.getValue() << "\n"; 
                }
			}
			if (mdEntries.isSetField(minQty)) {
				mdEntries.getField(minQty);	
                if (_config.printDebug) {
				    std::cout << "MinQty: " << minQty.getValue() << "\n"; 
                }
			}
			if (mdEntries.isSetField(mdEntryPositionNo)) {
				mdEntries.getField(mdEntryPositionNo);	
                if (_config.printDebug) {
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
			KBook* pBook;
			if (NULL != (pBook = getBook(symbol.getValue())))  {
				FIX::UtcTimeStamp time = FIX::UtcTimeStamp(sendingTime); 
				char side = mdEntryType.getValue(); 
                side_t nside = char2side_t(side);

                std::string id = mdEntryID.getValue(); 
                int nid = hashlittle(id.c_str(), id.size(), 0);

                /* since we sometimes don't get entry IDs in snapshots, 
                   try using the quote entry ID instead 
                 */ 
                if (id.length() == 0) {
                    id = quoteEntryID.getValue(); 
                    nid = hashlittle(id.c_str(), id.size(), 0);
                }				
                else {
                    std::cerr << "mdEntryID not set - AND quoteEntryID not set " << "\n";
                }
                double price = mdEntryPx.getValue(); 
				unsigned int size = mdEntrySize.getValue(); 
				//capitalk::PriceDepthEntry* entry = 
					//new capitalk::PriceDepthEntry(time, time, side, id, price, size); 
				
                //pBook->add(entry); 
                timespec evtTime, sndTime;
                clock_gettime(CLOCK_MONOTONIC, &evtTime);
                FIXConvertors::UTCTimeStampToTimespec(time, &sndTime);
                pBook->add(nid, nside, size, price, evtTime, sndTime);
                if (_config.printDebug) {
                    std::cout << "[FIX 4.3: Full Refresh] Adding ID=" << id << " price=" << price <<  " size=" << size << "\n"; 
                }
                 
			}
            else {
                std::cerr << "[FIX 4.3: Full Refresh] Orderbook is null - nothing to add to" << "\n";
            }
		}
	}
}

/*
 * Full refresh of order book - all existing orders at all levels 
 * 35=W
 */
void 
Application::onMessage(const FIX42::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID) 
{
	if (_config.printDebug) { 
	    std::cout << "Application::onMessage(const FIX42::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID)(" << message.toString() << ")" << "\n";
    }
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
		FIX42::MarketDataSnapshotFullRefresh::NoMDEntries mdEntries;
		
		// Group indexed on 1 not 0
		for (int i = 0; i< nEntries; i++) {
			message.getGroup(i+1, mdEntries);
	        if (mdEntries.isSetField(symbol)) {
		        mdEntries.getField(symbol);	
	        }
			if (mdEntries.isSetField(mdEntryType)) {
				mdEntries.getField(mdEntryType);
			}
			if (mdEntries.isSetField(mdEntryID)) {
				mdEntries.getField(mdEntryID);
			}
            else {
				std::cerr << "NO MDEntryID SET IN FIX42 SNAPSHOT" << "\n";	
            }
			if (mdEntries.isSetField(mdEntryPx)) {
				mdEntries.getField(mdEntryPx);
			}
			else {
				std::cerr << "NO MDEntryPrice SET IN FIX42 SNAPSHOT" << "\n";	
			}
			if (mdEntries.isSetField(mdEntrySize)) {
				mdEntries.getField(mdEntrySize);	
			}
			else {
				std::cerr << "NO MDEntrySize SET IN FIX42 SNAPSHOT" << "\n";	
			}
			if (mdEntries.isSetField(quoteType)) {
				mdEntries.getField(quoteType);	
                if (_config.printDebug) {
				    std::cout << "QuoteType: " << quoteType.getValue() << "\n"; 
                }
			}
			if (mdEntries.isSetField(mdEntryOriginator)) {
				mdEntries.getField(mdEntryOriginator);	
                if (_config.printDebug) {
				    std::cerr << "MDEntryOriginator: " << mdEntryOriginator.getValue() << "\n"; 
                }
			}
			if (mdEntries.isSetField(minQty)) {
				mdEntries.getField(minQty);	
                if (_config.printDebug) {
				    std::cout << "MinQty: " << minQty.getValue() << "\n"; 
                }
			}
			if (mdEntries.isSetField(mdEntryPositionNo)) {
				mdEntries.getField(mdEntryPositionNo);	
                if (_config.printDebug) {
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

			KBook* pBook;
			if (NULL != (pBook = getBook(symbol.getValue())))  {
				FIX::UtcTimeStamp time = FIX::UtcTimeStamp(sendingTime); 
				char side = mdEntryType.getValue(); 
                side_t nside = char2side_t(side);
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
                pBook->add(nid, nside, size, price, evtTime, sndTime);
                if (_config.printDebug) {
                    std::cout << "[FIX4.2: Full Refresh] Adding ID=" << id << " price=" << price <<  " size=" << size << "\n";   
                }
			}
            else {
                std::cerr << "{FIX 4.2: Full Refresh] Orderbook is null - nothing to add to" << "\n";
            }
		}
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
	//if (msgType.getValue() == "X") {
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
	KBook* pBook = NULL;
    std::ostream* pLog = NULL;

	if (message.isSetField(mdReqID)) {
		message.getField(mdReqID);	
	}

    if (message.isSetField(symbol)) {
        message.getField(symbol);
    }
    else {
        //std::cerr << "*************************************** SYMBOL field not set BODY of  35=X" << std::endl;
    }

	if (message.isSetField(noMDEntries)) {
		//try {
			message.getField(noMDEntries); 
			nEntries = noMDEntries.getValue();
		//}
		//catch (std::exception& e) {
			//std::cerr << e.what() << std::endl;
		//}
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
                //std::cerr << "*************************************** MDENTRYID field not set in 35=X" << std::endl;
            }
			if (mdEntries.isSetField(symbol)) {
				mdEntries.getField(symbol);
			}
            else {
                //std::cerr << "*************************************** SYMBOL field not set in FIELD of 35=X" << std::endl;

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
			// KTK - what to do if the price is empty? We need to have an orderbook that is indexed by 
			// MDEntryID and NOT by price.
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
            //bool modifiedBook = false; 
            timespec evtTime, sndTime;
            FIX::UtcTimeStamp time = FIX::UtcTimeStamp(sendingTime);
            FIXConvertors::UTCTimeStampToTimespec(time, &sndTime);
            clock_gettime(CLOCK_MONOTONIC, &evtTime);
		    pLog = getStream(symbol.getValue());

			if (NULL != (pBook = getBook(symbol.getValue())))  {
                *pLog << "OB," << pBook->getName() << "," << pBook->getEventTime() << "," << pBook->getExchangeSendTime() << "\n";
    		    char side = mdEntryType.getValue();
                side_t nside = char2side_t(side);
		        double price = mdEntryPx.getValue();
        		size = mdEntrySize.getValue();

           	    const std::string& id = mdEntryID.getValue();
                uint32_t nid = hashlittle(id.c_str(), id.size(), 0);
                if (_config.printDebug) {
                    std::cerr << "***** ORIG ID: " << id << ", HASH ID: " << nid << "\n";
                }
				int action = mdUpdateAction.getValue(); 
				if (action == FIX::MDUpdateAction_NEW) { // new 
                    if (_config.printDebug) {
                        std::cerr << "***** DEL BEFORE ADD: " << nid << "\n";
                    }
                    int removeOk = pBook->remove(nid, evtTime, sndTime);

                    if (_config.printDebug) {
                        if (removeOk == 0) {
                            std::cerr << "Order: " << nid << " not found - so just add \n"; 
                        }
                        std::cerr << "***** ADD: " << nid << ", " << size  << "@" << price << "\n";
                    }

                    if (!pBook->add(nid, nside, size, price, evtTime, sndTime)) {
                        std::cerr << "Book add failed!!!!!!!!!!\n (" << nid << nside << "(" << side << ")" << size << price << evtTime << sndTime << "\n";
                    }

                    std::cerr << "A," << nside << "," << std::setiosflags(std::ios_base::fixed) << size << "," << std::setprecision(7) << price << "," << evtTime << "\n"; 
                    *pLog << "A," << nside << "," << std::setiosflags(std::ios_base::fixed) << size << "," << std::setprecision(7) << price << "," << evtTime << "\n"; 

				} else if (action == FIX::MDUpdateAction_CHANGE) { // change  
                    size = mdEntrySize.getValue(); 
                    /* if the message contains a MDEntryRefID then 
                       we're renaming an existing entity so we delete the old one (with MDEntryRefID and re-add with mdEntryID) 
                    */ 
			        if (mdEntries.isSetField(mdEntryRefID)) {
			    	    mdEntries.getField(mdEntryRefID);
                        const std::string& refId = mdEntryRefID.getValue(); 
                        if (_config.printDebug) {
                            std::cerr << "*********** Using (mdEntryRefID as mdEntryID) " << mdEntryRefID << ", " << mdEntryID << "\n";
                            if (mdEntryRefID.getValue() != mdEntryID.getValue()) {
                                std::cerr << "********** MISMATCH ENTRY IDS" << "\n";
                            }
                        }
                        
                        uint32_t nrefId = hashlittle(refId.c_str(), refId.size(), 0);
                    
                        if (_config.printDebug) {
                            std::cerr << "***** SYNTHETIC MOD: ";
                            std::cerr << "Removing: " << nrefId << " and re-adding " << "(" << nid << ", " << nside << ", " << size << "@" << price << ")\n";
                        }
                        std::cerr << "M," << nside << "," << std::setiosflags(std::ios_base::fixed) << size << "," << std::setprecision(7) << price << "," << evtTime << "\n"; 
                        *pLog << "M," << nside << "," << std::setiosflags(std::ios_base::fixed) << size << "," << std::setprecision(7) << price << "," << evtTime << "\n"; 
    
                        pBook->remove(nrefId, evtTime, sndTime);
                        pBook->add(nid, nside, size, price, evtTime, sndTime);
			        } else { 
                        if (_config.printDebug) {
                            std::cerr << "***** MOD: " << nid << " to size " << size << "\n";
                        }
                        std::cerr << "M," << pBook->getOrder(nid)->getSide() << "," << size << "," << std::setprecision(7) << pBook->getOrder(nid)->getPrice() << "," << evtTime << "\n"; 
                        *pLog << "M," << pBook->getOrder(nid)->getSide() << "," << size << "," << std::setprecision(7) << pBook->getOrder(nid)->getPrice() << "," <<  evtTime << "\n"; 
    					pBook->modify(nid, size, evtTime, sndTime);
					}
				} else if (action == FIX::MDUpdateAction_DELETE) { // delete  
                    //modifiedBook = pBook->remove(id, time, true); 
                    if (_config.printDebug) { 
                        std::cerr << "***** DEL: " << nid << "\n";
                    }
                    std::cerr << "D," << pBook->getOrder(nid)->getSide() << "," << std::setiosflags(std::ios_base::fixed) << pBook->getOrder(nid)->getSize() << "," << std::setprecision(7) << pBook->getOrder(nid)->getPrice() << "," << evtTime << "\n"; 
                    *pLog << "D," << pBook->getOrder(nid)->getSide() << "," << std::setiosflags(std::ios_base::fixed) << pBook->getOrder(nid)->getSize() << "," << std::setprecision(7) << pBook->getOrder(nid)->getPrice() << "," << evtTime << "\n"; 
                    int removeOk = pBook->remove(nid, evtTime, sndTime);
                    //assert(removeOk == 1);
                    if (removeOk != 1) {
                        std::cerr << "***** DELETE FAILED: MdEntryID: " << nid << " does not exist in book\n"; 
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
        // PRINT BOOK HERE
        if(pBook->bestPrice(BID) > pBook->bestPrice(ASK)) {
            std::cerr << "XXXXXXXXXXXXXXXX CROSSED BOOK (" << pBook->bestPrice(BID) <<  ", " << pBook->bestPrice(ASK) << ") XXXXXXXXXXXXXXXXX\n";
            //assert(false);
        }
			    
		if (pLog == NULL) {
		    std::cerr << __FILE__ <<  ":"  << __LINE__ << "Can't find log - log is null!" << "\n";
        } else {
		    *pLog << *pBook;
            std::cerr << *pBook << "\n";
		}
	}
}

/* 
 * Incremental refresh message - started once initial orderbook has been built
 * This is the FIX 35=X message
 */
void 
Application::onMessage(const FIX43::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID) 
{

	if (_config.printDebug) { 
		std::cout << "Application::onMessage(const FIX43::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID)(" << message.toString() << ")" << "\n";
	}
    this->incremental_update_template<FIX43::MarketDataIncrementalRefresh>(message,sessionID); 
}



void 
Application::onMessage(const FIX42::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID) 
{

	if (_config.printDebug) { 
		std::cout << "Application::onMessage(const FIX42::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID)(" << message.toString() << ")" << "\n";
	}
    this->incremental_update_template<FIX42::MarketDataIncrementalRefresh>(message,sessionID); 
}


void 
Application::onMessage(const FIX43::MarketDataRequestReject& message, const FIX::SessionID& sessionID) 
{
	if (_config.printDebug) { 
	    std::cout << "Application::onMessage(const FIX43::MarketDataRequestReject& message, const FIX::SessionID& sessionID)(" << message.toString() << ")" << "\n";
    }
}

void 
Application::onMessage(const FIX42::MarketDataRequestReject& message, const FIX::SessionID& sessionID) 
{
	if (_config.printDebug) { 
	    std::cout << "Application::onMessage(const FIX42::MarketDataRequestReject& message, const FIX::SessionID& sessionID)(" << message.toString() << ")" << "\n";
    }
}


void
Application::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID) 
{
	if (_config.printDebug) { 
		std::cout << "Application::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID)" << "\n";
	}
	FIX::MsgType msgType;
	message.getHeader().getField(msgType);
	if (msgType.getValue() == "1") { 
		if (_config.printDebug) { 
			std::cerr << "Sending TestRequest" << "\n";
		}
	}	
	if (msgType.getValue() == "2") {
		if (_config.printDebug) {
			std::cerr << "Sending ResendRequest" << "\n";
		}
	}
	if (msgType.getValue() == "0") { 
		if (_config.printDebug) { 
			std::cerr << "Sending Heartbeat" << "\n";
		}
	}	
	if (msgType.getValue() == "4") {
		if (_config.printDebug) { 
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
		if (_resetSequence) {
			//_resetSequence = false;
			std::cerr << "Resetting sequence numbers" << std::endl;
            FIX::ResetSeqNumFlag flag = FIX::ResetSeqNumFlag_YES;
			message.setField(FIX::FIELD::ResetSeqNumFlag, "Y");
		}
/*
		if (_resetSequence) {
			// reset flag
			_resetSequence = false;
			std::cerr << "Resetting sequence numbers" << "\n";
			FIX::ResetSeqNumFlag resetSeqNumFlag;
			std::cerr << "Checking to see if flag already exists...";
			// KTK - how do you set an existing header field to a different one?
			// or do you need to remove it and re-add it? 
			if (message.isSetField(resetSeqNumFlag)) {
				message.getHeader().getField(resetSeqNumFlag);
				std::cerr << "yes. ResetSeqNumFlag is already set to (" << resetSeqNumFlag.getValue() << ")" << "\n";
				message.getHeader().setField(FIX::ResetSeqNumFlag("1"), true);
			}
			else {
				std::cerr << "no. Requesting sequence number reset" << "\n";
				message.getHeader().setField(FIX::ResetSeqNumFlag("1"));
			}
		}
		//message.getHeader().setField(FIX::EncryptMethod(0));
		//message.getHeader().setField(FIX::HeartBtInt(30));
*/
	}
	if (_config.printDebug) { 
		std::cout << "toAdmin(" << "Message: " << message << ")" << "\n";
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
    boost::gregorian::date today = boost::gregorian::day_clock::universal_day();
    std::string dateToday = date_to_string(today); 
//	std::string dateToday =  to_iso_string(d); 
    fs::path datePath = _pathToLog / fs::path(dateToday); 
    if (!fs::exists(datePath)) { fs::create_directory(datePath); }

	// KTK - make sure that this is the date at UTC
	std::cout << "Application::run()" << "\n";

	std::vector<std::string>::const_iterator it = _symbols.begin();
	// KTK - Create hash of orderbooks here - OK
	// Create for each symbol:
	// 1) OrderBook
	// 2) Log file	
	std::string logFileName;
	std::ofstream* pLog;
	//capitalk::PriceDepthOrderBook* pBook = NULL;
	KBook* pBook = NULL;
	std::string MIC_prefix = _config.mic_code.length() > 0 ? _config.mic_code + "_" : ""; 
	std::string symbol; 
	fs::path fullPathToLog;
	while(it != _symbols.end() && *it != "") {
        bool isRestart = false;
		symbol = *it; 
		//pBook = new capitalk::PriceDepthOrderBook(symbol, 5);	
		pBook = new KBook(symbol.c_str(), _config.marketDepth);	
		std::cerr << "Created new order book: " << symbol << "(" << pBook->getDepth() << ")" << "\n";
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
            *pLog << "\nRESTART: " << evtTime << "\n";
		    std::cout << "Appening to log for: " << symbol  << " as (" << fullPathToLog.string() << ") " << pLog->is_open()   << "\n";
        }
        else {
		    std::cout << "Created new log for: " << symbol  << " as (" << fullPathToLog.string() << ") " << pLog->is_open()   << "\n";
            timespec evtTime;
            clock_gettime(CLOCK_MONOTONIC, &evtTime);
            *pLog << pBook->getOutputVersionString() << "," << pBook->getName() << "," << pBook->getDepth() << evtTime << "\n"; 
        }
        
		addStream(symbol, pLog);
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
	if (_config.printDebug) { 
		std::cout << "Application::sendTestRequest()" << "\n";
	}

	FIX::Message testRequestMessage;
	switch(_config.version) {
	case 42: testRequestMessage = sendTestRequest42();
		break;
	case 43: testRequestMessage = sendTestRequest43();
		break;
	default: 
		throw "Unsupport FIX version"; 
	}
	FIX::Session::sendToTarget(testRequestMessage, _sessionID.getSenderCompID(), _sessionID.getTargetCompID());
}

FIX43::TestRequest 
Application::sendTestRequest43() 
{
	if (_config.printDebug) { 
		std::cout << "Application::sendTestRequest43()" << "\n";
	}
	FIX43::TestRequest tr;
	// KTK - change to timestamap at some point - better than static string to debug
	FIX::TestReqID trid("TestRequest");
	tr.setField(trid);
	return tr;
}

FIX42::TestRequest 
Application::sendTestRequest42() 
{
	if (_config.printDebug) { 
		std::cout << "Application::sendTestRequest42()" << "\n";
	}
	FIX42::TestRequest tr;
	// KTK - change to timestamap at some point - better than static string to debug
	FIX::TestReqID trid("TestRequest");
	tr.setField(trid);
	return tr;
}

void 
Application::querySingleMarketDataRequest(const std::string& requestSymbol)
{
	//int version = queryVersion();
	if (_config.printDebug) {
	    std::cout << "Application::querySingleMarketDataRequest(const std::string& requestSymbol)" << "\n";
    }
	FIX::Message md;
	switch(_config.version) {
	case 42: md = querySingleMarketDataRequest42(requestSymbol);
		break;
	case 43: md = querySingleMarketDataRequest43(requestSymbol);
		break;
	default: 
		throw "Unsupported FIX version"; 
	}
	FIX::Session::sendToTarget(md);
}

FIX43::MarketDataRequest 
Application::querySingleMarketDataRequest43(const std::string& requestSymbol)
{

	if (_config.printDebug) {
	    std::cout << "Application::querySingleMarketDataRequest43(const std::string& requestSymbol)(" << requestSymbol << ")" << "\n";
    }
    std::string reqID("CAPK-");
    reqID += requestSymbol;
	FIX::MDReqID mdReqID(reqID.c_str());

	FIX::SubscriptionRequestType 
	  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
// KTK TODO - pull the depth into symbols file
	FIX::MarketDepth marketDepth(1);
	FIX43::MarketDataRequest message(mdReqID, subType, marketDepth);
	
	if (_config.aggregatedBook) { 
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

	FIX::MDUpdateType updateType(FIX::MDUpdateType_INCREMENTAL_REFRESH);
	message.set(updateType);

	message.getHeader().setField(_sessionID.getSenderCompID());
	message.getHeader().setField(_sessionID.getTargetCompID());

	return message;
}

FIX42::MarketDataRequest 
Application::querySingleMarketDataRequest42(const std::string& requestSymbol)
{
	if (_config.printDebug) {
	    std::cerr << "Application::querySingleMarketDataRequest42(const std::string& requestSymbol)(" << requestSymbol << ")" << "\n";
    }
    std::string reqID("CAPK-");
    reqID += requestSymbol;
	FIX::MDReqID mdReqID(reqID.c_str());

	FIX::SubscriptionRequestType 
	  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
// KTK TODO - pull the depth into symbols file
	FIX::MarketDepth marketDepth(_config.marketDepth);
	FIX42::MarketDataRequest message(mdReqID, subType, marketDepth);

	if (_config.aggregatedBook) { 
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

	FIX::MDUpdateType updateType(FIX::MDUpdateType_INCREMENTAL_REFRESH);
	message.set(updateType);

/*
    FIX::K_ReplayFile k_replayFile("FIX.4.2-pro5792test-CNX.messages.current.log");
    message.setField(k_replayFile);
    FIX::K_Volatility k_volatility(0.5);
    message.setField(k_volatility);
    FIX::K_ReplayTimeDiv k_replayTimeDiv(2.223);
    message.setField(k_replayTimeDiv);
*/

	message.getHeader().setField(_sessionID.getSenderCompID());
	message.getHeader().setField(_sessionID.getTargetCompID());

	return message;
}


void 
Application::queryMarketDataRequest(const std::vector<std::string>& symbols)
{
	//int version = queryVersion();
    if (_config.printDebug) {
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
	default:
		throw "Unsupported FIX version"; 
	}
	FIX::Session::sendToTarget(md);
}

FIX43::MarketDataRequest 
Application::queryMarketDataRequest43(const std::vector<std::string>& symbols)
{

    if (_config.printDebug) {
    	std::cout << "Application::queryMarketDataRequest43(const std::vector<std::string>& symbols)" << "\n";
    }
	FIX::MDReqID mdReqID("MARKETDATAID");

	FIX::SubscriptionRequestType 
	  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
	FIX::MarketDepth marketDepth(_config.marketDepth);
	FIX43::MarketDataRequest message(mdReqID, subType, marketDepth);
	
	if (_config.aggregatedBook) { 
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
	for (std::vector<std::string>::const_iterator it = symbols.begin(); it != symbols.end(); ++it) {
		// this sucks - use a better check for empty symbols - or better a validator!
		// but again - premature optimization is the root of all evil
		if (*it != "") {
			FIX::Symbol symbol(*it);
			symbolGroup.set(symbol);	
			message.addGroup(symbolGroup);
		}
	}

	FIX::MDUpdateType updateType(FIX::MDUpdateType_INCREMENTAL_REFRESH);
	message.set(updateType);

	//queryHeader(message.getHeader());
	message.getHeader().setField(_sessionID.getSenderCompID());
	message.getHeader().setField(_sessionID.getTargetCompID());


	return message;
}

FIX42::MarketDataRequest 
Application::queryMarketDataRequest42(const std::vector<std::string>& symbols)
{

    if (_config.printDebug) {
	    std::cerr << "Application::queryMarketDataRequest42(const std::vector<std::string>& symbols)" << "\n";
    }
	FIX::MDReqID mdReqID("MARKETDATAID");

	FIX::SubscriptionRequestType 
	  subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
	FIX::MarketDepth marketDepth(0);
	FIX42::MarketDataRequest message(mdReqID, subType, marketDepth);
	
	if (_config.aggregatedBook) { 
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
	for (std::vector<std::string>::const_iterator it = symbols.begin(); it != symbols.end(); ++it) {
		// this sucks - use a better check for empty symbols - or better a validator!
		// but again - premature optimization is the root of all evil
		if (*it != "") {
			FIX::Symbol symbol(*it);
			symbolGroup.set(symbol);	
			message.addGroup(symbolGroup);
		}
	}

	FIX::MDUpdateType updateType(FIX::MDUpdateType_INCREMENTAL_REFRESH);
	message.set(updateType);

	//queryHeader(message.getHeader());
	message.getHeader().setField(_sessionID.getSenderCompID());
	message.getHeader().setField(_sessionID.getTargetCompID());


	return message;
}

void 
Application::setDataPath(const std::string& pathStr) 
{
	_pathToLog = fs::path(pathStr);
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
//Application::addBook(const std::string& symbol, capitalk::PriceDepthOrderBook* book) 
Application::addBook(const std::string& symbol, KBook* book) 
{
	std::cout << "Adding book for symbol: " << symbol << "\n";
	_symbolToBook[symbol] = book;
}

KBook*
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
    if (_config.printDebug)  {
        std::cerr << "Flushing streams\n";
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
    if (_config.printDebug)  {
        std::cerr << "Deleting books\n";
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
