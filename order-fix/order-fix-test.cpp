/*****************************************************************************
  Copyright (C) 2011,2012 Capital K Partners BV

   ##        ##       #####                                         #    #    
  #    ####    #     #     #   ##   #####  # #####   ##   #         #   #     
 #    #    #    #    #        #  #  #    # #   #    #  #  #         #  #      
 #    #         #    #       #    # #    # #   #   #    # #         ###       
 #    #         #    #       ###### #####  #   #   ###### #         #  #      
  #   #    #   #     #     # #    # #      #   #   #    # #         #   #     
   ##  ####  ##       #####  #    # #      #   #   #    # ######    #    #    

 No part of this code may be reproduced in whole or in part without express
 permission from Capital K partners.  

 Be Kind - Rewind
 
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

#include "../proto/spot_fx_md_1.pb.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem; 
namespace dt = boost::gregorian; 

void sighandler(int sig);

// Global pointers for signal handlers to cleanup properly
FIX::ThreadedSocketInitiator* pinitiator;
Application* papplication;

// ZMQ Globals - can't have these go out of scope
zmq::context_t context(1);
zmq::socket_t publisher(context, ZMQ_PUB);

int main( int argc, char** argv )
{
	int err = 0;
	std::string configFile;
	bool printDebug; 

    (void) signal(SIGINT, sighandler);
    (void) signal(SIGTERM, sighandler);
    (void) signal(SIGHUP, sighandler);

    GOOGLE_PROTOBUF_VERIFY_VERSION;

	try {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("c", po::value<std::string>(), "<config file>")
			("d", "debug info")
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
			std::cout << "Config file: " << vm["c"].as<std::string>() << ".\n";
			configFile = vm["c"].as<std::string>();
		} else {
			std::cout << "Config file was not set.\n";
			err++;
		}
        // debug? 
		printDebug = vm.count("d") > 0; 
			
	}
	catch(std::exception& e) {
		std::cerr << "EXCEPTION:" << e.what();
		return 1;
	}
	if (err > 0) {
		std::cout << "Aborting due to missing parameters.\n";
		return 1;
	}
	
	/* 
 	* @see http://www.boost.org/doc/libs/1_41_0/doc/html/program_options.html
 	*/
	
	std::vector<std::string> symbols = readSymbols(symbolFile);
	try
	{
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

        // Should use aggregated book?  
        config.aggregatedBook = dict.has("AggregatedBook") && dict.getBool("AggregatedBook");
        std::cout << "Aggregated book: " << config.aggregatedBook << std::endl;

        // Should we reset sequence numbers? 
		bool bReset = dict.has("ResetSeqNo") && dict.getBool("ResetSeqNo");  
		std::cout << "Resetting sequence numbers: " << bReset << std::endl; 

        // How to send market data requests - bulk or multiple messages
		config.sendIndividualMarketDataRequests = 
            dict.has("SendIndividualMarketDataRequests") && dict.getBool("SendIndividualMarketDataRequests");
		std::cout << "Send individual market data requests: " 
            << config.sendIndividualMarketDataRequests << std::endl;

        // Fix Version string
		config.version = dict.has("FIXVersion") ? (FIXVersion)atoi(dict.getString("FIXVersion").c_str()) : FIX_42;
		std::cout << "Using FIX version: " << config.version << std::endl;

        // Market depth 
		std::string depth = dict.has("MarketDepth") ? dict.getString("MarketDepth") : ""; 
        config.marketDepth = atoi(depth.c_str());
		std::cout << "Setting market depth: " << config.marketDepth << std::endl;

        // Debug settings
		config.printDebug = printDebug; 

		Application application(bReset, config);
        papplication = &application;
		application.addSymbols(symbols);
    
        // if user specified an output dir, then put files into date-sorted
        // subdirectories, otherwise put into the default dirs specified in 
        // config file 
        std::string orderBooksOutputDir = ".";
        std::string logOutputDir = dict.has("FileLogPath") ? dict.getString("FileLogPath") : "."; 
        std::string storeOutputDir = dict.has("FileStorePath") ? dict.getString("FileStorePath") : ".";

        if (argOutputDir.length() > 0) { 
            orderBooksOutputDir = argOutputDir; 
            fs::path argPath = fs::path(argOutputDir); 
            if (!fs::exists(argPath)) { fs::create_directories(argPath); }
            
            /* put both order books and message logs in subdirs, 
               but put the store at the root dir 
            */ 
            fs::path logOutputPath = argPath / fs::path("log");  
			if (!fs::exists(logOutputPath)) { fs::create_directory(logOutputPath); } 
			logOutputDir = logOutputPath.string(); 

            fs::path storeOutputPath = argPath / fs::path("store"); 
			if (!fs::exists(storeOutputPath)) { fs::create_directory(storeOutputPath); }
			storeOutputDir = storeOutputPath.string(); 
        }

        pid_t pid = getpid();
        pid_t ppid = getppid();
        
        printf("pid: %d, ppid: %d\n", pid, ppid);
        std::string pidFileName = std::string(argv[0]) + "." +  config.mic_code + std::string(".pid");
        std::ofstream pidFile(pidFileName);
        if (pidFile.is_open()) {
            pidFile << pid;
            pidFile.flush();
        }
        else {
            std::cerr << "Can't write pid file - exiting";
            exit(-1);
        }

        // Get the bind address for zmq sockets
        bool isPublishing = false;
        std::string bindAddress = dict.has("ZMQBindAddress") ? dict.getString("ZMQBindAddress") : "";
        if (bindAddress != "") {
            std::cout << "ZMQ: Binding to zmq address: " << bindAddress << std::endl;
            isPublishing = true;
        }
        else {
            std::cout << "ZMQ: NOT publishing prices: " << std::endl; 
        }

        // ZMQ initialization
        if (isPublishing) {
            std::cout << "ZMQ: Seting publishing params" << std::endl;
            publisher.bind(bindAddress.c_str());
            application.setZMQContext(&context);
            application.setZMQSocket(&publisher);
        }
        application.setPublishing(isPublishing);
        
        // orderbook output setup
		application.setDataPath(orderBooksOutputDir);
		FIX::FileStoreFactory storeFactory(storeOutputDir);         
		FIX::FileLogFactory logFactory(logOutputDir);

        // symbol validation
        application.setValidateSymbols(doValidateSymbols);

		FIX::ThreadedSocketInitiator initiator(application, storeFactory, settings, logFactory);
        pinitiator = &initiator;
		std::cout << "Starting initiator" << std::endl; 
		initiator.start();

		char x;
		while(std::cin >> x) {
			if (x == 'q') {
				break;
			}
		}
		std::cout << "Stopping initiator..." << std::endl;
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
sighandler(int sig) {  
    fprintf(stderr, "Received signal: %d\n", sig);
    assert(pinitiator);
	pinitiator->stop();
    papplication->deleteBooks();
    papplication->flushLogs();
    exit(sig); 
}


