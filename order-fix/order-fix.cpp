/*****************************************************************************
  Copyright (C) 2011,2012 Capital K Partners BV

 No part of this code may be reproduced in whole or in part without express
 permission from Capital K partners.  

 Think. 
 
 *****************************************************************************/

#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#else
//#include "config.h"
#endif

#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/ThreadedSocketInitiator.h"
#include "quickfix/SessionSettings.h"
#include "Application.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include "boost/date_time/gregorian/gregorian.hpp" 

#include <zmq.hpp>

#include <google/protobuf/stubs/common.h>
//#include "../proto/spot_fx_md_1.pb.h"

#include "logging.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem; 
namespace dt = boost::gregorian; 

void signal_handler(int signal_value);
void s_catch_signals();

// Global pointers for signal handlers to cleanup properly
FIX::SocketInitiator* pinitiator;
Application* papplication;

// ZMQ context - can't have these go out of scope
zmq::context_t ctx(1);

int main( int argc, char** argv )
{
	int err = 0;
	std::string configFile;
	bool printDebug; 
	bool runInteractive;

	logging_init((std::string(argv[0]) + ".log").c_str());

	s_catch_signals();
	// signal handlers for shutting down cleanly
/*
    (void) signal(SIGINT, sighandler);
    (void) signal(SIGTERM, sighandler);
    (void) signal(SIGHUP, sighandler);
*/

    GOOGLE_PROTOBUF_VERIFY_VERSION;

	try {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("c", po::value<std::string>(), "<config file>")
			("d", "debug info")
			("i", "run interactive mode")
		;
		
		po::variables_map vm;        
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);    

        // help		
		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 1;
		}
        // config file
		if (vm.count("c")) {
			pan::log_NOTICE("Config file: ", vm["c"].as<std::string>().c_str());
			configFile = vm["c"].as<std::string>();
		} else {
			pan::log_ERROR("Config file not set"); 
			err++;
		}
		if (vm.count("i")) {
			pan::log_NOTICE("Running interactive TEST MODE ");
			runInteractive = true;
		}
        // debug? 
		printDebug = vm.count("d") > 0; 
			
	}
	catch(std::exception& e) {
#ifdef LOG
		pan::log_CRITICAL("Exception parsing parameters");
#endif
		return 1;
	}
	if (err > 0) {
#ifdef LOG
		pan::log_CRITICAL("Aborting due to missing parameters");
#endif
		return 1;
	}
	
	/* 
 	* @see http://www.boost.org/doc/libs/1_41_0/doc/html/program_options.html
 	*/
	
	try
	{
		// We only handle one session per connection
		FIX::SessionSettings settings(configFile);
                std::set<FIX::SessionID> sessions = settings.getSessions ();
		assert(sessions.size() == 1); 

		FIX::SessionID sessionId = *(sessions.begin()); 
		const FIX::Dictionary& dict = settings.get(sessionId);
		ApplicationConfig config;  
        // MIC code for adding to output filename 
		config.mic_code = dict.has("MIC") ? dict.getString("MIC") : ""; 

        // Username and password settings
		config.username = dict.has("Username") ? dict.getString("Username") : ""; 
		config.password = dict.has("Password") ? dict.getString("Password") : ""; 
		config.sendPasswordInRawDataField = dict.has("SendPasswordInRawData") && 
		    dict.getBool("SendPasswordInRawData");

        // Fix Version string
		config.version = dict.has("FIXVersion") ? static_cast<FIXVersion>(atoi(dict.getString("FIXVersion").c_str())) : FIX_42;
		pan::log_INFORMATIONAL("Using FIX version: ", pan::integer(config.version)); 

        // Should we reset sequence numbers? 
		bool bReset = dict.has("ResetSeqNo") && dict.getBool("ResetSeqNo");  
#ifdef LOG
		pan::log_NOTICE("Resetting sequence numbers: ", pan::boolean(bReset));
#endif

		// Account name
		std::string account = dict.has("Account1") ? dict.getString("Account1").c_str() : "";
#ifdef LOG
		pan::log_INFORMATIONAL("Account1: ", account);
#endif

		// HandlInst as a CHAR not an int
		std::string handlInstStr = (dict.has("HandlInst21") ? dict.getString("HandlInst21"): "");
		char handlInst = handlInstStr[0];
#ifdef LOG
		pan::log_INFORMATIONAL("HandlInst21: ", pan::character(handlInst));
#endif
		
		// LimitOrder as a CHAR not an int
		std::string limitOrderStr = (dict.has("LimitOrder40") ? dict.getString("LimitOrder40"): "2");
		char limitOrder = limitOrderStr[0];
#ifdef LOG
		pan::log_INFORMATIONAL("LimitOrder40: ", pan::character(limitOrder));
#endif

		// Should we send currency in tag 15 or not?  
		bool useCurrency = dict.has("UseCurrency15") && dict.getBool("UseCurrency15");
#ifdef LOG
		pan::log_INFORMATIONAL("UseCurrency15: ", pan::boolean(useCurrency));
#endif


        // FIX::begin string
		config.begin_string = dict.has("BeginString") ? dict.getString("BeginString").c_str() : "UNKNOWN FIX VERSION";
#ifdef LOG
		pan::log_INFORMATIONAL("Using FIX BeginString: ", config.begin_string);
#endif
        // FIX::sender comp id 
		config.senderCompID = dict.has("SenderCompID") ? dict.getString("SenderCompID").c_str() : "";
		if (config.senderCompID == "") { 
			pan::log_CRITICAL("SenderCompID may NOT be empty");
			return (-1);
		}
#ifdef LOG
		pan::log_INFORMATIONAL("Using SenderCompID: ", config.senderCompID);
#endif
        // FIX:: target comp id 
		config.targetCompID = dict.has("TargetCompID") ? dict.getString("TargetCompID").c_str() : "";
		if (config.targetCompID == "") { 
			pan::log_CRITICAL("TargetCompID may NOT be empty");
			return (-1);
		}
#ifdef LOG
		pan::log_INFORMATIONAL("Using TargetCompID: ", config.targetCompID);
#endif

        // venue ID
		config.venueID = dict.has("VenueID") ? atoi(dict.getString("VenueID").c_str()) : 0;
		if (config.venueID == 0) { 
			pan::log_CRITICAL("VenueID may NOT be empty or 0");
			return (-1);
		}
#ifdef LOG
		pan::log_INFORMATIONAL("Using VenueID: ", pan::integer(config.venueID));
#endif

        // Debug settings
		config.printDebug = printDebug; 

		Application application(bReset, config);
        papplication = &application;
    
        std::string logOutputDir = dict.has("FileLogPath") ? dict.getString("FileLogPath") : "."; 
        std::string storeOutputDir = dict.has("FileStorePath") ? dict.getString("FileStorePath") : ".";

        pid_t pid = getpid();
        pid_t ppid = getppid();
        
#ifdef LOG
		pan::log_NOTICE("pid: ", pan::integer(pid), " ppid: ", pan::integer(ppid));
#endif

        std::string pidFileName = std::string(argv[0]) + "." +  config.mic_code + std::string(".pid");
        std::ofstream pidFile(pidFileName);
        if (pidFile.is_open()) {
            pidFile << pid;
            pidFile.flush();
			pidFile.close();
        }
        else {
            std::cerr << "Can't write pid file - exiting";
#ifdef LOG
			pan::log_ALERT("Can't write pid file - exiting");
#endif
            exit(-1);
        }

        // Get the bind address for zmq sockets
        bool isPublishing = false;
        std::string bindAddress = dict.has("ZMQBindAddressIn") ? dict.getString("ZMQBindAddressIn") : "";
        if (bindAddress != "") {
#ifdef LOG
            pan::log_INFORMATIONAL("ZMQ: Binding to zmq address: ", bindAddress);
#endif
			config.zmq_bind_addr = bindAddress;
            isPublishing = true;
        }
        else {
#ifdef LOG
            pan::log_INFORMATIONAL("ZMQ: NOT publishing : ", bindAddress);
#endif
        }

		// Set application params
		application.setHandlInst21(handlInst);
		application.setAccount1(account);
		application.setLimitOrderChar40(limitOrder);
		application.setUseCurrency15(useCurrency);

        // ZMQ initialization
		application.setZMQContext(&ctx);
        
		FIX::FileStoreFactory storeFactory(storeOutputDir);         
		FIX::FileLogFactory logFactory(logOutputDir);

		FIX::SocketInitiator initiator(application, storeFactory, settings, logFactory);
        pinitiator = &initiator;
#ifdef LOG
		pan::log_INFORMATIONAL("Starting initiator"); 
#endif
		initiator.start();

		if (runInteractive) {
			application.test();
		}
		else {
			application.run();
		}
/*
		char x;
		while(std::cin >> x) {
			if (x == 'q') {
				break;
			}
		}
*/
#ifdef LOG
		pan::log_INFORMATIONAL("Stopping initiator"); 
#endif
		initiator.stop();
		return 0;
	}
	catch ( FIX::Exception & e )
	{
		std::cout << e.what();
		return 1;
	}
    
}

void
s_catch_signals()
{
	struct sigaction action;
	action.sa_handler = signal_handler;
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
    sigaction(SIGHUP, &action, NULL);
}

void 
signal_handler(int signal_value) 
{  
#ifdef LOG
	pan::log_CRITICAL("Stopping - received signal:", pan::integer(signal_value)); 
#endif
    assert(pinitiator);
	pinitiator->stop();
	assert(papplication);
	papplication->_pMsgProcessor->stop();
}


