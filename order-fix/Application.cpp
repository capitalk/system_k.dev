/* -*- C++ -*- */

/******************************************************************************
  Copyright (C) 2011,2012 Capital K Partners BV

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
#include "utils/JenkinsHash.h"

#include <iostream>


namespace fs = boost::filesystem; 
namespace dt = boost::gregorian; 

Application::Application(bool bReset, const ApplicationConfig& config) 
         :  OrderInterface(config.venueID), _loggedIn(false), _loggedOut(false), _loginCount(0), 
            _appMsgCount(0),  _config(config), _resetSequence(bReset)
{
	_handlInst21 = -1;
	_account1 = "";
#ifdef LOG
	pan::log_INFORMATIONAL("Application::Application()");
#endif
}

Application::~Application()
{
	pan::log_DEBUG("Application::~Application()");
	assert(_pout_sock);
	_pout_sock->close();
	
}

/*****************************************************************************
 * VIRTUAL IMPLEMENTATIONS 
 ****************************************************************************/
void Application::onLogon(const FIX::SessionID& sessionID )
{
	_loggedIn = true;
	_loggedOut = false;
	//_sessionID = sessionID;
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
#ifdef LOG
		pan::log_DEBUG("RECEIVED MSG fromAdmin(", message.toString() ,")");
#endif 
	FIX::BeginString beginString;
	message.getHeader().getField(beginString);
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
#ifdef LOG
		pan::log_DEBUG("SENDING MSG toAdmin(", message.toString(), ")");
#endif
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
	if (msgType.getValue() == "5") {
#ifdef LOG
		pan::log_DEBUG("Sending Logout(", message.toString(), ")");
#endif 
	}
	// logon message 
	if (msgType.getValue() == "A") {
#ifdef LOG
		pan::log_DEBUG("Sending Login(", message.toString(), ")");
#endif 
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
	pan::log_DEBUG("ExecutionReport42 - ",  message.toString());		
#endif
	capkproto::execution_report* er = new capkproto::execution_report();
	FIX::ClOrdID clOrdID;
	FIX::OrigClOrdID origClOrdID;
	FIX::ExecID execID;
	FIX::ExecTransType execTransType;
	FIX::OrdStatus orderStatus;
	FIX::ExecType execType;
	FIX::Symbol symbol;
	FIX::SecurityType securityType;
	FIX::Side side;
	FIX::OrderQty orderQty;
	FIX::OrdType ordType;
	FIX::Price price;
	FIX::LastShares lastShares;
	FIX::LastPx lastPx;
	FIX::LeavesQty leavesQty;
	FIX::CumQty cumQty;
	FIX::AvgPx avgPx;
	FIX::TimeInForce timeInForce;
	FIX::ExecInst execInst;
	FIX::TransactTime transactTime;
	FIX::HandlInst handlInst;
	FIX::OrdRejReason ordRejReason;
	FIX::MinQty minQty;

	order_id_t cloid;
	order_id_t origcloid;
	int ret;

	if (message.isSetField(clOrdID)) {
		message.getField(clOrdID);
		pan::log_DEBUG("FIX::ClOrdID: ", clOrdID.getString().c_str());
		ret = cloid.set(clOrdID.getString().c_str());
		assert(ret == 0);
		er->set_cl_order_id(cloid.get_uuid(), cloid.size());
		pan::log_DEBUG("Received OID: ", pan::blob(cloid.get_uuid(), cloid.size())); 
//pan::log_DEBUG("A");
	}
	if (message.isSetField(origClOrdID)) { 
		message.getField(origClOrdID);
		origcloid.set(origClOrdID.getString().c_str());
		er->set_orig_cl_order_id(origcloid.get_uuid(), origcloid.size());
//pan::log_DEBUG("B");
	}
	if (message.isSetField(execID)) {
		message.getField(execID);	
		er->set_exec_id(execID.getString());
//pan::log_DEBUG("C");
	}
	if (message.isSetField(execTransType)) {
		message.getField(execTransType);
		er->set_exec_trans_type(execTransType);
//pan::log_DEBUG("D");
	}
	if (message.isSetField(orderStatus)) {
		message.getField(orderStatus);
		er->set_order_status(orderStatus);
//pan::log_DEBUG("E");
	}
	if (message.isSetField(execType)) {
		message.getField(execType);
		er->set_exec_type(execType);
//pan::log_DEBUG("F");
	}
	if (message.isSetField(symbol)) {
		message.getField(symbol);
		er->set_symbol(symbol);
//pan::log_DEBUG("G");
	}
	if (message.isSetField(securityType)) {
		message.getField(securityType);
		er->set_security_type(securityType);
//pan::log_DEBUG("H");
	}
	if (message.isSetField(side)) {
		message.getField(side);
		if (side == FIX::Side_BUY) {	
			er->set_side(capkproto::BID);
		}
		if (side == FIX::Side_SELL) {	
			er->set_side(capkproto::ASK);
		}
//pan::log_DEBUG("I");
	}
	if (message.isSetField(orderQty)) {
		message.getField(orderQty);
		er->set_order_qty(orderQty);
//pan::log_DEBUG("J");
	}
	if (message.isSetField(ordType)) {
		message.getField(ordType);
		er->set_ord_type(ordType);
//pan::log_DEBUG("K");
	}
	if (message.isSetField(price)) {
		message.getField(price);
		er->set_price(price);
//pan::log_DEBUG("L");
	}
	if (message.isSetField(lastShares)) {
		message.getField(lastShares);
		er->set_last_shares(lastShares);
//pan::log_DEBUG("M");
	}
	if (message.isSetField(lastPx)) {
		message.getField(lastPx);
		er->set_last_price(lastPx);
//pan::log_DEBUG("N");
	}
	if (message.isSetField(leavesQty)) {
		message.getField(leavesQty);
		er->set_leaves_qty(leavesQty);
//pan::log_DEBUG("O");
	}
	if (message.isSetField(cumQty)) {
		message.getField(cumQty);
		er->set_cum_qty(cumQty);
//pan::log_DEBUG("P");
	}
	if (message.isSetField(avgPx)) {
		message.getField(avgPx);
		er->set_avg_price(avgPx);
//pan::log_DEBUG("Q");
	}
	if (message.isSetField(timeInForce)) {
		message.getField(timeInForce);
		if (timeInForce == FIX::TimeInForce_DAY) {
			er->set_time_in_force(capkproto::GFD);
		}
		if (timeInForce == FIX::TimeInForce_FILL_OR_KILL) {
			er->set_time_in_force(capkproto::FOK);
		}
		if (timeInForce == FIX::TimeInForce_IMMEDIATE_OR_CANCEL) {
			er->set_time_in_force(capkproto::IOC);
		}
//pan::log_DEBUG("R");
	}
	if (message.isSetField(transactTime)) {
		message.getField(transactTime);
		er->set_transact_time(FIX::UtcTimeStampConvertor::convert(transactTime, true));
//pan::log_DEBUG("S");
	}
	if (message.isSetField(execInst)) {
		message.getField(execInst);
		er->set_exec_inst(execInst);
//pan::log_DEBUG("T");
	}
	if (message.isSetField(handlInst)) {
		message.getField(handlInst);
		er->set_handl_inst(handlInst);
//pan::log_DEBUG("U");
	}
	if (message.isSetField(ordRejReason)) {
		message.getField(ordRejReason);
		er->set_order_reject_reason(ordRejReason);
//pan::log_DEBUG("V");
	}
	if (message.isSetField(minQty)) {
		message.getField(minQty);
		er->set_min_qty(minQty);
//pan::log_DEBUG("W");
	}
	
	// 1) lookup order id to fetch strategy id
	// 2) send the strategy id back to msg processor so that we can route 
	// properly back to strategy
	// 3) send the data in the message
	// 4) end the transmission
	//_pout_sock->send();
	
	bool sndOK;
	pan::log_DEBUG("Fetching sid for order id: ", pan::blob(cloid.get_uuid(), cloid.size())); 
	assert(_pMsgProcessor);
	KOrderCache* ocache = _pMsgProcessor->getOrderCache();
	assert(ocache);
	OrderInfo_ptr oinfo = ocache->get(cloid);
	if (!oinfo) {
		pan::log_CRITICAL("No order info for orderid - can't lookup sid in order cache");	
	}
	strategy_id_t strategy_id = oinfo->getStrategyID();
	pan::log_DEBUG("Found sid: ", pan::blob(strategy_id.get_uuid(), strategy_id.size()));

	// create strategy id frame and send it with more flag set
	zmq::message_t sidframe(strategy_id.size());	
	memcpy(sidframe.data(), strategy_id.get_uuid(), strategy_id.size());
	sndOK = _pout_sock->send(sidframe, ZMQ_SNDMORE);
	assert(sndOK);

	// send the message type
	zmq::message_t msgtypeframe(sizeof(capk::EXEC_RPT));
	memcpy(msgtypeframe.data(), &capk::EXEC_RPT, sizeof(capk::EXEC_RPT));
	sndOK = _pout_sock->send(msgtypeframe, ZMQ_SNDMORE);
	assert(sndOK);

	// send the rest of the message using zero-copy since exection report
	// was created on heap
	zmq::message_t dataframe(er->ByteSize());
	er->SerializeToArray(dataframe.data(), dataframe.size());
	sndOK = _pout_sock->send(dataframe, 0);
	assert(sndOK);
	delete er;	
	
}

void 
Application::onMessage(const FIX42::OrderCancelReject& message, 
						const FIX::SessionID& sessionID) 
{
#ifdef LOG
	pan::log_DEBUG("OrderCancelReject42 - ",  message.toString());		
#endif
	capkproto::order_cancel_reject* ocr = new capkproto::order_cancel_reject();
	FIX::OrigClOrdID origClOrdID;
	FIX::ClOrdID clOrdID;
	FIX::OrdStatus ordStatus;
	FIX::CxlRejReason cxlRejReason;
	FIX::CxlRejResponseTo cxlRejResponseTo;
	FIX::Text cxlRejText;

	int ret;
	order_id_t origcloid;
	order_id_t cloid;

	if (message.isSetField(origClOrdID)) {
		message.getField(origClOrdID);
		pan::log_DEBUG("FIX::OrigClOrdID: ", origClOrdID.getString().c_str());
		ret = origcloid.set(origClOrdID.getString().c_str());
		assert(ret == 0);
		ocr->set_orig_cl_order_id(origcloid.get_uuid(), origcloid.size());
		pan::log_DEBUG("Received OrigClOrdID: ", pan::blob(origcloid.get_uuid(), origcloid.size())); 
	}
	if (message.isSetField(clOrdID)) {
		message.getField(clOrdID);
		pan::log_DEBUG("FIX::ClOrdID: ", clOrdID.getString().c_str());
		ret = cloid.set(clOrdID.getString().c_str());
		assert(ret == 0);
		ocr->set_cl_order_id(cloid.get_uuid(), cloid.size());
		pan::log_DEBUG("Received ClOid: ", pan::blob(cloid.get_uuid(), cloid.size()));
	}
	if (message.isSetField(ordStatus)) {
		message.getField(ordStatus);
		ocr->set_order_status(ordStatus);
	}
	if (message.isSetField(cxlRejReason)) {
		message.getField(cxlRejReason);
		ocr->set_cancel_reject_reason(cxlRejReason);
	}
	if (message.isSetField(cxlRejResponseTo)) {
		message.getField(cxlRejResponseTo);
		ocr->set_cancel_reject_response_to(cxlRejResponseTo);
	}
	if (message.isSetField(cxlRejText)) {
		message.getField(cxlRejText);
		ocr->set_cancel_reject_text(cxlRejText);
	}

	// 1) lookup order id to fetch strategy id
	// 2) send the strategy id back to msg processor so that we can route 
	// properly back to strategy
	// 3) send the data in the message
	// 4) end the transmission
		
	bool sndOK;
	pan::log_DEBUG("Fetching sid for order id: ", pan::blob(cloid.get_uuid(), cloid.size())); 
	assert(_pMsgProcessor);
	KOrderCache* ocache = _pMsgProcessor->getOrderCache();
	assert(ocache);
	OrderInfo_ptr oinfo = ocache->get(cloid);
	if (!oinfo) {
		pan::log_CRITICAL("No order info for orderid - can't lookup sid in order cache");	
	}
	strategy_id_t strategy_id = oinfo->getStrategyID();
	pan::log_DEBUG("Found sid: ", pan::blob(strategy_id.get_uuid(), strategy_id.size()));

	// create strategy id frame and send it with more flag set
	zmq::message_t sidframe(strategy_id.size());	
	memcpy(sidframe.data(), strategy_id.get_uuid(), strategy_id.size());
	sndOK = _pout_sock->send(sidframe, ZMQ_SNDMORE);
	assert(sndOK);

	// send the message type
	zmq::message_t msgtypeframe(sizeof(capk::ORDER_CANCEL_REJ));
	memcpy(msgtypeframe.data(), &capk::ORDER_CANCEL_REJ, sizeof(capk::ORDER_CANCEL_REJ));
	sndOK = _pout_sock->send(msgtypeframe, ZMQ_SNDMORE);
	assert(sndOK);

	// send the rest of the message 
	zmq::message_t dataframe(ocr->ByteSize());
	ocr->SerializeToArray(dataframe.data(), dataframe.size());
	sndOK = _pout_sock->send(dataframe, 0);
	assert(sndOK);
	delete ocr;	
	
}

void 
Application::onMessage(const FIX42::ListStatus& message, 
						const FIX::SessionID& sessionID)
{
#ifdef LOG
	pan::log_DEBUG(message.toString());
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
    queryClOrdID(), FIX::HandlInst(_handlInst21), querySymbol(), querySide(),
    FIX::TransactTime(), ordType = queryOrdType() );

  newOrderSingle.set( queryOrderQty() );
  newOrderSingle.set( queryTimeInForce() );
  newOrderSingle.set( queryPrice() );

	if (_account1 != "") {
		newOrderSingle.set(FIX::Account(_account1));
	}
	if (_useCurrency15) {
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
    queryClOrdID(), FIX::HandlInst(_handlInst21), querySide(),
    FIX::TransactTime(), ordType = queryOrdType() );

  newOrderSingle.set( querySymbol() );
  newOrderSingle.set( queryOrderQty() );
  newOrderSingle.set( queryTimeInForce() );
  newOrderSingle.set( queryPrice() );

	if (_account1 != "") {
		newOrderSingle.set(FIX::Account(_account1));
	}
	if (_useCurrency15) {
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

  newOrderSingle.set( FIX::HandlInst(_handlInst21) );
  newOrderSingle.set( querySymbol() );
  newOrderSingle.set( queryOrderQty() );
  newOrderSingle.set( queryTimeInForce() );
  newOrderSingle.set( queryPrice() );

	if (_account1 != "") {
		newOrderSingle.set(FIX::Account(_account1));
	}
	if (_useCurrency15) {
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

  newOrderSingle.set( FIX::HandlInst(_handlInst21) );
  newOrderSingle.set( querySymbol() );
  newOrderSingle.set( queryOrderQty() );
  newOrderSingle.set( queryTimeInForce() );
  newOrderSingle.set( queryPrice() );

	if (_account1 != "") {
		newOrderSingle.set(FIX::Account(_account1));
	}
	if (_useCurrency15) {
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
    queryOrigClOrdID(), queryClOrdID(), FIX::HandlInst(_handlInst21),
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
    queryOrigClOrdID(), queryClOrdID(), FIX::HandlInst(_handlInst21),
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

  cancelReplaceRequest.set( FIX::HandlInst(_handlInst21));
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

  cancelReplaceRequest.set( FIX::HandlInst(_handlInst21));
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
	header.setField(FIX::SenderCompID(_config.senderCompID));
	header.setField(FIX::TargetCompID(_config.targetCompID));

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
    case '2': return FIX::OrdType(_limitOrderChar40);
    case '3': return FIX::OrdType(FIX::OrdType_STOP );
    case '4': return FIX::OrdType(FIX::OrdType_STOP_LIMIT );
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

void
Application::run()
{
#ifdef LOG
	pan::log_DEBUG("Application::run()");
#endif
	zmq::context_t* ctx = getZMQContext();
	_pMsgProcessor = new KMsgProcessor(ctx,
                "tcp://127.0.0.1:9999",
                "inproc://msgout",
                1,
                this);

	_pMsgProcessor->setOrderInterface(this);
	// create sockets and bind 
	// KTK TODO - create sockets in other thread - should not use sockets 
	// created in thread A from thread B
	_pMsgProcessor->init();
	_pout_sock = new zmq::socket_t(*ctx, ZMQ_DEALER);
	int zero = 0;
	_pout_sock->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
	std::string outaddr = _pMsgProcessor->getOutboundAddr();
	_pout_sock->connect(outaddr.c_str());
	
	_pMsgProcessor->run();

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

// Order interface methods
void
Application::dispatch(capk::msg_t msgType, 
					char* data, 
					size_t len)
{
	pan::log_DEBUG("Application::dispatch() - msgType: ", pan::integer(msgType), " data: ", pan::blob(data, len));
	bool parseOK;
	switch(msgType) {
		case capk::ORDER_NEW:
		{
			capkproto::new_order_single nos;
			parseOK = nos.ParseFromArray(data, len);
			if (!parseOK) {
				pan::log_CRITICAL(__FILE__, " ", 
					pan::integer(__LINE__), " ", 
					"NewOrderSingle - Parsing protobuf failed!");
					// KTK TODO broadcast error
			}
			else {	
				if (_config.version == 42) {
					newOrderSingle42(nos);
				}
				else {
					pan::log_CRITICAL("NO SUPPORTED FIX VERSION FOUND FOR NEW ORDER REQUEST");
				}
			}
			break;
		}

		case capk::ORDER_CANCEL:
		{
			capkproto::order_cancel oc;
			parseOK = oc.ParseFromArray(data, len);
			if (!parseOK) {
				pan::log_CRITICAL(__FILE__, " ", 
					pan::integer(__LINE__), " ", 
					"OrderCancel - Parsing protobuf failed!");
					// KTK TODO broadcast error
			}
			else {
				if (_config.version == 42) {
					orderCancel42(oc);
				}
				else {
					pan::log_CRITICAL("NO SUPPORTED FIX VERSION FOUND FOR CANCEL REQUEST");
				}
				break;
			}
		}

		case capk::ORDER_REPLACE: 
		{
			capkproto::order_cancel_replace ocr;
			parseOK = ocr.ParseFromArray(data, len);
			if (!parseOK) {
				pan::log_CRITICAL(__FILE__, " ", 
					pan::integer(__LINE__), " ", 
					"OrderCancelReplace - Parsing protobuf failed!");
					// KTK TODO broadcast error
			}
			else {	
				if (_config.version == 42) {
					orderCancelReplace42(ocr);
				}
				else {
					pan::log_CRITICAL("NO SUPPORTED FIX VERSION FOUND FOR CANCEL REPLACE REQUEST");
				}
			}
		}
		
		case capk::ORDER_STATUS: 
		{
			capkproto::order_status os;
			parseOK = os.ParseFromArray(data, len);
			if (!parseOK) {
				pan::log_CRITICAL(__FILE__, " ", 
					pan::integer(__LINE__), " ", 
					"OrderStatus - Parsing protobuf failed!");
					// KTK TODO broadcast error
			}
			else {
				if (_config.version == 42) {
					orderStatus42(os);	
				}
				else {
					pan::log_CRITICAL("NO SUPPORTED FIX VERSION FOUND FOR CANCEL REPLACE REQUEST");
				}
			}
		}
	}
}

void 
Application::snd(capk::msg_t msgType, 
					char* data, 
					size_t len)
{
	pan::log_DEBUG("Application::snd() - msgType: ", pan::integer(msgType), " data: ", pan::blob(data, len));
}

void Application::newOrderSingle42(capkproto::new_order_single& nos)
{

	pan::log_DEBUG("Application::newOrderSingle42()");
	pan::log_DEBUG("received protobuf: ", nos.DebugString());


	// pull the data out of protobuf into FIX elements
	char cloidbuf[UUID_LEN+1];
	order_id_t cloid;
	cloid.set(nos.order_id().c_str(), nos.order_id().size());
	cloid.c_str(cloidbuf);
	FIX::ClOrdID clOrdID(cloidbuf);
	pan::log_DEBUG("CLORDID: ", cloidbuf);

	// symbol
	FIX::Symbol symbol(nos.symbol().c_str());
	pan::log_DEBUG("SYMBOL: ", nos.symbol());

	// side
	FIX::Side side;
	pan::log_DEBUG("SIDE: ", pan::integer(nos.side()));
	if (nos.side() == capkproto::BID) {
		pan::log_DEBUG("SETTING BID");
		side = FIX::Side_BUY;
	}
	else if (nos.side() == capkproto::ASK) {
		pan::log_DEBUG("SETTING ASK");
		side = FIX::Side_SELL;
	}

	// order type
	FIX::OrdType ordType;

	pan::log_DEBUG("ORDER TYPE: ", pan::integer(nos.ord_type()));
	if (nos.ord_type() == capkproto::LIM) {
		pan::log_DEBUG("SETTING ORDER TYPE LIMIT");
		ordType = _limitOrderChar40;
	}
	else {
		ordType = nos.ord_type();
	}

	FIX42::NewOrderSingle nos42(clOrdID, FIX::HandlInst(_handlInst21), symbol, side,
		FIX::TransactTime(), ordType);


	// order quantity
	pan::log_DEBUG("ORDER QTY: ", pan::real(nos.order_qty()));
	FIX::OrderQty orderQty(nos.order_qty());
	nos42.set(orderQty);

	// time in force
	//nos42.set( queryTimeInForce() );
	FIX::TimeInForce timeInForce;
	pan::log_DEBUG("TIME IN FORCE: ", pan::integer(nos.time_in_force()));
	if (nos.time_in_force() == capkproto::GFD) {
		pan::log_DEBUG("TIF = DAY");
		timeInForce = FIX::TimeInForce_DAY;
	}
	else if (nos.time_in_force() == capkproto::IOC) {
		pan::log_DEBUG("TIF = IOC");
		timeInForce = FIX::TimeInForce_IMMEDIATE_OR_CANCEL;
	}
	nos42.set(timeInForce);

	// price
	pan::log_DEBUG("PRICE: ", pan::real(nos.price()));
	FIX::Price price(nos.price());
	nos42.set(price);

	pan::log_DEBUG("----------------->", nos42.toString());

	if (_account1 != "") {
		nos42.set(FIX::Account(_account1));
	}
	if (_useCurrency15) {
		FIX::Symbol symbol;
		nos42.getField(symbol);
		std::string ssym = symbol.getValue();
		std::string curPrefix = ssym.substr(0,3);
#ifdef LOG
		//pan::log_DEBUG("************** CURRENCY PREFIX: ", curPrefix.c_str());
#endif
		nos42.set(FIX::Currency(curPrefix.c_str()));
	}

/*
	nos42.set(FIX::TransactTime());

	// pull the data out of protobuf into FIX elements
	char cloidbuf[UUID_LEN+1];
	order_id_t cloid;
	cloid.set(nos.order_id().c_str(), nos.order_id().size());
	cloid.c_str(cloidbuf);
	FIX::ClOrdID clOrdID(cloidbuf);
	nos42.set(clOrdID);

	// symbol
	FIX::Symbol symbol(nos.symbol().c_str());
	nos42.set(symbol);

	// handling instructions
	FIX::HandlInst handlInst(_handlInst21);
	nos42.set(handlInst);

	// side
	FIX::Side side;
	pan::log_DEBUG("SIDE: ", pan::integer(nos.side()));
	if (nos.side() == capkproto::BID) {
		side.setField(FIX::Side_BUY);
	}
	else if (nos.side() == capkproto::ASK) {
		side.setField(FIX::Side_SELL);
	}
	else {
		pan::log_CRITICAL("No side specified for new OID: ", cloidbuf);
	}
	nos42.set(side);

	// order quantity
	FIX::OrderQty orderQty(nos.order_qty());
	nos42.set(orderQty);

	// time in force
	FIX::TimeInForce timeInForce;
	pan::log_DEBUG("TIME IN FORCE: ", pan::integer(nos.time_in_force()));
	timeInForce.setField(nos.time_in_force());
	nos42.setField(timeInForce);

	// order type
	FIX::OrdType ordType;
	pan::log_DEBUG("ORDER TYPE: ", pan::integer(nos.order_type()));
	if (nos.order_type() == 2) {
		ordType.setField(_limitOrderChar40);
	}
	else {
		ordType.setField(nos.order_type());
	}
	nos42.set(timeInForce);

	FIX::Price price(nos.price());
	nos42.set(price);

	if (_account1 != "") {
		nos42.set(FIX::Account(_account1));
	}
	if (_useCurrency15) {
		FIX::Symbol symbol;
		nos42.getField(symbol);
		std::string ssym = symbol.getValue();
		std::string curPrefix = ssym.substr(0,3);
#ifdef LOG
		pan::log_DEBUG("NewOrderSingle42 setting currency prefix: ", curPrefix.c_str());
#endif
		nos42.set(FIX::Currency(curPrefix.c_str()));
	}

	nos42.getHeader().setField(FIX::SenderCompID(_sessionID.getSenderCompID()));
	nos42.getHeader().setField(FIX::TargetCompID(_sessionID.getTargetCompID()));

	#ifdef LOG
	pan::log_DEBUG("Sending FIX message: ", nos42.toString());
#endif
*/

	//FIX::Session::sendToTarget(nos42, FIX::SenderCompID("GSF"), FIX::TargetCompID("BAXTERtest"));
	nos42.getHeader().setField(FIX::SenderCompID(_config.senderCompID));
	nos42.getHeader().setField(FIX::TargetCompID(_config.targetCompID));
	pan::log_DEBUG("SendToTarget(nos42)");
	bool sendOK = FIX::Session::sendToTarget(nos42);
    if (sendOK == false) {
        pan::log_CRITICAL("SendToTarget(nos42) FAILED for OID: ", pan::blob(cloid.get_uuid(), UUID_LEN), 
                " full order information follows: \n", 
                nos42.toString().c_str());
    }


}

void Application::newOrderSingle43(capkproto::new_order_single& nos)
{
	pan::log_DEBUG("Application::newOrderSingle43()");
}

void Application::newOrderSingle44(capkproto::new_order_single& nos)
{
	pan::log_DEBUG("Application::newOrderSingle44()");
}

void Application::newOrderSingle50(capkproto::new_order_single& nos)
{
	pan::log_DEBUG("Application::newOrderSingle50()");
}

void Application::orderCancelReplace42(capkproto::order_cancel_replace& ocr)
{
	pan::log_DEBUG("Application::orderCancelReplace42()");

	// OrigOrdID
	char origoidbuf[UUID_LEN+1];
	order_id_t origoid;
	origoid.set(ocr.orig_order_id().c_str(), ocr.orig_order_id().size());
	origoid.c_str(origoidbuf);
	FIX::OrigClOrdID origClOrdID(origoidbuf);
	// ClOrdID
	char clordidbuf[UUID_LEN+1];
	order_id_t clordid;
	origoid.set(ocr.cl_order_id().c_str(), ocr.cl_order_id().size());
	origoid.c_str(clordidbuf);
	FIX::ClOrdID clOrdID(clordidbuf);
	
	// symbol
	FIX::Symbol symbol(ocr.symbol().c_str());

	// side
	FIX::Side side;
	pan::log_DEBUG("SIDE: ", pan::integer(ocr.side()));
	if (ocr.side() == capkproto::BID) {
		pan::log_DEBUG("SETTING BID");
		side = FIX::Side_BUY;
	}
	else if (ocr.side() == capkproto::ASK) {
		pan::log_DEBUG("SETTING ASK");
		side = FIX::Side_SELL;
	}
	
	// order type
	FIX::OrdType ordType;
	if (ocr.ord_type() == capkproto::LIM) {
		pan::log_DEBUG("SETTING ORDER TYPE LIMIT");
		ordType = _limitOrderChar40;
	}
	else {
		pan::log_DEBUG("SETTING ORDER TYPE: ", pan::integer(ocr.ord_type()));
		ordType = ocr.ord_type();
	}
	// order quantity
	FIX::OrderQty orderQty(ocr.order_qty());
	pan::log_DEBUG("SETTING QTY: ", pan::real(ocr.order_qty()));

	// order price
	FIX::Price price(ocr.price());
	pan::log_DEBUG("SETTING PRICE: ", pan::real(ocr.price()));
	
	// time in force
	FIX::TimeInForce timeInForce;
	pan::log_DEBUG("TIME IN FORCE: ", pan::integer(ocr.time_in_force()));
	if (ocr.time_in_force() == capkproto::GFD) {
		pan::log_DEBUG("TIF = DAY");
		timeInForce = FIX::TimeInForce_DAY;
	}
	else if (ocr.time_in_force() == capkproto::IOC) {
		pan::log_DEBUG("TIF = IOC");
		timeInForce = FIX::TimeInForce_IMMEDIATE_OR_CANCEL;
	}

	FIX42::OrderCancelReplaceRequest ocr42(origClOrdID,
								clOrdID,	
								FIX::HandlInst(_handlInst21),
								symbol,
								side, 
								FIX::TransactTime(),
								ordType);

	ocr42.set(timeInForce);
	ocr42.set(orderQty);
	ocr42.set(price);
	ocr42.getHeader().setField(FIX::SenderCompID(_config.senderCompID));
	ocr42.getHeader().setField(FIX::TargetCompID(_config.targetCompID));
	pan::log_DEBUG("SendToTarget(ocr42)");
	bool sendOK = FIX::Session::sendToTarget(ocr42);
    if (sendOK == false) {
        pan::log_CRITICAL("SendToTarget(ocr42) FAILED for OID: ", pan::blob(clordid.get_uuid(), UUID_LEN), 
                " full order information follows: \n", 
                ocr42.toString().c_str());
    }

}



void Application::orderCancel42(capkproto::order_cancel& oc)
{
	pan::log_DEBUG("Application::orderCancel42()");
	// OrigOrdID
	char origoidbuf[UUID_LEN+1];
	order_id_t origoid;
	origoid.set(oc.orig_order_id().c_str(), oc.orig_order_id().size());
	origoid.c_str(origoidbuf);
	FIX::OrigClOrdID origClOrdID(origoidbuf);
	// ClOrdID
	char clordidbuf[UUID_LEN+1];
	order_id_t clordid;
	clordid.set(oc.cl_order_id().c_str(), oc.cl_order_id().size());
	clordid.c_str(clordidbuf);
	FIX::ClOrdID clOrdID(clordidbuf);
	
	FIX::Symbol symbol(oc.symbol().c_str());
	// side
	FIX::Side side;
	pan::log_DEBUG("SIDE: ", pan::integer(oc.side()));
	if (oc.side() == capkproto::BID) {
		pan::log_DEBUG("SETTING BID");
		side = FIX::Side_BUY;
	}
	else if (oc.side() == capkproto::ASK) {
		pan::log_DEBUG("SETTING ASK");
		side = FIX::Side_SELL;
	}

	FIX::OrderQty orderQty(oc.order_qty());

	FIX42::OrderCancelRequest oc42(origClOrdID,
								clOrdID,	
								symbol,
								side, 
								FIX::TransactTime());

	oc42.set(orderQty);
	oc42.getHeader().setField(FIX::SenderCompID(_config.senderCompID));
	oc42.getHeader().setField(FIX::TargetCompID(_config.targetCompID));
	bool sendOK = FIX::Session::sendToTarget(oc42);
    if (sendOK == false) {
        pan::log_CRITICAL("SendToTarget(oc42) FAILED for OID: ", pan::blob(clordid.get_uuid(), UUID_LEN), 
                " full order information follows: \n", 
                oc42.toString().c_str());
    }

}

void Application::orderCancel43(capkproto::order_cancel& oc)
{
	pan::log_DEBUG("Application::orderCancel43()");
}

void Application::orderCancel44(capkproto::order_cancel& oc)
{
	pan::log_DEBUG("Application::orderCancel44()");
}

void Application::orderCancel50(capkproto::order_cancel& oc)
{
	pan::log_DEBUG("Application::orderCancel50()");
}

void Application::orderStatus42(capkproto::order_status& os)
{
	pan::log_DEBUG("Application::orderStatus42()");
	// OrigOrdID
	char oidbuf[UUID_LEN+1];
	order_id_t oid;
	oid.set(os.order_id().c_str(), os.order_id().size());
	oid.c_str(oidbuf);
	FIX::OrderID orderID(oidbuf);
	// ClOrdID
	char clordidbuf[UUID_LEN+1];
	order_id_t clordid;
	clordid.set(os.cl_order_id().c_str(), os.cl_order_id().size());
	clordid.c_str(clordidbuf);
	FIX::ClOrdID clOrdID(clordidbuf);
	
	FIX::Symbol symbol(os.symbol().c_str());
	// side
	FIX::Side side;
	pan::log_DEBUG("SIDE: ", pan::integer(os.side()));
	if (os.side() == capkproto::BID) {
		pan::log_DEBUG("SETTING BID");
		side = FIX::Side_BUY;
	}
	else if (os.side() == capkproto::ASK) {
		pan::log_DEBUG("SETTING ASK");
		side = FIX::Side_SELL;
	}


	FIX42::OrderStatusRequest os42(clOrdID,
								symbol,
								side);

	os42.set(orderID);
	os42.getHeader().setField(FIX::SenderCompID(_config.senderCompID));
	os42.getHeader().setField(FIX::TargetCompID(_config.targetCompID));
	bool sendOK = FIX::Session::sendToTarget(os42);
    if (sendOK == false) {
        pan::log_CRITICAL("SendToTarget(ocr42) FAILED for OID: ", pan::blob(clordid.get_uuid(), UUID_LEN), 
                " full order information follows: \n", 
                os42.toString().c_str());
    }
}


/*
void 
Application::sndNewOrder(order_id_t& ClOrdID,
					const char* Symbol,
					const side_t BuySell,
					const double OrderQty,
					const short int OrdType,
					const double Price,
					const short int TimeInForce,
					const char* Account)
{
	char oidbuf[UUID_LEN+1];
	ClOrdID.c_str(oidbuf);
	FIX::ClOrdID clordid(oidbuf);
	FIX::OrdType ordtype(OrdType);
	FIX::Side side(BuySell);

	FIX::HandlInst handlinst(_handlInst21);
	FIX::Symbol symbol(Symbol);
	FIX::OrderQty qty(OrderQty);
	FIX::TimeInForce tif(TimeInForce);
	FIX::Price price(Price);


	FIX50::NewOrderSingle nos(clordid, side, FIX::TransactTime(), ordtype);

	nos.set(handlinst);
	nos.set(symbol);
	nos.set(qty);
	nos.set(tif);
	nos.set(price);

	if (_account1 != "") {
		nos.set(FIX::Account(_account1));
	}
	if (_useCurrency15) {
		FIX::Symbol symbol;
		nos.getField(symbol);
		std::string ssym = symbol.getValue();
		std::string curPrefix = ssym.substr(0,3);
#ifdef LOG
		//pan::log_DEBUG("************** CURRENCY PREFIX: ", curPrefix.c_str());
#endif
		nos.set(FIX::Currency(curPrefix.c_str()));
	}

	nos.getHeader().setField(FIX::SenderCompID(_sessionID.getSenderCompID()));
	nos.getHeader().setField(FIX::TargetCompID(_sessionID.getTargetCompID()));

	FIX::Session::sendToTarget(nos);
#ifdef LOG
	pan::log_DEBUG(nos.toString());
#endif

}

*/
