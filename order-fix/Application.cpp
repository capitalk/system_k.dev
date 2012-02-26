/* -*- C++ -*- */

/******************************************************************************
  Copyright (C) 2011,2012 Capital K Partners BV
   ______            _ __        __   __ __
  / ____/___ _____  (_) /_____ _/ /  / //_/
 / /   / __ `/ __ \/ / __/ __ `/ /  / ,<   
/ /___/ /_/ / /_/ / / /_/ /_/ / /  / /| |  
\____/\__,_/ .___/_/\__/\__,_/_/  /_/ |_|  
          /_/                            

 No part of this code may be reproduced in whole or in part without express
 permission from Capital K partners.  

 Think. 

******************************************************************************/

#ifdef _MSC_VER
#pragma warning(disable : 4503 4355 4786 )
#else
//#include "config.h"
#endif

#include "Application.h"
#include "quickfix/Session.h"
#include "quickfix/FieldConvertors.h"
#include "utils/KTimeUtils.h"
#include "utils/FIXConvertors.h"
#include "utils/hash.cpp"

#include <iostream>


// For google protobufs
//#include "../proto/spot_fx_md_1.pb.h"

namespace fs = boost::filesystem; 
namespace dt = boost::gregorian; 

Application::Application(bool bReset, const ApplicationConfig& config) 
         :  _loggedIn(false), _loggedOut(false), _loginCount(0), 
            _appMsgCount(0),  _config(config), _resetSequence(bReset)
{
	_handlInst = -1;
	_account = "";
	START_PTIME
	#ifdef LOG
	pan::log_INFORMATIONAL("Application::Application()");
	#endif
	STOP_PTIME
	DIFF_PTIME
	pan::log_DEBUG(ptime_string(ptime_duration));
}

/*****************************************************************************
 * VIRTUAL IMPLEMENTATIONS 
 ****************************************************************************/
void Application::onLogon(const FIX::SessionID& sessionID )
{
	_loggedIn = true;
	_loggedOut = false;
	_sessionID = sessionID;
	_loginCount++;
	#ifdef LOG
	pan::log_INFORMATIONAL("onLogon:", sessionID.toString().c_str());
	#endif
}

void Application::onLogout(const FIX::SessionID& sessionID )
{
	_loggedIn = false;
	_loggedOut = true;
	#ifdef LOG
	pan::log_INFORMATIONAL("onLogout:", sessionID.toString().c_str());
	#endif
}

void Application::onCreate(const FIX::SessionID& sessionID )
{
	#ifdef LOG
	pan::log_INFORMATIONAL("onCreate:", sessionID.toString().c_str());
	#endif
}

/*****************************************************************************
 * ADMIN 
 ****************************************************************************/
void Application::fromAdmin(const FIX::Message& message, 
                            const FIX::SessionID& sessionID)
	throw(FIX::FieldNotFound, 
    FIX::IncorrectDataFormat, 
    FIX::IncorrectTagValue, 
    FIX::RejectLogon )
{
	FIX::BeginString beginString;
	message.getHeader().getField(beginString);
	#ifdef LOG
	pan::log_DEBUG("ADMIN MSG(", message.toString() ,")");
    #endif 
    crack(message, sessionID);
/*
	if (beginString == FIX::BeginString_FIX42) {
		((FIX42::MessageCracker&)(*this)).crack((const FIX42::Message&) message, sessionID);
	}
	else if (beginString == FIX::BeginString_FIX43) {
		((FIX43::MessageCracker&)(*this)).crack((const FIX43::Message&) message, sessionID);
	}
	else if (beginString == FIX::BeginString_FIX44) {
		((FIX44::MessageCracker&)(*this)).crack((const FIX44::Message&) message, sessionID);
	}
	else if (beginString == FIX::BeginString_FIX50) {
		((FIX50::MessageCracker&)(*this)).crack((const FIX50::Message&) message, sessionID);
	}
*/
}

void
Application::toAdmin(FIX::Message& message, 
                    const FIX::SessionID& sessionID) 
{
	FIX::MsgType msgType;
	message.getHeader().getField(msgType);
	if (msgType.getValue() == "1") { 
		#ifdef LOG
		pan::log_DEBUG("Sending TestRequest");
		#endif 
	}	
	if (msgType.getValue() == "2") {
		#ifdef LOG
		pan::log_DEBUG("Sending ResendRequest");
		#endif 
	}
	if (msgType.getValue() == "3") {
		#ifdef LOG
		pan::log_DEBUG("Sending Reject");
		pan::log_CRITICAL("Sending Reject");
		#endif 
    }
	if (msgType.getValue() == "0") { 
		#ifdef LOG
		pan::log_DEBUG("Sending Heartbeat");
		#endif 
	}	
	if (msgType.getValue() == "4") {
		#ifdef LOG
		pan::log_DEBUG("Sending SequenceReset(", message.toString(), ")");
		#endif 
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
			#ifdef LOG
			pan::log_DEBUG("Resetting sequence numbers");
			#endif 
            FIX::ResetSeqNumFlag flag = FIX::ResetSeqNumFlag_YES;
			message.setField(FIX::FIELD::ResetSeqNumFlag, "Y");
		}
	}
	#ifdef LOG
		//pan::log_DEBUG("toAdmin(", message.toString(), ")");
	#endif
}

void Application::fromApp(const FIX::Message& message, 
                        const FIX::SessionID& sessionID )
    throw(FIX::FieldNotFound, 
        FIX::IncorrectDataFormat, 
        FIX::IncorrectTagValue, 
        FIX::UnsupportedMessageType )
{
	#ifdef LOG
		//pan::log_DEBUG("fromApp(", message.toString(), ")");
	#endif
    _appMsgCount++;
    if (_appMsgCount % 10000 == 0) {
		std::cout << "Received: " << _appMsgCount << " application messages\n";
		#ifdef LOG
		pan::log_NOTICE("Received: ", pan::integer(_appMsgCount), " application messages"); 
		#endif
	}
    crack(message, sessionID);
/*
	FIX::BeginString beginString;
	message.getHeader().getField(beginString);
	if (beginString == FIX::BeginString_FIX42) {
		((FIX42::MessageCracker&)(*this)).crack((const FIX42::Message&) message, sessionID);
	}
	else if (beginString == FIX::BeginString_FIX43) {
		((FIX43::MessageCracker&)(*this)).crack((const FIX43::Message&) message, sessionID);
	}
*/
}

void 
Application::toApp(FIX::Message& message, 
                        const FIX::SessionID& sessionID )
    throw(FIX::DoNotSend )
{
	#ifdef LOG
	    //pan::log_DEBUG("toApp(", message.toString() , ")");
    #endif
	try {
		// Don't send potenially duplicate requests for market data
		FIX::PossDupFlag possDupFlag;
		if (message.isSetField(possDupFlag)) {
			message.getHeader().getField(possDupFlag );
			if (possDupFlag ) throw FIX::DoNotSend();
		}
	}
	catch (FIX::FieldNotFound& e) {
		#ifdef LOG
		pan::log_CRITICAL("toApp - Field not found (" , 
							e.what(), 
							e.detail.c_str(), 
							pan::integer(e.field), ")") ;	
		#endif
	}
	catch (FIX::Exception& e) {
		#ifdef LOG
		pan::log_CRITICAL("toApp - Exception (" , e.what(), ")") ;	
		#endif
	}

}

/*****************************************************************************
 * TRADING SESSION STATUS
 ****************************************************************************/
void
printTradSesStatus(FIX::TradingSessionID id, 
                    FIX::TradSesStatus status) 
{
    #ifdef LOG
	pan::log_NOTICE("Trading session ID: ", id.getString().c_str()); 
	#endif
    if (status.getValue() == FIX::TradSesStatus_OPEN) {
		#ifdef LOG
		pan::log_NOTICE("Trading session status is OPEN"); 
		#endif
    }
    if (status.getValue() == FIX::TradSesStatus_HALTED) {
		#ifdef LOG
		pan::log_NOTICE("Trading session status is HALTED"); 
		#endif
    }
    if (status.getValue() == FIX::TradSesStatus_CLOSED) {
		#ifdef LOG
		pan::log_NOTICE("Trading session status is CLOSED"); 
		#endif
    }
    if (status.getValue() == FIX::TradSesStatus_PREOPEN) {
		#ifdef LOG
		pan::log_NOTICE("Trading session status is PREOPEN"); 
		#endif
    }
}

void 
Application::onMessage(const FIX42::TradingSessionStatus& message, 
                        const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG(message.toString());		
    #endif
	FIX::TradingSessionID tradingSessionID;
	FIX::TradSesStatus tradSesStatus;
	if (message.isSetField(tradingSessionID)) {
		message.getField(tradingSessionID);
	}
	if (message.isSetField(tradSesStatus)) { 
		message.getField(tradSesStatus);
	}
    printTradSesStatus(tradingSessionID, tradSesStatus);
}

void 
Application::onMessage(const FIX43::TradingSessionStatus& message, 
                            const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG(message.toString());		
    #endif
	FIX::TradingSessionID tradingSessionID;
	FIX::TradSesStatus tradSesStatus;

	if (message.isSetField(tradingSessionID)) {
		message.getField(tradingSessionID);
	}
	if (message.isSetField(tradSesStatus)) { 
		message.getField(tradSesStatus);
	}
    printTradSesStatus(tradingSessionID, tradSesStatus);
}

void 
Application::onMessage(const FIX44::TradingSessionStatus& message, 
                            const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG(message.toString());		
    #endif
	FIX::TradingSessionID tradingSessionID;
	FIX::TradSesStatus tradSesStatus;

	if (message.isSetField(tradingSessionID)) {
		message.getField(tradingSessionID);
	}
	if (message.isSetField(tradSesStatus)) { 
		message.getField(tradSesStatus);
	}
    printTradSesStatus(tradingSessionID, tradSesStatus);
}

void 
Application::onMessage(const FIX50::TradingSessionStatus& message, 
                            const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG(message.toString());		
    #endif
	FIX::TradingSessionID tradingSessionID;
	FIX::TradSesStatus tradSesStatus;

	if (message.isSetField(tradingSessionID)) {
		message.getField(tradingSessionID);
	}
	if (message.isSetField(tradSesStatus)) { 
		message.getField(tradSesStatus);
	}
    printTradSesStatus(tradingSessionID, tradSesStatus);
}

/*****************************************************************************
 * HEARTBEAT
 ****************************************************************************/

void 
Application::onMessage(const FIX42::Heartbeat& message, 
                        const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG(message.toString());		
    #endif
}

void 
Application::onMessage(const FIX43::Heartbeat& message, 
                        const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG(message.toString());		
    #endif
}

void 
Application::onMessage(const FIX44::Heartbeat& message, 
                        const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG(message.toString());		
    #endif
}

void 
Application::onMessage(const FIXT11::Heartbeat& message, 
                        const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG(message.toString());		
    #endif
}
/*****************************************************************************
 * LOGON/LOGOUT
 ****************************************************************************/
void 
Application::onMessage(const FIX42::Logon& message, 
                        const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG("Logon:FIX42 ", message.toString());		
    #endif
}

void 
Application::onMessage(const FIX43::Logon& message, 
                        const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG("Logon:FIX43 ", message.toString());		
    #endif
}

void 
Application::onMessage(const FIX44::Logon& message, 
                        const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG("Logon:FIX44 ", message.toString());		
    #endif
}

void 
Application::onMessage(const FIXT11::Logon& message, 
                        const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG("Logon:FIXT11 ", message.toString());		
    #endif
}

void 
Application::onMessage(const FIX42::Logout& message, 
                        const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG("Logout:FIX42 ",message.toString());		
    #endif
}

void 
Application::onMessage(const FIX43::Logout& message, 
                        const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG("Logout:FIX43 ", message.toString());		
    #endif
}

void 
Application::onMessage(const FIX44::Logout& message, 
                        const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG("Logout:FIX44", message.toString());		
    #endif
}

void 
Application::onMessage(const FIXT11::Logout& message, 
                        const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG("Logout:FIXT11", message.toString());		
    #endif
}

void 
Application::onMessage(const FIX42::ExecutionReport& message, 
						const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG( message.toString());		
    #endif
}

void 
Application::onMessage(const FIX42::OrderCancelReject& message, 
						const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG( message.toString());		
    #endif

}

void
Application::onMessage(const FIX43::ExecutionReport& message, 
						const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG( message.toString());		
    #endif

}

void 
Application::onMessage(const FIX43::OrderCancelReject& message, 
						const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG( message.toString());		
    #endif

}

void 
Application::onMessage(const FIX44::ExecutionReport& message, 
						const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG( message.toString());		
    #endif

}

void 
Application::onMessage(const FIX44::OrderCancelReject& message, 
						const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG( message.toString());		
    #endif

}

void 
Application::onMessage(const FIX50::ExecutionReport& message, 
						const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG( message.toString());		
    #endif

}

void 
Application::onMessage(const FIX50::OrderCancelReject& message, 
						const FIX::SessionID& sessionID) 
{
	#ifdef LOG
	pan::log_DEBUG( message.toString());		
    #endif

}


void Application::queryEnterOrder()
{
  int version = queryVersion();
  std::cout << "\nNewOrderSingle\n";
  FIX::Message order;

  switch ( version ) {
  case 42:
    order = queryNewOrderSingle42();
    break;
  case 43:
    order = queryNewOrderSingle43();
    break;
  case 44:
    order = queryNewOrderSingle44();
    break;
  case 50:
    order = queryNewOrderSingle50();
    break;
  default:
    std::cerr << "No test for version " << version << std::endl;
    break;
  }

	if ( queryConfirm( "Send order" ) ) {
		FIX::Session::sendToTarget( order );
		#ifdef LOG
		pan::log_DEBUG(order.toString());
		#endif
	}
	else {
		pan::log_DEBUG("Not sending order");
	}
}

void Application::queryCancelOrder()
{
  int version = queryVersion();
  std::cout << "\nOrderCancelRequest\n";
  FIX::Message cancel;

  switch ( version ) {
  case 42:
    cancel = queryOrderCancelRequest42();
    break;
  case 43:
    cancel = queryOrderCancelRequest43();
    break;
  case 44:
    cancel = queryOrderCancelRequest44();
    break;
  case 50:
    cancel = queryOrderCancelRequest50();
    break;
  default:
    std::cerr << "No test for version " << version << std::endl;
    break;
  }

  if ( queryConfirm( "Send cancel" ) )
    FIX::Session::sendToTarget( cancel );
}

void Application::queryReplaceOrder()
{
  int version = queryVersion();
  std::cout << "\nCancelReplaceRequest\n";
  FIX::Message replace;

  switch ( version ) {
  case 42:
    replace = queryCancelReplaceRequest42();
    break;
  case 43:
    replace = queryCancelReplaceRequest43();
    break;
  case 44:
    replace = queryCancelReplaceRequest44();
    break;
  case 50:
    replace = queryCancelReplaceRequest50();
    break;
  default:
    std::cerr << "No test for version " << version << std::endl;
    break;
  }

  if ( queryConfirm( "Send replace" ) )
    FIX::Session::sendToTarget( replace );
}


FIX42::NewOrderSingle 
Application::queryNewOrderSingle42()
{
  FIX::OrdType ordType;

  FIX42::NewOrderSingle newOrderSingle(
    queryClOrdID(), FIX::HandlInst(_handlInst), querySymbol(), querySide(),
    FIX::TransactTime(), ordType = queryOrdType() );

  newOrderSingle.set( queryOrderQty() );
  newOrderSingle.set( queryTimeInForce() );
  newOrderSingle.set( queryPrice() );

	if (_account != "") {
		newOrderSingle.set(FIX::Account(_account));
	}
	if (_useCurrency) {
		FIX::Symbol symbol;
		newOrderSingle.getField(symbol);
		std::string ssym = symbol.getValue();
		std::string curPrefix = ssym.substr(0,3);
#ifdef LOG
		//pan::log_DEBUG("************** CURRENCY PREFIX: ", curPrefix.c_str());
#endif
		newOrderSingle.set(FIX::Currency(curPrefix.c_str()));
	}


  queryHeader( newOrderSingle.getHeader() );
  return newOrderSingle;
}

FIX43::NewOrderSingle 
Application::queryNewOrderSingle43()
{
  FIX::OrdType ordType;

  FIX43::NewOrderSingle newOrderSingle(
    queryClOrdID(), FIX::HandlInst(_handlInst), querySide(),
    FIX::TransactTime(), ordType = queryOrdType() );

  newOrderSingle.set( querySymbol() );
  newOrderSingle.set( queryOrderQty() );
  newOrderSingle.set( queryTimeInForce() );
  newOrderSingle.set( queryPrice() );

	if (_account != "") {
		newOrderSingle.set(FIX::Account(_account));
	}
	if (_useCurrency) {
		FIX::Symbol symbol;
		newOrderSingle.getField(symbol);
		std::string ssym = symbol.getValue();
		std::string curPrefix = ssym.substr(0,3);
#ifdef LOG
		//pan::log_DEBUG("************** CURRENCY PREFIX: ", curPrefix.c_str());
#endif
		newOrderSingle.set(FIX::Currency(curPrefix.c_str()));
	}


  queryHeader( newOrderSingle.getHeader() );
  return newOrderSingle;
}

FIX44::NewOrderSingle 
Application::queryNewOrderSingle44()
{
  FIX::OrdType ordType;

  FIX44::NewOrderSingle newOrderSingle(
    queryClOrdID(), querySide(),
    FIX::TransactTime(), ordType = queryOrdType() );

  newOrderSingle.set( FIX::HandlInst(_handlInst) );
  newOrderSingle.set( querySymbol() );
  newOrderSingle.set( queryOrderQty() );
  newOrderSingle.set( queryTimeInForce() );
  newOrderSingle.set( queryPrice() );

	if (_account != "") {
		newOrderSingle.set(FIX::Account(_account));
	}
	if (_useCurrency) {
		FIX::Symbol symbol;
		newOrderSingle.getField(symbol);
		std::string ssym = symbol.getValue();
		std::string curPrefix = ssym.substr(0,3);
#ifdef LOG
		//pan::log_DEBUG("************** CURRENCY PREFIX: ", curPrefix.c_str());
#endif
		newOrderSingle.set(FIX::Currency(curPrefix.c_str()));
	}


  queryHeader( newOrderSingle.getHeader() );
  return newOrderSingle;
}

FIX50::NewOrderSingle 
Application::queryNewOrderSingle50()
{
  FIX::OrdType ordType;

  FIX50::NewOrderSingle newOrderSingle(
    queryClOrdID(), querySide(),
    FIX::TransactTime(), ordType = queryOrdType() );

  newOrderSingle.set( FIX::HandlInst(_handlInst) );
  newOrderSingle.set( querySymbol() );
  newOrderSingle.set( queryOrderQty() );
  newOrderSingle.set( queryTimeInForce() );
  newOrderSingle.set( queryPrice() );

	if (_account != "") {
		newOrderSingle.set(FIX::Account(_account));
	}
	if (_useCurrency) {
		FIX::Symbol symbol;
		newOrderSingle.getField(symbol);
		std::string ssym = symbol.getValue();
		std::string curPrefix = ssym.substr(0,3);
#ifdef LOG
		//pan::log_DEBUG("************** CURRENCY PREFIX: ", curPrefix.c_str());
#endif
		newOrderSingle.set(FIX::Currency(curPrefix.c_str()));
	}


  queryHeader( newOrderSingle.getHeader() );
  return newOrderSingle;
}


FIX42::OrderCancelRequest 
Application::queryOrderCancelRequest42()
{
  FIX42::OrderCancelRequest orderCancelRequest( queryOrigClOrdID(),
      queryClOrdID(), querySymbol(), querySide(), FIX::TransactTime() );

  orderCancelRequest.set( queryOrderQty() );
  queryHeader( orderCancelRequest.getHeader() );
  return orderCancelRequest;
}

FIX43::OrderCancelRequest 
Application::queryOrderCancelRequest43()
{
  FIX43::OrderCancelRequest orderCancelRequest( queryOrigClOrdID(),
      queryClOrdID(), querySide(), FIX::TransactTime() );

  orderCancelRequest.set( querySymbol() );
  orderCancelRequest.set( queryOrderQty() );
  queryHeader( orderCancelRequest.getHeader() );
  return orderCancelRequest;
}

FIX44::OrderCancelRequest 
Application::queryOrderCancelRequest44()
{
  FIX44::OrderCancelRequest orderCancelRequest( queryOrigClOrdID(),
      queryClOrdID(), querySide(), FIX::TransactTime() );

  orderCancelRequest.set( querySymbol() );
  orderCancelRequest.set( queryOrderQty() );
  queryHeader( orderCancelRequest.getHeader() );
  return orderCancelRequest;
}

FIX50::OrderCancelRequest 
Application::queryOrderCancelRequest50()
{
  FIX50::OrderCancelRequest orderCancelRequest( queryOrigClOrdID(),
      queryClOrdID(), querySide(), FIX::TransactTime() );

  orderCancelRequest.set( querySymbol() );
  orderCancelRequest.set( queryOrderQty() );
  queryHeader( orderCancelRequest.getHeader() );
  return orderCancelRequest;
}


FIX42::OrderCancelReplaceRequest 
Application::queryCancelReplaceRequest42()
{
  FIX42::OrderCancelReplaceRequest cancelReplaceRequest(
    queryOrigClOrdID(), queryClOrdID(), FIX::HandlInst(_handlInst),
    querySymbol(), querySide(), FIX::TransactTime(), queryOrdType() );

  if ( queryConfirm( "New price" ) )
    cancelReplaceRequest.set( queryPrice() );
  if ( queryConfirm( "New quantity" ) )
    cancelReplaceRequest.set( queryOrderQty() );

  queryHeader( cancelReplaceRequest.getHeader() );
  return cancelReplaceRequest;
}

FIX43::OrderCancelReplaceRequest 
Application::queryCancelReplaceRequest43()
{
  FIX43::OrderCancelReplaceRequest cancelReplaceRequest(
    queryOrigClOrdID(), queryClOrdID(), FIX::HandlInst(_handlInst),
    querySide(), FIX::TransactTime(), queryOrdType() );

  cancelReplaceRequest.set( querySymbol() );
  if ( queryConfirm( "New price" ) )
    cancelReplaceRequest.set( queryPrice() );
  if ( queryConfirm( "New quantity" ) )
    cancelReplaceRequest.set( queryOrderQty() );

  queryHeader( cancelReplaceRequest.getHeader() );
  return cancelReplaceRequest;
}

FIX44::OrderCancelReplaceRequest 
Application::queryCancelReplaceRequest44()
{
  FIX44::OrderCancelReplaceRequest cancelReplaceRequest(
    queryOrigClOrdID(), queryClOrdID(),
    querySide(), FIX::TransactTime(), queryOrdType() );

  cancelReplaceRequest.set( FIX::HandlInst(_handlInst));
  cancelReplaceRequest.set( querySymbol() );
  if ( queryConfirm( "New price" ) )
    cancelReplaceRequest.set( queryPrice() );
  if ( queryConfirm( "New quantity" ) )
    cancelReplaceRequest.set( queryOrderQty() );

  queryHeader( cancelReplaceRequest.getHeader() );
  return cancelReplaceRequest;
}

FIX50::OrderCancelReplaceRequest 
Application::queryCancelReplaceRequest50()
{
  FIX50::OrderCancelReplaceRequest cancelReplaceRequest(
    queryOrigClOrdID(), queryClOrdID(),
    querySide(), FIX::TransactTime(), queryOrdType() );

  cancelReplaceRequest.set( FIX::HandlInst(_handlInst));
  cancelReplaceRequest.set( querySymbol() );
  if ( queryConfirm( "New price" ) )
    cancelReplaceRequest.set( queryPrice() );
  if ( queryConfirm( "New quantity" ) )
    cancelReplaceRequest.set( queryOrderQty() );

  queryHeader( cancelReplaceRequest.getHeader() );
  return cancelReplaceRequest;
}


void 
Application::queryHeader( FIX::Header& header )
{
	header.setField( querySenderCompID() );
	header.setField( queryTargetCompID() );
//	header.setField(FIX::SenderCompID(_sessionID.getSenderCompID()));
//	header.setField(FIX::TargetCompID(_sessionID.getTargetCompID()));
	#if 0
	pan::log_DEBUG("SenderCompID: ", _sessionID.getSenderCompID());
	pan::log_DEBUG("TargetCompID: ", _sessionID.getTargetCompID());
	#endif

//  if ( queryConfirm( "Use a TargetSubID" ) )
 //   header.setField( queryTargetSubID() );
}

char 
Application::queryAction()
{
  char value;
  std::cout << std::endl
  << "1) Enter Order" << std::endl
  << "2) Cancel Order" << std::endl
  << "3) Replace Order" << std::endl
  << "4) Market data test" << std::endl
  << "5) Quit" << std::endl
  << "Action: ";
  std::cin >> value;
  switch ( value )
  {
    case '1': case '2': case '3': case '4': case '5': break;
    default: throw std::exception();
  }
  return value;
}

int 
Application::queryVersion()
{
  char value;
  std::cout << std::endl
  << "3) FIX.4.2" << std::endl
  << "4) FIX.4.3" << std::endl
  << "5) FIX.4.4" << std::endl
  << "6) FIXT.1.1 (FIX.5.0)" << std::endl
  << "BeginString: ";
  std::cin >> value;
  switch ( value )
  {
    case '3': return 42;
    case '4': return 43;
    case '5': return 44;
    case '6': return 50;
    default: throw std::exception();
  }
}

bool 
Application::queryConfirm( const std::string& query )
{
  std::string value;
  std::cout << std::endl << query << "?: ";
  std::cin >> value;
  return toupper( *value.c_str() ) == 'Y';
}

FIX::SenderCompID 
Application::querySenderCompID()
{
  std::string value;
  std::cout << std::endl << "SenderCompID: ";
  std::cin >> value;
  return FIX::SenderCompID( value );
}

FIX::TargetCompID 
Application::queryTargetCompID()
{
  std::string value;
  std::cout << std::endl << "TargetCompID: ";
  std::cin >> value;
  return FIX::TargetCompID( value );
}

FIX::TargetSubID 
Application::queryTargetSubID()
{
  std::string value;
  std::cout << std::endl << "TargetSubID: ";
  std::cin >> value;
  return FIX::TargetSubID( value );
}

FIX::ClOrdID 
Application::queryClOrdID()
{
  std::string value;
  std::cout << std::endl << "ClOrdID: ";
  std::cin >> value;
  return FIX::ClOrdID( value );
}

FIX::OrigClOrdID 
Application::queryOrigClOrdID()
{
  std::string value;
  std::cout << std::endl << "OrigClOrdID: ";
  std::cin >> value;
  return FIX::OrigClOrdID( value );
}

FIX::Symbol 
Application::querySymbol()
{
  std::string value;
  std::cout << std::endl << "Symbol: ";
  std::cin >> value;
  return FIX::Symbol( value );
}

FIX::Side Application::querySide()
{
  char value;
  std::cout << std::endl
  << "1) Buy" << std::endl
  << "2) Sell" << std::endl
  << "3) Sell Short" << std::endl
  << "4) Sell Short Exempt" << std::endl
  << "5) Cross" << std::endl
  << "6) Cross Short" << std::endl
  << "7) Cross Short Exempt" << std::endl
  << "Side: ";

  std::cin >> value;
  switch ( value )
  {
    case '1': return FIX::Side( FIX::Side_BUY );
    case '2': return FIX::Side( FIX::Side_SELL );
    case '3': return FIX::Side( FIX::Side_SELL_SHORT );
    case '4': return FIX::Side( FIX::Side_SELL_SHORT_EXEMPT );
    case '5': return FIX::Side( FIX::Side_CROSS );
    case '6': return FIX::Side( FIX::Side_CROSS_SHORT );
    case '7': return FIX::Side( 'A' );
    default: throw std::exception();
  }
}

FIX::OrderQty Application::queryOrderQty()
{
  long value;
  std::cout << std::endl << "OrderQty: ";
  std::cin >> value;
  return FIX::OrderQty( value );
}

FIX::OrdType Application::queryOrdType()
{
  char value;
  std::cout << std::endl
  << "1) Market" << std::endl
  << "2) Limit" << std::endl
  << "3) Stop" << std::endl
  << "4) Stop Limit" << std::endl
  << "OrdType: ";

  std::cin >> value;
  switch ( value )
  {
    case '1': return FIX::OrdType( FIX::OrdType_MARKET );
    //case '2': return FIX::OrdType( FIX::OrdType_LIMIT );
    case '2': return FIX::OrdType(_limitOrderChar);
    case '3': return FIX::OrdType( FIX::OrdType_STOP );
    case '4': return FIX::OrdType( FIX::OrdType_STOP_LIMIT );
    default: throw std::exception();
  }
}

FIX::Price Application::queryPrice()
{
  double value;
  std::cout << std::endl << "Price: ";
  std::cin >> value;
  return FIX::Price( value );
}

FIX::StopPx Application::queryStopPx()
{
  double value;
  std::cout << std::endl << "StopPx: ";
  std::cin >> value;
  return FIX::StopPx( value );
}

FIX::TimeInForce Application::queryTimeInForce()
{
  char value;
  std::cout << std::endl
  << "1) Day" << std::endl
  << "2) IOC" << std::endl
/*
  << "3) OPG" << std::endl
  << "4) GTC" << std::endl
  << "5) GTX" << std::endl
*/
  << "TimeInForce: ";

  std::cin >> value;
  switch ( value )
  {
    case '1': return FIX::TimeInForce( FIX::TimeInForce_DAY );
    case '2': return FIX::TimeInForce( FIX::TimeInForce_IMMEDIATE_OR_CANCEL );
/*
    case '3': return FIX::TimeInForce( FIX::TimeInForce_AT_THE_OPENING );
    case '4': return FIX::TimeInForce( FIX::TimeInForce_GOOD_TILL_CANCEL );
    case '5': return FIX::TimeInForce( FIX::TimeInForce_GOOD_TILL_CROSSING );
*/
    default: throw std::exception();
  }
}

void*
Application::run()
{
	#ifdef LOG
	pan::log_DEBUG("Application::test()");
	#endif
// SHIT STARTS HERE - move to class or struct
// KTK - Not sure I like this zthread library czmq and my gut is to change 
// this to just use standard paradigms and the straight C++ API.
	//zctx_t *ctx = zctx_new();
// SHIT ENDS HERE - move to class

}

void 
Application::test()
{

	#ifdef LOG
	pan::log_DEBUG("Application::test()");
	#endif
    
    while (1) {
		try {
			char action = queryAction();
			if ( action == '1' )
				queryEnterOrder();
			else if ( action == '2' )
				queryCancelOrder();
			else if ( action == '3' )
				queryReplaceOrder();
			else if ( action == '5' )
				break;
			}
		catch ( std::exception & e ) {
			std::cout << "Message Not Sent: " << e.what();
		}
	}

	//timespec evtTime;
	//clock_gettime(CLOCK_MONOTONIC, &evtTime);
}

Application::~Application() 
{
}
