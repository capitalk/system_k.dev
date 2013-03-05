
#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#else
//#include "config.h"
#endif

#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/NullStore.h"
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
#include <boost/date_time/gregorian/gregorian.hpp> 
#include <boost/spirit/include/qi.hpp>

#include <zmq.hpp>

#include "proto/spot_fx_md_1.pb.h"
#include "proto/venue_configuration.pb.h"

#include "utils/config_server.h"
#include "utils/config_server.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem; 
namespace dt = boost::gregorian; 
namespace qi = boost::spirit::qi;

void sighandler(int sig);

/**
 * Global pointers for signal handlers to cleanup properly
 */
FIX::SocketInitiator* g_pinitiator = NULL;
Application* g_papplication = NULL;

/*
 * ZMQ Globals 
 */
void *g_zmq_context = NULL;
void *g_pub_socket = NULL;

/**
 * configuration protobuf to receive into from config server
 */
capkproto::configuration all_venue_config;

/**
 * Read symbol config file - one symbol per line 
 */
std::vector<std::string> 
readSymbols(std::string symbolFileName)
{
	std::string line;
	std::vector<std::string> symbols;
	std::ifstream symbolsFile(symbolFileName);
#ifdef LOG
    pan::log_DEBUG("Reading symbol file: ", symbolFileName);
#endif
	if (symbolsFile.is_open()) {
		while(symbolsFile.good()) {
			std::getline(symbolsFile, line);
#ifdef LOG
            pan::log_DEBUG("Symbol: ",  pan::integer(line));
#endif
			symbols.push_back(line);	
		}
	}
	return symbols;

}

int main( int argc, char** argv )
{
	int err = 0;
	std::string arg_output_dir; 
	std::string symbil_file_name;
	std::string config_file_name;
	std::string password;
	bool print_debug; 
    bool is_logging;
    int zero = 0;  

    (void) signal(SIGINT, sighandler);
    (void) signal(SIGTERM, sighandler);
    (void) signal(SIGHUP, sighandler);

	g_zmq_context = zmq_init(1);
	assert(g_zmq_context);

	g_pub_socket = zmq_socket(g_zmq_context, ZMQ_PUB);
    zmq_setsockopt(g_pub_socket, ZMQ_LINGER, &zero, sizeof(zero));
	assert(g_pub_socket);

    GOOGLE_PROTOBUF_VERIFY_VERSION;

	try {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("c", po::value<std::string>(), "<config file>")
			("s", po::value<std::string>(), "<symbol file>")
			("o", po::value<std::string>(), "<output path>")
			("nolog", po::value<int>()->implicit_value(0), "disable logging (FIX and tick)")
			("d", "debug info")
		;
		
		po::variables_map vm;        
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);    

		if (vm.count("nolog")) {
            std::cout << "Logging disabled" << std::endl;
            is_logging = false;
		}
        else { 
            std::cout << "Logging enabled" << std::endl;
            is_logging = true;
        }
		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 1;
		}
		if (vm.count("o")) {
			std::cout << "Output path: " << vm["o"].as<std::string>() << "\n";
            arg_output_dir<std::string>();
		} else {
			// set default
			std::cout << "Output path file was not set \n";
		}
		if (vm.count("s")) {
			std::cout << "Symbol file: " << vm["s"].as<std::string>() << "\n";
			symbil_file_name = vm["s"].as<std::string>();
		} else {
			// use default name for symbols file name 
			symbil_file_name = "symbols.cfg"; 
			std::cout << "Using default symbols file: symbols.cfg\n"; 
		}
		if (vm.count("c")) {
			std::cout << "Config file: " << vm["c"].as<std::string>() << ".\n";
			config_file_name = vm["c"].as<std::string>();
		} else {
			std::cout << "Config file was not set.\n";
			err++;
		}
		print_debug = vm.count("d") > 0; 
			
		/** moved passwords to cfg files 
		if (vm.count("p")) {
			std::cout << "Pass: " << vm["p"].as<std::string>() << ".\n";
			password = vm["p"].as<std::string>();
		} else {
			std::cout << "Password was not set.\n";
			err++;
		}
		*/ 
	}
	catch(std::exception& e) {
		std::cerr << "EXCEPTION:" << e.what();
		return 1;
	}
	if (err > 0) {
		std::cout << "Aborting due to missing parameters.\n";
		return 1;
	}
	
	std::vector<std::string> symbols = readSymbols(symbil_file_name);

	try
	{
		FIX::SessionSettings settings(config_file_name);
                std::set<FIX::SessionID> sessions = settings.getSessions ();
		assert(sessions.size() == 1); 
		FIX::SessionID sessionId = *(sessions.begin()); 
		const FIX::Dictionary& dict = settings.get(sessionId);
		ApplicationConfig config;  

        // MIC code for adding to output filename 
		config.mic_string = dict.has("MIC") ? dict.getString("MIC") : ""; 
        std::cout << "My MIC is: " << config.mic_string << std::endl;


        capk::get_config_params(g_zmq_context, "tcp://127.0.0.1:11111", &all_venue_config);
        capkproto::venue_configuration my_config = capk::get_venue_config(&all_venue_config, config.mic_string.c_str());
        std::cout << "Received config:\n***\n" << my_config.DebugString() << "***\n" << std::endl;

        if (my_config.venue_id() <= 0) {
            std::cerr << "venue_id not set!(" << my_config.venue_id() << ")" << std::endl;
            exit(-1);
        }
        else {
            // boost version of atoi
            //if (qi::parse(my_config.venue_id().begin(), my_config.venue_id().end(),  qi::int_, config.venue_id) == false) {
                //std::cout << "Can't parse venue_id"  << std::endl;
                //exit(-1); 
            //}
            config.venue_id = my_config.venue_id();
            if (config.venue_id == 0) {
                std::cerr << "venue_id can not be 0" << std::endl;
                exit(-1);
            }
            std::cout << "Set venue_id to: " << config.venue_id << std::endl;
        }

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

        // Update Type 
		long updateType = dict.has("MDUpdateType") ? dict.getLong("MDUpdateType") : -1; 
		std::cout << "Setting update type: " << updateType << std::endl;

        // Debug settings
		config.print_debug = print_debug; 

		Application application(bReset, config);
        g_papplication = &application;
		application.addSymbols(symbols);
    
        // if user specified an output dir, then put files into date-sorted
        // subdirectories, otherwise put into the default dirs specified in 
        // config file 
        std::string orderBooksOutputDir = ".";
        std::string logOutputDir = dict.has("FileLogPath") ? dict.getString("FileLogPath") : "."; 
        std::string storeOutputDir = dict.has("FileStorePath") ? dict.getString("FileStorePath") : ".";

        if (arg_output_dir.length() > 0) { 
            orderBooksOutputDir = arg_output_dir; 
            fs::path argPath = fs::path(arg_output_dir); 
            if (!fs::exists(argPath)) { fs::create_directories(argPath); }
            
            /** put both order books and message logs in subdirs, 
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
        std::string pidFileName = std::string(argv[0]) + "." +  config.mic_string + std::string(".pid");
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
        bool isPublishing = dict.has("should_publish_prices") && dict.getBool("should_publish_prices");
        if (isPublishing) {
            std::cout << "Collector is publishing prices to: " << my_config.market_data_broadcast_addr() << std::endl;
        }
        else {
            std::cout << "Collector is NOT publishing prices" << std::endl;
        }

        // ZMQ initialization
        if (isPublishing) {
            zmq_bind(g_pub_socket, my_config.market_data_broadcast_addr().c_str());
            application.setZMQContext(g_zmq_context);
            application.setZMQSocket(g_pub_socket);
        }
        application.setPublishing(isPublishing);
        application.setLogging(is_logging);

		// Set MDUpdateType
		application.setUpdateType(updateType);
        
        // orderbook output setup
		application.setDataPath(orderBooksOutputDir);
        // fix logging params

        if (is_logging) {
            std::cout << "Logging with FileStoreFactory" << std::endl;
		    FIX::FileStoreFactory fileStoreFactory(storeOutputDir);         
		    FIX::FileLogFactory logFactory(logOutputDir);
		    g_pinitiator = new FIX::SocketInitiator(application, fileStoreFactory, settings, logFactory);
        }
        else {
            std::cout << "Logging with NullStoreFactory" << std::endl;
            FIX::NullStoreFactory nullStoreFactory;
		    g_pinitiator = new FIX::SocketInitiator(application, nullStoreFactory, settings);
        }
        //g_pinitiator = &initiator;
		std::cout << "Starting initiator" << std::endl; 
		g_pinitiator->start();

		char x;
		while(std::cin >> x) {
			if (x == 'q') {
				break;
			}
		}
		std::cout << "Stopping initiator..." << std::endl;
		g_pinitiator->stop();
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
    assert(g_pinitiator);
	g_pinitiator->stop();
    g_papplication->deleteBooks();
    g_papplication->flushLogs();
    exit(sig); 
}


