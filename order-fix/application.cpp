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

#include "application.h"

#include "quickfix/Session.h"
#include "quickfix/FieldConvertors.h"

#include "utils/time_utils.h"
#include "utils/fix_convertors.h"
#include "utils/jenkins_hash.h"
#include "utils/venue_globals.h"

#include <iostream>


namespace fs = boost::filesystem; 
namespace dt = boost::gregorian; 

Application::Application(bool bReset, const ApplicationConfig& config) 
         :  OrderInterface(config.venue_id), 
            _pMsgProcessor(0), 
            _loggedIn(false), 
            _loggedOut(false), 
            _loginCount(0), 
            _appMsgCount(0),  
            _config(config), 
            _resetSequence(bReset),
            _pzmq_context(0),
            _pzmq_strategy_reply_sock(0)
{
	_handlInst21 = -1;
	_account1 = "";
#ifdef LOG
	pan::log_INFORMATIONAL("Application()");
#endif
}

Application::~Application()
{
#ifdef LOG
	pan::log_DEBUG("Application()");
#endif
	//assert(_pzmq_strategy_reply_sock);
    if (_pzmq_strategy_reply_sock) {
	    _pzmq_strategy_reply_sock->close();
    }
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
		pan::log_CRITICAL("toApp() - Field not found (" , 
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
	pan::log_NOTICE("Trading session ID: ", id.getString().c_str()); 
    if (status.getValue() == FIX::TradSesStatus_OPEN) {
		pan::log_NOTICE("Trading session status is OPEN"); 
    }
    if (status.getValue() == FIX::TradSesStatus_HALTED) {
		pan::log_NOTICE("Trading session status is HALTED"); 
    }
    if (status.getValue() == FIX::TradSesStatus_CLOSED) {
		pan::log_NOTICE("Trading session status is CLOSED"); 
    }
    if (status.getValue() == FIX::TradSesStatus_PREOPEN) {
		pan::log_NOTICE("Trading session status is PREOPEN"); 
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
	pan::log_DEBUG("OnMessage(ExecutionReport42) received FIX msg:\n",  message.toString());		
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
    FIX::Account account;
    FIX::ExecRefID execRefID;

	order_id_t cloid;
	order_id_t origcloid;
	int ret;

	if (message.isSetField(clOrdID)) {
		message.getField(clOrdID);
#ifdef LOG
		pan::log_DEBUG("OnMessage(ExecutionReport42) received FIX::clOrdID: ", 
                clOrdID.getString().c_str());
#endif
		ret = cloid.parse(clOrdID.getString().c_str());
		assert(ret == 0);
		er->set_cl_order_id(cloid.get_uuid(), cloid.size());
#ifdef LOG
		pan::log_DEBUG("OnMessage(ExecutionReport) converted OID to: ", 
                pan::blob(cloid.get_uuid(), cloid.size())); 
#endif
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
	if (message.isSetField(account)) {
		message.getField(account);
		er->set_account(account);
//pan::log_DEBUG("Q");
	}
   	if (message.isSetField(execRefID)) {
		message.getField(execRefID);
		er->set_exec_ref_id(execRefID.getValue());
//pan::log_DEBUG("Q");
	}
//er->set_mic(_config.mic_string);
    er->set_venue_id(_config.venue_id);
	
#ifdef LOG
    pan::log_DEBUG("OnMessage(ExecutionReport42) forwarded protobuf:\n", er->DebugString());
#endif
	// 1) lookup order id to fetch strategy id
	// 2) send the strategy id back to msg processor so that we can route 
	// properly back to strategy
	// 3) send the data in the message
	// 4) end the transmission
    strategy_id_t strategy_id;
    if (_pMsgProcessor) {
        bool sndOK;
        assert(_pMsgProcessor);
        KOrderCache* ocache = _pMsgProcessor->getOrderCache();
        assert(ocache);
        OrderInfo_ptr oinfo = ocache->get(cloid);
        if (!oinfo) {
            pan::log_CRITICAL("OnMessage(ExecutionReport42) no order info for orderid - can't lookup sid in order cache");	
        }
        else {
            strategy_id = oinfo->getStrategyID();
        }
        // create strategy id frame and send it with more flag set
        zmq::message_t sidframe(strategy_id.size());	
        memcpy(sidframe.data(), strategy_id.get_uuid(), strategy_id.size());
        sndOK = _pzmq_strategy_reply_sock->send(sidframe, ZMQ_SNDMORE);
        assert(sndOK);

        // send the message type
        zmq::message_t msgtypeframe(sizeof(capk::EXEC_RPT));
        memcpy(msgtypeframe.data(), &capk::EXEC_RPT, sizeof(capk::EXEC_RPT));
        sndOK = _pzmq_strategy_reply_sock->send(msgtypeframe, ZMQ_SNDMORE);
        assert(sndOK);

        // send the rest of the message using zero-copy since 
        // exection report was created on heap
        zmq::message_t dataframe(er->ByteSize());
        er->SerializeToArray(dataframe.data(), dataframe.size());
        sndOK = _pzmq_strategy_reply_sock->send(dataframe, 0);
        assert(sndOK);

        // persist the order/trade info
        send_to_serialization_service(strategy_id, *er);
    }
    else {
        pan::log_CRITICAL("OnMessage(ExecutionReport42) - no MsgProcessor to route messages  - not sending message");
    }

    delete er;	
        
}

void 
Application::onMessage(const FIX42::OrderCancelReject& message, 
						const FIX::SessionID& sessionID) 
{
#ifdef LOG
	pan::log_DEBUG("OnMessage(OrderCancelReject42) received FIX msg:\n",  message.toString());		
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
		ret = origcloid.set(origClOrdID.getString().c_str());
		assert(ret == 0);
		ocr->set_orig_cl_order_id(origcloid.get_uuid(), origcloid.size());
	}
	if (message.isSetField(clOrdID)) {
		message.getField(clOrdID);
		ret = cloid.set(clOrdID.getString().c_str());
		assert(ret == 0);
		ocr->set_cl_order_id(cloid.get_uuid(), cloid.size());
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

#ifdef LOG
    pan::log_DEBUG("OnMessage(OrderCancelReject42) forwarded protobuf:\n", ocr->DebugString());
#endif
	// 1) lookup order id to fetch strategy id
	// 2) send the strategy id back to msg processor so that we can route 
	// properly back to strategy
	// 3) send the data in the message
	// 4) end the transmission
	strategy_id_t strategy_id;
    if (_pMsgProcessor) {
        bool sndOK;
        assert(_pMsgProcessor);
        KOrderCache* ocache = _pMsgProcessor->getOrderCache();
        assert(ocache);
        OrderInfo_ptr oinfo = ocache->get(cloid);
        if (!oinfo) {
            pan::log_CRITICAL("OrderCancelReject42() - No order info for orderid - can't lookup sid in order cache");	
        }
        else {
            strategy_id = oinfo->getStrategyID();
        }

        // create strategy id frame and send it with more flag set
        zmq::message_t sidframe(strategy_id.size());	
        memcpy(sidframe.data(), strategy_id.get_uuid(), strategy_id.size());
        sndOK = _pzmq_strategy_reply_sock->send(sidframe, ZMQ_SNDMORE);
        assert(sndOK);

        // send the message type
        zmq::message_t msgtypeframe(sizeof(capk::ORDER_CANCEL_REJ));
        memcpy(msgtypeframe.data(), &capk::ORDER_CANCEL_REJ, sizeof(capk::ORDER_CANCEL_REJ));
        sndOK = _pzmq_strategy_reply_sock->send(msgtypeframe, ZMQ_SNDMORE);
        assert(sndOK);

        // send the rest of the message 
        zmq::message_t dataframe(ocr->ByteSize());
        ocr->SerializeToArray(dataframe.data(), dataframe.size());
        sndOK = _pzmq_strategy_reply_sock->send(dataframe, 0);
        assert(sndOK);
    }
    else {
        pan::log_CRITICAL("OrderCancelReject42() - No MsgProcessor to route messages  - not sending message");
    }
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
void
Application::run()
{
#ifdef LOG
	pan::log_DEBUG("run()");
#endif

	int zero = 0;
	zmq::context_t* ctx = getZMQContext();
	_pMsgProcessor = new capk::MsgProcessor(ctx,
                _config.orderListenerAddr.c_str(),
                "inproc://msgout",
                _config.pingListenerAddr.c_str(),
                1,
                this);

	// create sockets and bind 
	// KTK TODO - create sockets in other thread - should not use sockets 
	// created in thread A from thread B
	_pMsgProcessor->init();
	_pzmq_strategy_reply_sock = new zmq::socket_t(*ctx, ZMQ_DEALER);
	_pzmq_strategy_reply_sock->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
	std::string outaddr = _pMsgProcessor->getOutboundAddr();
    pan::log_NOTICE("Connecting (internal) strategy socket to msg processor addressing socket at: ", 
            outaddr.c_str()); 
	_pzmq_strategy_reply_sock->connect(outaddr.c_str());

    _pzmq_serialization_sock = new zmq::socket_t(*ctx, ZMQ_DEALER);
    _pzmq_serialization_sock->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
    pan::log_NOTICE("Listening (external) for orders at: ", _pMsgProcessor->getListenerAddr());
    pan::log_NOTICE("Connecting to trade serialization service at: ", 
            capk::kTRADE_SERIALIZATION_ADDR);
    _pzmq_serialization_sock->connect(capk::kTRADE_SERIALIZATION_ADDR);
    pan::log_NOTICE("Starting message processor loop");
	_pMsgProcessor->run();

}



// Order interface methods
void
Application::dispatch(capk::msg_t msgType, 
					char* data, 
					size_t len)
{
#ifdef LOG
	pan::log_DEBUG("dispatch() - msgType: ", pan::integer(msgType), " data: ", pan::blob(data, len));
#endif
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

void Application::newOrderSingle42(capkproto::new_order_single& nos)
{
#ifdef LOG
	pan::log_DEBUG("newOrderSingle42() inbound protobuf:\n", nos.DebugString());
#endif 
	// pull the data out of protobuf into FIX elements

    strategy_id_t strategy_id(false);
    bool sid_ok = strategy_id.set(nos.strategy_id().c_str(), nos.strategy_id().size());
    assert (sid_ok); 
    uuidbuf_t cloidbuf;
	order_id_t cloid;
	bool order_id_ok = cloid.set(nos.order_id().c_str(), nos.order_id().size());
	assert(order_id_ok); 
    cloid.c_str(cloidbuf);
	FIX::ClOrdID clOrdID(cloidbuf);

	// symbol
	FIX::Symbol symbol(nos.symbol().c_str());

	// side
	FIX::Side side;
	if (nos.side() == capkproto::BID) {
		side = FIX::Side_BUY;
	}
	else if (nos.side() == capkproto::ASK) {
		side = FIX::Side_SELL;
	}

	// order type
	FIX::OrdType ordType;

	if (nos.ord_type() == capkproto::LIM) {
		ordType = _limitOrderChar40;
	}
	else {
		ordType = nos.ord_type();
	}

	FIX42::NewOrderSingle nos42(clOrdID, 
            FIX::HandlInst(_handlInst21), 
            symbol, 
            side,
		    FIX::TransactTime(), 
            ordType);

	// order quantity
	FIX::OrderQty orderQty(nos.order_qty());
	nos42.set(orderQty);

	// time in force
	FIX::TimeInForce timeInForce;
	if (nos.time_in_force() == capkproto::GFD) {
		timeInForce = FIX::TimeInForce_DAY;
	}
	else if (nos.time_in_force() == capkproto::IOC) {
		timeInForce = FIX::TimeInForce_IMMEDIATE_OR_CANCEL;
	}
    else if (nos.time_in_force() == capkproto::FOK) {
		timeInForce = FIX::TimeInForce_IMMEDIATE_OR_CANCEL;
	}
	nos42.set(timeInForce);

	// price
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
		nos42.set(FIX::Currency(curPrefix.c_str()));
	}

	nos42.getHeader().setField(FIX::SenderCompID(_config.senderCompID));
	nos42.getHeader().setField(FIX::TargetCompID(_config.targetCompID));
	bool sendOK = FIX::Session::sendToTarget(nos42);
#ifdef LOG
    pan::log_DEBUG("newOrderSingle42() outbound FIX msg:\n", nos42.toString());
#endif 
    if (sendOK == false) {
        pan::log_CRITICAL("SendToTarget(nos42) FAILED for OID: ", 
                pan::blob(cloid.get_uuid(), cloid.size())); 
    }
    // persist the order/trade info
    send_to_serialization_service(strategy_id, nos);

}

void Application::newOrderSingle43(capkproto::new_order_single& nos)
{
	pan::log_DEBUG("newOrderSingle43()");
}

void Application::newOrderSingle44(capkproto::new_order_single& nos)
{
	pan::log_DEBUG("newOrderSingle44()");
}

void Application::newOrderSingle50(capkproto::new_order_single& nos)
{
	pan::log_DEBUG("newOrderSingle50()");
}

void Application::orderCancelReplace42(capkproto::order_cancel_replace& ocr)
{

#ifdef LOG
	pan::log_DEBUG("orderCancelReplace42() inbound protobuf:\n", ocr.DebugString());
#endif 
	// OrigOrdID
	//char origoidbuf[UUID_LEN+1];
    uuidbuf_t origoidbuf;
	order_id_t origoid;
	bool orig_cl_order_id_ok = origoid.set(ocr.orig_order_id().c_str(), ocr.orig_order_id().size());
	assert (orig_cl_order_id_ok);
    origoid.c_str(origoidbuf);
	FIX::OrigClOrdID origClOrdID(origoidbuf);
	// ClOrdID
	//char clordidbuf[UUID_LEN+1];
    uuidbuf_t clordidbuf;
	order_id_t clordid;
	
    //origoid.set(ocr.cl_order_id().c_str(), ocr.cl_order_id().size());
	//origoid.c_str(clordidbuf);
	bool cl_order_id_ok = clordid.set(ocr.cl_order_id().c_str(), ocr.cl_order_id().size());
    assert (cl_order_id_ok); 
    clordid.c_str(clordidbuf);

    FIX::ClOrdID clOrdID(clordidbuf);
	
	// symbol
	FIX::Symbol symbol(ocr.symbol().c_str());

	// side
	FIX::Side side;
	if (ocr.side() == capkproto::BID) {
		side = FIX::Side_BUY;
	}
	else if (ocr.side() == capkproto::ASK) {
		side = FIX::Side_SELL;
	}
	
	// order type
	FIX::OrdType ordType;
	if (ocr.ord_type() == capkproto::LIM) {
		ordType = _limitOrderChar40;
	}
	else {
		ordType = ocr.ord_type();
	}
	// order quantity
	FIX::OrderQty orderQty(ocr.order_qty());

	// order price
	FIX::Price price(ocr.price());
	
	// time in force
	FIX::TimeInForce timeInForce;
	if (ocr.time_in_force() == capkproto::GFD) {
		timeInForce = FIX::TimeInForce_DAY;
	}
	else if (ocr.time_in_force() == capkproto::IOC) {
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
	bool sendOK = FIX::Session::sendToTarget(ocr42);
#ifdef LOG
    pan::log_DEBUG("orderCancelReplace42() outbound FIX msg:\n", ocr42.toString());
#endif 
    if (sendOK == false) {
        pan::log_CRITICAL("SendToTarget(ocr42) FAILED for OID: ", pan::blob(clordid.get_uuid(), clordid.size())); 
    }

}



void Application::orderCancel42(capkproto::order_cancel& oc)
{
#ifdef LOG
	pan::log_DEBUG("orderCancel42() inbound protobuf:\n", oc.DebugString());
#endif 
	// OrigOrdID
    uuidbuf_t origoidbuf;
	order_id_t origoid;
	origoid.set(oc.orig_order_id().c_str(), oc.orig_order_id().size());
	origoid.c_str(origoidbuf);
	FIX::OrigClOrdID origClOrdID(origoidbuf);

	// ClOrdID
    uuidbuf_t clordidbuf;
	order_id_t clordid;
	clordid.set(oc.cl_order_id().c_str(), oc.cl_order_id().size());
	clordid.c_str(clordidbuf);
	FIX::ClOrdID clOrdID(clordidbuf);
	
	FIX::Symbol symbol(oc.symbol().c_str());
	// side
	FIX::Side side;
	//pan::log_DEBUG("SIDE: ", pan::integer(oc.side()));
	if (oc.side() == capkproto::BID) {
		//pan::log_DEBUG("SETTING BID");
		side = FIX::Side_BUY;
	}
	else if (oc.side() == capkproto::ASK) {
		//pan::log_DEBUG("SETTING ASK");
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
#ifdef LOG
    pan::log_DEBUG("orderCancel42() outbound FIX msg:\n", oc42.toString());
#endif 
    if (sendOK == false) {
        pan::log_CRITICAL("orderCancel42(): SendToTarget(oc42) FAILED for OID: ", 
                pan::blob(clordid.get_uuid(), clordid.size())); 
    }

}

void Application::orderCancel43(capkproto::order_cancel& oc)
{
	pan::log_DEBUG("orderCancel43()");
}

void Application::orderCancel44(capkproto::order_cancel& oc)
{
	pan::log_DEBUG("orderCancel44()");
}

void Application::orderCancel50(capkproto::order_cancel& oc)
{
	pan::log_DEBUG("orderCancel50()");
}

void Application::orderStatus42(capkproto::order_status& os)
{
#ifdef LOG
	pan::log_DEBUG("orderStatus42() inbound protobuf:\n", os.DebugString());
#endif 
	// OrigOrdID
	//char oidbuf[UUID_LEN+1];
    uuidbuf_t oidbuf;
	order_id_t oid;
	oid.set(os.order_id().c_str(), os.order_id().size());
	oid.c_str(oidbuf);
	FIX::OrderID orderID(oidbuf);
	// ClOrdID
	//char clordidbuf[UUID_LEN+1];
    uuidbuf_t clordidbuf;
	order_id_t clordid;
	clordid.set(os.cl_order_id().c_str(), os.cl_order_id().size());
	clordid.c_str(clordidbuf);
	FIX::ClOrdID clOrdID(clordidbuf);
	
	FIX::Symbol symbol(os.symbol().c_str());
	// side
	FIX::Side side;
	if (os.side() == capkproto::BID) {
		side = FIX::Side_BUY;
	}
	else if (os.side() == capkproto::ASK) {
		side = FIX::Side_SELL;
	}


	FIX42::OrderStatusRequest os42(clOrdID,
								symbol,
								side);

	os42.set(orderID);
	os42.getHeader().setField(FIX::SenderCompID(_config.senderCompID));
	os42.getHeader().setField(FIX::TargetCompID(_config.targetCompID));
	bool sendOK = FIX::Session::sendToTarget(os42);
#ifdef LOG
    pan::log_DEBUG("orderStatus42() outbound FIX msg:\n", os42.toString());
#endif 
    if (sendOK == false) {
        pan::log_CRITICAL("orderStatus42() - SendToTarget(ocr42) FAILED for OID: ",
                pan::blob(clordid.get_uuid(), clordid.size())); 
    }
}

void
Application::send_to_serialization_service(const strategy_id_t& strategy_id, 
        const capkproto::new_order_single& nos)
{
    // PART 1 - Strategy ID
    zmq::message_t strategy_id_msg(strategy_id.size());
    memcpy(strategy_id_msg.data(), strategy_id.get_uuid(), strategy_id.size());
    _pzmq_serialization_sock->send(strategy_id_msg, ZMQ_SNDMORE);

    // PART 2 - Message type
    zmq::message_t msg_type_msg(sizeof(capk::msg_t));
    *(static_cast<int*>(msg_type_msg.data())) = capk::ORDER_NEW;
    _pzmq_serialization_sock->send(msg_type_msg, ZMQ_SNDMORE);

    // PART 3 - Message body
    zmq::message_t serialize_new_order_msg(nos.ByteSize());;
    nos.SerializeToArray(serialize_new_order_msg.data(), 
           serialize_new_order_msg.size()); 
    bool sendOK = _pzmq_serialization_sock->send(serialize_new_order_msg, 0);
    if (sendOK == false) {
        pan::log_CRITICAL("send_to_serialziation_service(new_order_single) - serialization of ORDER_NEW failed full order information follows: \n", 
                nos.DebugString().c_str());
    } 
}

void
Application::send_to_serialization_service(const strategy_id_t& strategy_id, 
        const capkproto::execution_report& er)
{
    // PART 1 - Strategy ID
    zmq::message_t strategy_id_msg(strategy_id.size());
    memcpy(strategy_id_msg.data(), strategy_id.get_uuid(), strategy_id.size());
    _pzmq_serialization_sock->send(strategy_id_msg, ZMQ_SNDMORE);

    // PART 2 - Message type
    zmq::message_t msg_type_msg(sizeof(capk::msg_t));
    *(static_cast<int*>(msg_type_msg.data())) = capk::EXEC_RPT;
    _pzmq_serialization_sock->send(msg_type_msg, ZMQ_SNDMORE);

    // PART 3 - Message body
    zmq::message_t serialize_new_order_msg(er.ByteSize());;
    er.SerializeToArray(serialize_new_order_msg.data(), 
           serialize_new_order_msg.size()); 
    bool sendOK = _pzmq_serialization_sock->send(serialize_new_order_msg, 0);
    if (sendOK == false) {
        pan::log_CRITICAL("send_to_serialization_service(execution_report) - serialization of EXEC_RPT failed full order information follows: \n", 
                er.DebugString().c_str());
    } 
}
/******************************************************************************
 *
 * All code below is only used when --i (interactive) switch is set on command
 * line.
 *
 * ***************************************************************************/

//
// Entry point for interactive run i.e. --i specified as option
//
void 
Application::test()
{

#ifdef LOG
	pan::log_DEBUG("test()");
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
    bool sendOK; 
	if ( queryConfirm( "Send order" ) ) {
		sendOK = FIX::Session::sendToTarget( order );
#ifdef LOG
		pan::log_DEBUG(order.toString());
#endif
        if (!sendOK) {
            pan::log_CRITICAL("SendToTarget failed! - new ");
        }
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
    bool sendOK;
    if ( queryConfirm( "Send cancel" ) ) {
        sendOK = FIX::Session::sendToTarget( cancel );
#ifdef LOG
		pan::log_DEBUG(cancel.toString());
#endif
         if (!sendOK) {
            pan::log_CRITICAL("SendToTarget failed! - cancel");
         } 
    }
    else {
        pan::log_DEBUG("Not sending order");
    }
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
    bool sendOK;
  if ( queryConfirm( "Send replace" ) ) {
    sendOK = FIX::Session::sendToTarget( replace );
#ifdef LOG
		pan::log_DEBUG(replace.toString());
#endif
    if (!sendOK) {
        pan::log_CRITICAL("SendToTarget fialed! - replace");
    }
  }
  else {
      pan::log_DEBUG("Not sending order");
  }
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
		pan::log_DEBUG("queryNewOrderSingle - using FIX tag 15 set to: ", curPrefix.c_str());
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
		pan::log_DEBUG("queryNewOrderSingle - using FIX tag 15 set to: ", curPrefix.c_str());
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
		pan::log_DEBUG("queryNewOrderSingle - using FIX tag 15 set to: ", curPrefix.c_str());
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
		pan::log_DEBUG("queryNewOrderSingle - using FIX tag 15 set to: ", curPrefix.c_str());
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
  << "3) FOK" << std::endl
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
    case '3': return FIX::TimeInForce( FIX::TimeInForce_FILL_OR_KILL );
/*
    case '3': return FIX::TimeInForce( FIX::TimeInForce_AT_THE_OPENING );
    case '4': return FIX::TimeInForce( FIX::TimeInForce_GOOD_TILL_CANCEL );
    case '5': return FIX::TimeInForce( FIX::TimeInForce_GOOD_TILL_CROSSING );
*/
    default: throw std::exception();
  }
}


