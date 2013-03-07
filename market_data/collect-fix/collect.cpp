
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
#include "utils/logging.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem; 
namespace dt = boost::gregorian; 
namespace qi = boost::spirit::qi;

/**
 * Constants for defaults
 */
const char* const DEFAULT_SYMBOLS_FILE_NAME = "symbols.cfg";
const char* const DEFAULT_CONFIG_SERVER_ADDRESS = "tcp://127.0.0.1:11111";
const char* const DEFAULT_TICK_FILE_DIR = ".";

/**
 * Signal handler - for ZMQ clean exits
 */
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
 * Read symbol config file - one symbol per line 
 */
std::vector<std::string> 
readSymbols(std::string symbol_file_name)
{
	std::string line;
	std::vector<std::string> symbols;
	std::ifstream symbols_stream(symbol_file_name);
#ifdef LOG
    pan::log_DEBUG("Reading symbol file: ", symbol_file_name);
#endif
	if (symbols_stream.is_open()) {
		while(symbols_stream.good()) {
			std::getline(symbols_stream, line);
#ifdef LOG
            pan::log_DEBUG("Symbol: ",  line.c_str());
#endif
			symbols.push_back(line);	
		}
	}
	return symbols;

}


void
SetSignalHandlers() 
{
    (void) signal(SIGINT, sighandler);
    (void) signal(SIGTERM, sighandler);
    (void) signal(SIGHUP, sighandler);
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

int
InitializeZMQ() 
{
    int zero = 0;  
    /** Initialize the global context */
	g_zmq_context = zmq_init(1);
	assert(g_zmq_context);

    /** Setup the scoekt for publishing prices */
	g_pub_socket = zmq_socket(g_zmq_context, ZMQ_PUB);
    zmq_setsockopt(g_pub_socket, ZMQ_LINGER, &zero, sizeof(zero));
	assert(g_pub_socket);

    if (g_zmq_context == NULL || g_pub_socket == NULL) {
        return (-1);
    }
    return 0;
}

/**
 * Write the file containing the process id to current directory
 * as prefix.suffix.pid
 * ppid is carried as junk for now
 * return 0 on success; -1 on failure
 */
int
WritePidFile(const char* prefix, const char* suffix, pid_t pid, pid_t ppid) 
{ 
    std::string pidFileName = std::string(prefix) + "." +  suffix + std::string(".pid");
    std::ofstream pidFile(pidFileName);
    if (pidFile.is_open()) {
        pidFile << pid;
        pidFile.flush();
        pidFile.close();
        return (0);
    }
    else {
        return (-1);
    }
}


/**
 * Set the ApplicationConfig structure from the FIX::Dictionary that is returned from 
 * parsing the FIX configuration file. Non-standard config params are picked up into the 
 * dictionary when the file is parsed so we just capture them here and put them into 
 * the local ApplicationConfig structure. 
 */
int
ReadLocalConfig(ApplicationConfig* application_config, const FIX::Dictionary& dict)
{
    if (application_config == NULL) {
        return (-1);
    }
    int err = 0;

    pan::log_DEBUG("BEGIN - file based configuration");
    
    // MIC code 
    application_config->mic_string = dict.has("MIC") ? dict.getString("MIC") : ""; 
    pan::log_DEBUG("MIC: ", application_config->mic_string.c_str());

     // Username and password settings
    application_config->username = dict.has("Username") ? dict.getString("Username") : ""; 
    application_config->password = dict.has("Password") ? dict.getString("Password") : ""; 
    application_config->sendPasswordInRawDataField = 
        dict.has("SendPasswordInRawData") && dict.getBool("SendPasswordInRawData");

    // If user specified an output dir, then put files into date-sorted
    // subdirectories, otherwise put into the default dir
    application_config->order_books_output_dir = DEFAULT_TICK_FILE_DIR;
    application_config->fix_log_output_dir = 
        dict.has("FileLogPath") ? 
        dict.getString("FileLogPath") : 
        DEFAULT_TICK_FILE_DIR; 

    application_config->fix_store_output_dir = 
        dict.has("FileStorePath") ? 
        dict.getString("FileStorePath") : 
        DEFAULT_TICK_FILE_DIR;

    // Should use aggregated book?  
    application_config->is_aggregated_book = 
        dict.has("AggregatedBook") && dict.getBool("AggregatedBook");
    pan::log_DEBUG("Aggregated book: ", pan::boolean(application_config->is_aggregated_book));

    // Should we reset sequence numbers? 
    bool reset = dict.has("ResetSeqNo") && dict.getBool("ResetSeqNo");  
    application_config->reset_seq_nums = reset;
    pan::log_DEBUG("Resetting sequence numbers: ", pan::boolean(reset));

    // How to send market data requests - bulk or multiple messages
    application_config->sendIndividualMarketDataRequests = 
        dict.has("SendIndividualMarketDataRequests") && dict.getBool("SendIndividualMarketDataRequests");
    pan::log_DEBUG("Send individual market data requests: ",  pan::boolean(application_config->sendIndividualMarketDataRequests));

    // New orders delete existing orders with same ID before adding?
    application_config->new_replaces = dict.has("new_replaces") ? dict.getBool("new_replaces") : false;
    pan::log_DEBUG("New orders replace existing orders with same id: ", pan::boolean(application_config->new_replaces));

    // Fix Version string
    application_config->version = dict.has("FIXVersion") ? (FIXVersion)atoi(dict.getString("FIXVersion").c_str()) : FIX_42;
    pan::log_DEBUG("Using FIX version: ", pan::integer(application_config->version));

    // Market depth 
    std::string depth = dict.has("MarketDepth") ? dict.getString("MarketDepth") : ""; 
    application_config->market_depth = atoi(depth.c_str());
    if (application_config->market_depth < 0) {
        pan::log_WARNING("MarketDepth has invalid value: ", pan::integer(application_config->market_depth));
        err++;
    }
    pan::log_DEBUG("Setting market depth: ", pan::integer(application_config->market_depth));

    // Update Type 
    long update_type = dict.has("MDUpdateType") ? dict.getLong("MDUpdateType") : -1; 
    if (update_type < 0) {
        pan::log_WARNING("MDUpdateType has invalid value: ", pan::integer(update_type));
        err++;
    }
    application_config->update_type = update_type;
    pan::log_DEBUG("Setting update type: ", pan::integer(update_type));

    // Publishing switch 
    bool is_publishing = dict.has("should_publish_prices") && dict.getBool("should_publish_prices");
    application_config->is_publishing = is_publishing;

    pan::log_DEBUG("END - file based configuration");

    return (err);
}

int 
ReadRemoteConfig(ApplicationConfig* application_config)
{
    pan::log_DEBUG("BEGIN - remote configuration");

    // Received protobuf for configuration
    capkproto::configuration all_venue_config;

    // Call the configuration server
    capk::get_config_params(g_zmq_context, application_config->config_server_addr.c_str(),  &all_venue_config);
    capkproto::venue_configuration my_config = capk::get_venue_config(&all_venue_config, application_config->mic_string.c_str());
    pan::log_DEBUG("My config:\n", my_config.DebugString().c_str(), "\n");

    if (my_config.venue_id() <= 0) {
        pan::log_CRITICAL("venue_id is not set or set to 0");
        return (-1);
    }
    else {
        application_config->venue_id = my_config.venue_id();
        if (application_config->venue_id == 0) {
            pan::log_CRITICAL("venue_id received from config server is 0");
            return (-1);
        }
        pan::log_DEBUG("My venue_id: ", pan::integer(application_config->venue_id));
    }
    
    
    // If publishing get the bind address
    if (application_config->is_publishing) {
        std::string addr = my_config.market_data_broadcast_addr();
        if (addr.length() > 0) {
            application_config->publishing_addr = addr;
            pan::log_DEBUG("Publishing to: ",  my_config.market_data_broadcast_addr().c_str());
        }
        else {
            pan::log_WARNING("Publishing is set but no address configured");
        }
    }
    else {
        pan::log_DEBUG("Not publishing");
    }

    pan::log_DEBUG("END - remote configuration");

    return (0);
}

int
ReadCommandLineParams(int argc, char** argv, ApplicationConfig* application_config)
{
    int err = 0;
    try {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("c", po::value<std::string>(), "<config file>")
			("s", po::value<std::string>(), "<symbol file>")
			("o", po::value<std::string>(), "<orderbook output path>")
			("config-server", po::value<std::string>(), "<config server address>")
			("nolog", po::value<int>()->implicit_value(0), "disable logging (FIX and tick)")
			("d", "debug info")
		;
		
		po::variables_map vm;        
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);    

		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 1;
		}

        application_config->is_logging = true;	
		if (vm.count("nolog")) {
            application_config->is_logging = false;
		}
        pan::log_INFORMATIONAL("Logging:", pan::boolean(application_config->is_logging));

        application_config->root_output_dir = DEFAULT_TICK_FILE_DIR;
	    if (vm.count("o")) {
            application_config->root_output_dir = vm["o"].as<std::string>();
		} 
        pan::log_INFORMATIONAL("Root log dir: ", application_config->root_output_dir.c_str());

        application_config->symbol_file_name = DEFAULT_SYMBOLS_FILE_NAME;
		if (vm.count("s")) {
            application_config->symbol_file_name = vm["s"].as<std::string>();
		} 
        pan::log_INFORMATIONAL("Symbol file: ", application_config->symbol_file_name.c_str());

		if (vm.count("c")) {
			application_config->config_file_name = vm["c"].as<std::string>();
		} else {
            pan::log_WARNING("Config file was not set.");
			err++;
		}
        pan::log_INFORMATIONAL("Config file: ", application_config->config_file_name.c_str()); 

        application_config->print_debug = false;
		if (vm.count("d")) { 
            application_config->print_debug = true;
        }
        pan::log_INFORMATIONAL("Print debug: ", pan::boolean(application_config->print_debug));

        application_config->config_server_addr = DEFAULT_CONFIG_SERVER_ADDRESS;
        if (vm.count("config-server") > 0) {
            application_config->config_server_addr = vm["config-server"].as<std::string>();
        }
        pan::log_INFORMATIONAL("Using config server: ", application_config->config_server_addr);
			
		/** moved passwords to cfg files 
		if (vm.count("p")) {
        pan::log_DEBUG("Pass: " << vm["p"].as<std::string>() << ".\n";
			password = vm["p"].as<std::string>();
		} else {
        pan::log_DEBUG("Password was not set.\n";
			err++;
		}
		*/ 
        return (err);
	}
	catch(std::exception& e) {
		std::cerr << "EXCEPTION:" << e.what();
		return (-1);
	}
}

int 
main(int argc, char** argv )
{
    // Application configuration settings
	ApplicationConfig config;  
	config.print_debug = false;
    config.is_logging = true;

	//std::string root_output_dir; 
	//std::string symbol_file_name;
	//std::string config_file_name;
	//std::string password;

    SetSignalHandlers();

    if (InitializeZMQ() != 0) {
        pan::log_CRITICAL("Can't init ZMQ - exiting");
        return (-1);
    }

    GOOGLE_PROTOBUF_VERIFY_VERSION;

	
	if (ReadCommandLineParams(argc, argv, &config) != 0) {
        pan::log_CRITICAL("Aborting due to missing parameters.");
		return (-1);
	}
	
	std::vector<std::string> symbols = readSymbols(config.symbol_file_name);
    if (symbols.size() <= 0) {
        pan::log_CRITICAL("No symbols set in:", config.symbol_file_name.c_str());
        return (-1);
    }

	try
	{
		FIX::SessionSettings settings(config.config_file_name);
                std::set<FIX::SessionID> sessions = settings.getSessions();
		assert(sessions.size() == 1); 
		FIX::SessionID sessionId = *(sessions.begin()); 
		const FIX::Dictionary& dict = settings.get(sessionId);


        // Get additional config settings from FIX config file
        if (ReadLocalConfig(&config, dict) != 0) {
            return (-1);
        }

        // Get config settings from config server
        if (ReadRemoteConfig(&config) != 0) {
            return (-1);
        }
      
        // Create the FIX application instance
		Application application(config);
        g_papplication = &application;
		application.addSymbols(symbols);
    
        // Create the output directories for orderbooks, log, and store if needed.
        // Note that actual log directories are created in the location specified 
        // with -o arg each time the program starts
        // @TODO (tkaria@capitalkpartners.com) Maybe better to put each log in the dated 
        // directory rather than have store and log on same level as dated tick dirs
        if (config.root_output_dir.length() > 0) { 
            fs::path argPath = fs::path(config.root_output_dir); 
            if (!fs::exists(argPath)) { 
                fs::create_directories(argPath); 
            }
            config.order_books_output_dir = config.root_output_dir;
            
            fs::path fix_log_path = argPath / fs::path("log");  
			if (!fs::exists(fix_log_path)) { 
                fs::create_directory(fix_log_path); 
            } 
			config.fix_log_output_dir = fix_log_path.string(); 

            fs::path store_path = argPath / fs::path("store"); 
			if (!fs::exists(store_path)) { 
                fs::create_directory(store_path); 
            }
			config.fix_store_output_dir = store_path.string(); 
        }

        pid_t pid = getpid();
        pid_t ppid = getppid();

        pan::log_DEBUG("pid: ", pan::integer(pid), " ppid: ", pan::integer(ppid));
        if (WritePidFile(argv[0], config.mic_string.c_str(), pid, ppid) != 0) {
            pan::log_CRITICAL("Can't write pid file - exiting");
            return (-1);
        }

        // ZMQ initialization
        if (config.is_publishing) {
            zmq_bind(g_pub_socket, config.publishing_addr.c_str());
            application.setZMQContext(g_zmq_context);
            application.setZMQSocket(g_pub_socket);
        }


        if (config.is_logging) {
            pan::log_DEBUG("Logging with FileStoreFactory");
		    FIX::FileStoreFactory fileStoreFactory(config.fix_store_output_dir);         
		    FIX::FileLogFactory logFactory(config.fix_log_output_dir);
		    g_pinitiator = new FIX::SocketInitiator(application, fileStoreFactory, settings, logFactory);
            assert(g_pinitiator);
        }
        else {
            pan::log_DEBUG("Logging with NullStoreFactory");
            FIX::NullStoreFactory nullStoreFactory;
		    g_pinitiator = new FIX::SocketInitiator(application, nullStoreFactory, settings);
            assert(g_pinitiator);
        }

        pan::log_DEBUG("Starting initiator");
		g_pinitiator->start();

		char x;
		while(std::cin >> x) {
			if (x == 'q') {
				break;
			}
		}
        pan::log_DEBUG("Stopping initiator...");
		g_pinitiator->stop();
		return 0;
	}
	catch ( FIX::Exception & e ) {
		std::cerr << e.what();
		return 1;
	}
    
}


