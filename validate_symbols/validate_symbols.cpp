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

//#include "getopt-repl.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include "boost/date_time/gregorian/gregorian.hpp" 



namespace po = boost::program_options;
namespace fs = boost::filesystem; 
namespace dt = boost::gregorian; 

void sighandler(int sig);

// Global pointers for signal handlers to cleanup properly
FIX::SocketInitiator* pinitiator = 0;
FIX::Application* papplication = 0;

std::vector<std::string> 
readSymbols(std::string symbolFileName)
{
	std::string line;
	std::vector<std::string> symbols; 
	std::ifstream symbolsFile(symbolFileName);
	//std::cerr << "---->BEGIN CONFIG" << std::endl;
	if (symbolsFile.is_open()) {
		while(symbolsFile.good()) {
			std::getline(symbolsFile, line);
			//std::cerr << "LINE: " << line << std::endl;
			symbols.push_back(line);	
		}
	}
	//std::cerr << "---->END CONFIG" << std::endl;
	return symbols;

}

int main( int argc, char** argv )
{
	int err = 0;
	std::string argOutputDir; 
	std::string symbolFile;
	std::string configFile;
	std::string password;
	bool printDebug; 
    

    (void) signal(SIGINT, sighandler);
    (void) signal(SIGTERM, sighandler);
    (void) signal(SIGHUP, sighandler);


	try {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("s", po::value<std::string>(), "<symbol file>")
			("c", po::value<std::string>(), "<config file>")
			("d", "debug info")
		;
		
		po::variables_map vm;        
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);    

		
		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 1;
		}
		
		if (vm.count("s")) {
			std::cout << "Symbol file: " << vm["s"].as<std::string>() << "\n";
			symbolFile = vm["s"].as<std::string>();
		} else {
			// use default name for symbols file name 
			symbolFile = "symbols.cfg"; 
			std::cout << "Using default symbols file: symbols.cfg\n"; 
		}
		if (vm.count("c")) {
			std::cout << "Config file: " << vm["c"].as<std::string>() << ".\n";
			configFile = vm["c"].as<std::string>();
		} else {
			std::cout << "Config file was not set.\n";
			err++;
		}
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

        // Fix Version string
		config.version = dict.has("FIXVersion") ? (FIXVersion)atoi(dict.getString("FIXVersion").c_str()) : FIX_42;
		std::cout << "Using FIX version: " << config.version << std::endl;

        // Debug settings
		config.printDebug = printDebug; 
        
		Application application(config);
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

        
		FIX::FileStoreFactory storeFactory(storeOutputDir);         
		FIX::FileLogFactory logFactory(logOutputDir);

		FIX::SocketInitiator initiator(application, storeFactory, settings, logFactory);
        pinitiator = &initiator;
		std::cout << "Starting initiator" << std::endl; 
		initiator.start();
        //application.run();

		char x;
		while(std::cin >> x) {
            std::cout << " Press 'q' to quit" << std::endl;
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
    exit(sig); 
}


