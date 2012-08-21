/****************************************************************************
** Copyright (c) quickfixengine.org  All rights reserved.
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
#pragma warning(disable : 4503 4355 4786)
#else
//#include "config.h"
#endif

#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketAcceptor.h"
#include "quickfix/Log.h"
#include "quickfix/SessionSettings.h"
#include "Application.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

void wait()
{
  // Put realtime config options in this loop
  // - Adjust vol
  // - Adjust rate
  // - Adjust freq(spike)
  // - Adjust monotonicity
  std::cout << "Type Ctrl-C to quit" << std::endl;
  while(true)
  {
    FIX::process_sleep(1);
  }
}

int main(int argc, char** argv)
{
    int err = 0;
    std::string replayFile;
    double replaySpeed = 1;
    double replayVolatility = 0;
    std::string symbolFile;
    std::string configFile;
    bool printDebug;
    bool useReplayFile = false;


    try {

        po::options_description desc("Allowed options");
        desc.add_options()
            ("c", po::value<std::string>(), "<config file>")
            ("i", po::value<std::string>(), "<input file (FIX message log) >")
            ("v", po::value<double>(), "<volatility >")
            ("t", po::value<double>(), "<replay speed (time/s = speed) so 2=> 2x replay speed >")
            ("d", "debug info")
            ("h", "show usage")
            ("help", "show usage");
            
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
     
        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 1;
        }

        if (vm.count("i")) {
            std::cout << "Replay file: " << vm["i"].as<std::string>() << "\n";
            replayFile = vm["i"].as<std::string>();
            useReplayFile = true;
        } 
        if (vm.count("v")) {
            replayVolatility = vm["v"].as<double>();
            std::cout << "Replay volatility: " << replayVolatility << "\n";
        } 
        if (vm.count("t")) {
            replaySpeed = vm["t"].as<double>();
            std::cout << "Replay speed: " << replaySpeed << "\n";
        } 
        /*else {
            // set default
            replayFile = "./";
            std::cout << "Replay log file was not set - using " << replayFile << "\n";
        }*/
/*
        if (vm.count("s")) {
            std::cout << "Symbol file: " << vm["s"].as<std::string>() << "\n";
            symbolFile = vm["s"].as<std::string>();
        } else {
            // use default name for symbols file name
            symbolFile = "symbols.cfg";
            std::cout << "Using default symbols file: " << symbolFile << "\n";
        }
*/
        if (vm.count("c")) {
            std::cout << "Config file: " << vm["c"].as<std::string>() << "\n";
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
  if (argc != 2)
  {
    std::cout << "usage: " << argv[0]
    << " FILE." << std::endl;
    return 0;
  }
  std::string file = argv[1];
*/

  try
  {
    //const FIX::Dictionary& dict = settings.get(sessionId);
    FIX::SessionSettings settings(configFile);
    const FIX::Dictionary& dict = settings.get();
    std::string logOutputDir = dict.has("FileLogPath") ? dict.getString("FileLogPath") : "."; 
    ApplicationConfig config;
    // MIC code for adding to output filename
    config.mic_code = dict.has("MIC") ? dict.getString("MIC") : "";
    // Fix Version string
    config.version = dict.has("FIXVersion") ? (FIXVersion)atoi(dict.getString("FIXVersion").c_str()) : FIX_42;
    std::cout << "Using FIX version: " << config.version << std::endl;
 
    // Debug settings
    config.printDebug = printDebug;
        
    Application application(config);
    if (useReplayFile) {
        std::cout << "Using replay log: " << replayFile << std::endl;
        application.setReplayLog(replayFile);
    }
    application.setReplaySpeed(replaySpeed);
    application.setReplayVolatility(replayVolatility);
    FIX::FileStoreFactory storeFactory(settings);
    //FIX::ScreenLogFactory logFactory(settings);
    FIX::FileLogFactory logFactory(logOutputDir);
    FIX::SocketAcceptor acceptor(application, storeFactory, settings, logFactory);

    acceptor.start();
    wait();
    acceptor.stop();
    return 0;
  }
  catch (std::exception & e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
}

