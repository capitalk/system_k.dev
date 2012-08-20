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
//#include "KFixFields.h"
#include "quickfix/Session.h"
#include "quickfix/FieldConvertors.h"
#include "utils/time_utils.h"
#include "utils/fix_convertors.h"
#include "utils/jenkins_hash.cpp"
#include <iostream>

#include <boost/lexical_cast.hpp>


namespace fs = boost::filesystem; 
namespace dt = boost::gregorian; 

void Application::onLogon(const FIX::SessionID& sessionID )
{
	_loggedIn = true;
	_loggedOut = false;
	_sessionID = sessionID;
	std::cout << "Logged in" << "\n";
	std::cout << "onLogon - " << sessionID << "\n";
	_loginCount++;
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

void Application::onMessage(const FIX42::SecurityDefinition& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
        std::cerr << message << "\n";
	    //std::cerr << "Application::onMessage(const FIX42::SecurityDefinition& message, const FIX::SessionID& sessionID)" << "\n";
    }
	FIX::SecurityReqID securityReqID; 
	FIX::SecurityResponseID securityResponseID; 
	FIX::SecurityResponseType securityResponseType; 
	FIX::TotalNumSecurities totalNumSecurities; 
	FIX::Symbol symbol; 
	FIX::SecurityType securityType; 

	if (message.isSetField(securityReqID)) {
		message.getField(symbol);
        _validSymbols.push_back(symbol); 
        std::cout << symbol << std::endl;
        if (_validSymbols.size() == _nValidSymbols) {
            int i = 0;
            std::cout << "List complete: " << _nValidSymbols << std::endl;
            std::vector<std::string>::const_iterator vit = _validSymbols.begin();
            std::cout << "Valid symbols on: " << _config.mic_code << "\n";
            std::string allSymFilename = _config.mic_code + ".all_supported.symbols";
            std::string validSymFilename = _config.mic_code + ".valid_subset.symbols";
            std::ofstream allSymFile(allSymFilename);
            std::ofstream validSubsetFile(validSymFilename);

            while(vit != _validSymbols.end() && *vit != "") {
                    std::cerr << ++i << ") " << *vit << "\n";
                    allSymFile << *vit << "\n";
                    vit++;
            }
            
            std::vector<std::string>::const_iterator it;// = _symbols.begin();
            //while(it != _symbols.end()) { //  && *it != "") {
            for (it = _symbols.begin(); it != _symbols.end(); it++) {
                if (*it == "") { break; }
                std::cout << "Checking: " << *it << "...";
                if (std::find(_validSymbols.begin(), _validSymbols.end(), *it) != _validSymbols.end()) {
                    std::cout << "OK\n"; 
                    validSubsetFile << *it << "\n";
                }
                else {
                    std::cout << "ERROR\n";
                    std::cerr << *it << " INVALID FOR " << _config.mic_code << std::endl;
                }
                //it++;
            }
			std::cerr << "COMPLETE" << std::endl;
            _querySecurityDefinitionComplete = true;
            std::cout << "All symbols written to: " << allSymFilename << "\n";
            std::cout << "Valid symbols subset written to: " << validSymFilename << "\n";
        }
	}	

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
		//std::cout << "\tReceiving " << totalNumSecurities.getValue() << "\n";
        _nValidSymbols = totalNumSecurities.getValue();
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

void Application::onMessage(const FIX::Message& message, const FIX::SessionID& sessionID) 
{
    if (_config.printDebug) {
	    std::cout << "Application::onMessage(const FIX::Message& message, const FIX::SessionID& sessionID)" << message.toString() << ")" << "\n";
    }
}

void
Application::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID) 
{
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
	if (msgType.getValue() == "3") {
		if (_config.printDebug) {
            std::cerr << "*******************************************************\n";
            std::cerr << "*******************************************************\n";
            std::cerr << "*******************************************************\n";
            std::cerr << "*******************************************************\n";
            std::cerr << "*******************************************************\n";
			std::cerr << "Sending Reject" << "\n";
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
			#ifdef LOG
			pan::log_DEBUG("Resetting sequence numbers");
			#endif 
            FIX::ResetSeqNumFlag flag = FIX::ResetSeqNumFlag_YES;
			message.setField(FIX::FIELD::ResetSeqNumFlag, "Y");
		}
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

char 
Application::menu()
{
    char value;
    std::cout << std::endl 
    << "1) ValidateSecurities" << std::endl
    << "q) Quit\n" << std::endl
    << "Action: ";
    std::cin >> value;
    return value;

}


void 
Application::run()
{
    //while (true) {
        //char action = menu();
        //if (action == '1') {
            //std::cout << "Validating securities" << std::endl;
            querySecurityDefinitionRequest("CAPK");
        //}
        //if (action == 'q') { 
            //break;
        //}
    //}
}

void Application::logout()
{
	if (_config.printDebug) {
	    std::cout << "Application::querySecurityDefinitionRequest(...)" << "\n";
    }

	FIX::Message md;
	switch(_config.version) {
	case 42: md = logout42();
		break;
	case 43: md = logout43();
		break;
	default: 
		throw "Unsupported FIX version"; 
	}
	FIX::Session::sendToTarget(md);
}

FIX42::Logout
Application::logout42() 
{
    FIX42::Logout message;
	message.getHeader().setField(_sessionID.getSenderCompID());
	message.getHeader().setField(_sessionID.getTargetCompID());
    return message;
}

FIX43::Logout
Application::logout43()
{
    FIX43::Logout message;
	message.getHeader().setField(_sessionID.getSenderCompID());
	message.getHeader().setField(_sessionID.getTargetCompID());
    return message;
}

void 
Application::querySecurityDefinitionRequest(const std::string& securityReqID) 
{
	if (_config.printDebug) {
	    std::cout << "Application::querySecurityDefinitionRequest(...)" << "\n";
    }

	FIX::Message md;
	switch(_config.version) {
	case 42: md = querySecurityDefinitionRequest42(securityReqID);
		break;
	case 43: md = querySecurityDefinitionRequest43(securityReqID);
		break;
	default: 
		throw "Unsupported FIX version"; 
	}
	FIX::Session::sendToTarget(md);
}

FIX42::SecurityDefinitionRequest 
Application::querySecurityDefinitionRequest42(const std::string& securityReqID)
{

	if (_config.printDebug) {
	    std::cout << "Application::querySecurityDefinitionRequest42(...)" << "\n";
    }
    std::string reqID("CAPK-SecDefReq");

	FIX42::SecurityDefinitionRequest message;
    message.set(FIX::SecurityReqID(reqID.c_str()));
    message.set(FIX::SecurityRequestType(FIX::SecurityRequestType_REQUEST_LIST_SECURITIES));

	message.getHeader().setField(_sessionID.getSenderCompID());
	message.getHeader().setField(_sessionID.getTargetCompID());

    return message;
}

FIX43::SecurityDefinitionRequest
Application::querySecurityDefinitionRequest43(const std::string& securityReqID)
{

	if (_config.printDebug) {
	    std::cout << "Application::querySecurityDefinitionRequest42(...)" << "\n";
    }
    std::string reqID("CAPK-SecDefReq");

	FIX43::SecurityDefinitionRequest message;
    message.set(FIX::SecurityReqID(reqID.c_str()));
    message.set(FIX::SecurityRequestType(FIX::SecurityRequestType_REQUEST_LIST_SECURITIES));

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


Application::~Application() 
{
}
