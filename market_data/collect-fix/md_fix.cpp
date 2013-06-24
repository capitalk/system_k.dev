#ifdef _MSC_VER
#pragma warning(disable : 4503 4355 4786)
#endif

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/spirit/include/qi.hpp>

#include <zmq.hpp>

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <cassert>

#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/NullStore.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/ThreadedSocketInitiator.h"
#include "quickfix/SessionSettings.h"

#include "proto/spot_fx_md_1.pb.h"
#include "proto/venue_configuration.pb.h"

#include "utils/config_server.h"
#ifdef LOG
#include "utils/logging.h"
#endif // LOG

#include "md_parser_qf.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;
namespace dt = boost::gregorian;
namespace qi = boost::spirit::qi;

/**
 * Defaults
 */
const char* const DEFAULT_SYMBOLS_FILE_NAME = "symbols.cfg";
const char* const DEFAULT_CONFIG_SERVER_ADDRESS = "tcp://127.0.0.1:11111";
const char* const DEFAULT_ROOT_DIR = ".";

#ifdef LOG
const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = "collect-fix";
#endif // LOG

/**
 * Signal handler - needed for clean exits
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

void
SetSignalHandlers()
{
  (void) signal(SIGINT, sighandler);
  (void) signal(SIGTERM, sighandler);
  (void) signal(SIGHUP, sighandler);
}

/**
 * Signal handler
 * @param Signal received
 */
void
sighandler(int sig)
{
  fprintf(stderr, "Received signal: %d\n", sig);
  assert(g_pinitiator);
  g_pinitiator->stop();
  g_papplication->deleteBooks();
  exit(sig);
}

/**
 * Create the ZMQ context and set socket options
 * for the pub socket on which we'll broadcast market data
 * @return 0 on success
 */
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
 * Write a file containing the process id to current directory.
 * File name is written as "prefix.suffix.pid"
 * @parma prefix arbitrary string prefix (can be NULL) for filename
 * @param suffix arbitrary string suffix (can be NULL)
 * @param pid the current process id
 * @parma ppid the curren process parent id - NOT USED junk for now
 * @return 0 on success
 */
int
WritePidFile(const char* prefix, const char* suffix, pid_t pid, pid_t ppid)
{
  std::string pidFileName = std::string(prefix)
      + "."
      +  suffix
      + std::string(".pid");
  std::ofstream pidFile(pidFileName);
  if (pidFile.is_open()) {
    pidFile << pid;
    pidFile.flush();
    pidFile.close();
    return (0);
  } else {
    return (-1);
  }
}


/**
 * Read local configuration file.
 * Sets the ApplicationConfig structure (non-FIX) from the FIX::Dictionary
 * that is returned from parsing the FIX configuration file.
 * Non-standard config params are picked up into the
 * dictionary when the file is parsed so we just capture them here and put them into
 * the local ApplicationConfig structure.
 * The parsing of the file into the dict is part of QuickFix library
 * @param applciation_config The config structure for this module
 * @param dict The FIX::Dictionary structure that holds parsed params
 * @return 0 on success
 */
int
ReadFIXConfig(ApplicationConfig* application_config,
                const FIX::Dictionary& dict)
{
  if (application_config == NULL) {
    return (-1);
  }
  int err = 0;

#ifdef LOG
  pan::log_DEBUG("BEGIN - FIX configuration");
#endif

  // MIC code
  application_config->mic_string =
      dict.has("MIC") ? dict.getString("MIC") : "";
#ifdef LOG
  pan::log_DEBUG("MIC: ", application_config->mic_string.c_str());
#endif

  // Username and password settings
  application_config->username =
      dict.has("Username") ? dict.getString("Username") : "";
  application_config->password =
      dict.has("Password") ? dict.getString("Password") : "";
  application_config->sendPasswordInRawDataField =
    dict.has("SendPasswordInRawData") && dict.getBool("SendPasswordInRawData");

  application_config->fix_log_output_dir =
    dict.has("FileLogPath") ?
    dict.getString("FileLogPath") :
    DEFAULT_ROOT_DIR;

  application_config->fix_store_output_dir =
    dict.has("FileStorePath") ?
    dict.getString("FileStorePath") :
    DEFAULT_ROOT_DIR;

  // Should use aggregated book?
  application_config->want_aggregated_book =
    dict.has("AggregatedBook") && dict.getBool("AggregatedBook");
#ifdef LOG
  pan::log_DEBUG("Aggregated book: ",
                 pan::boolean(application_config->want_aggregated_book));
#endif

  // Should we reset sequence numbers?
  bool reset = dict.has("ResetSeqNo") && dict.getBool("ResetSeqNo");
  application_config->reset_seq_nums = reset;
#ifdef LOG
  pan::log_DEBUG("Resetting sequence numbers: ", pan::boolean(reset));
#endif

  // How to send market data requests - bulk or multiple messages
  application_config->sendIndividualMarketDataRequests =
      dict.has("SendIndividualMarketDataRequests") &&
      dict.getBool("SendIndividualMarketDataRequests");
#ifdef LOG
  pan::log_DEBUG("Send individual market data requests: ",
                 pan::boolean(application_config->sendIndividualMarketDataRequests));
#endif

  // New orders delete existing orders with same ID before adding?
  application_config->new_replaces =
      dict.has("new_replaces") ? dict.getBool("new_replaces") : false;
#ifdef LOG
  pan::log_DEBUG("New orders replace existing orders with same id: ",
                 pan::boolean(application_config->new_replaces));
#endif

  // Fix Version string
  application_config->version =
      dict.has("FIXVersion") ?
      (FIXVersion)atoi(dict.getString("FIXVersion").c_str()) : BAD_FIX_VERSION;

  if (application_config->version == BAD_FIX_VERSION) {
#ifdef LOG
    pan::log_WARNING("Invalid or no FIXVersion specified");
#endif
    err++;
  } else {
#ifdef LOG
    pan::log_DEBUG("Using FIX version: ",
                   pan::integer(application_config->version));
#endif
  }

  // Market depth
  std::string depth =
      dict.has("MarketDepth") ? dict.getString("MarketDepth") : "";
  application_config->market_depth = atoi(depth.c_str());
  if (application_config->market_depth < 0) {
#ifdef LOG
    pan::log_WARNING("MarketDepth has invalid value: ",
                     pan::integer(application_config->market_depth));
#endif
    err++;
  }
#ifdef LOG
  pan::log_DEBUG("Setting market depth: ",
                 pan::integer(application_config->market_depth));
#endif

  // Update Type
  int32_t update_type =
      dict.has("MDUpdateType") ? dict.getLong("MDUpdateType") : -1;
  if (update_type < 0) {
#ifdef LOG
    pan::log_WARNING("MDUpdateType has invalid value: ",
                     pan::integer(update_type));
#endif
    err++;
  }
  application_config->update_type = update_type;
#ifdef LOG
  pan::log_DEBUG("Setting update type: ", pan::integer(update_type));
#endif

  // Publishing switch
  bool is_publishing =
      dict.has("should_publish_prices") &&
      dict.getBool("should_publish_prices");
  application_config->is_publishing = is_publishing;

#ifdef LOG
  pan::log_DEBUG("END - FIX configuration");
#endif

  return (err);
}

int ReadLocalVenueConfig(const std::string & venue_config_file, ApplicationConfig* application_config ) {
#ifdef LOG
  pan::log_DEBUG("BEGIN - local venue configuration");
#endif
  // protobuf for configuration
  capkproto::configuration all_venue_config;
  capk::readVenueConfigFile(venue_config_file, &all_venue_config);
  capkproto::venue_configuration my_config =
      capk::get_venue_config(&all_venue_config,
                             application_config->mic_string.c_str());

  if (my_config.venue_id() <= 0) {
#ifdef LOG
    pan::log_CRITICAL("venue_id is not set or set to 0");
#endif
    return (-1);
  } else {
    application_config->venue_id = my_config.venue_id();
    if (application_config->venue_id == 0) {
#ifdef LOG
      pan::log_CRITICAL("venue_id in config file is 0");
#endif
      return (-1);
   }
#ifdef LOG
    pan::log_DEBUG("My venue_id: ", pan::integer(application_config->venue_id));
#endif
  }

  // If publishing get the bind address
  if (application_config->is_publishing) {
    std::string addr = my_config.market_data_broadcast_addr();
    if (addr.length() > 0) {
      application_config->publishing_addr = addr;
#ifdef LOG
      pan::log_DEBUG("Publishing to: ",
                     my_config.market_data_broadcast_addr().c_str());
#endif
    } else {
#ifdef LOG
      pan::log_WARNING("Publishing is set but no address configured");
#endif
    }
  } else {
#ifdef LOG
    pan::log_DEBUG("Not publishing");
#endif
  }

#ifdef LOG
  pan::log_DEBUG("END - local venue configuration");
#endif

  return (0);
}
/**
 * Read the configuration from the config server.
 * Config is sent in a protobuf that details the network config and
 * other settings that may need to be discovered by other components.
 * Difference between this and local settings is that local file settings
 * are more venue specific and this config is system specific.
 * @param application_config Pointer to application config strucutre
 * that stores settings read from remote config
 * @return 0 on success
 */
int ReadRemoteConfig(ApplicationConfig* application_config) {
#ifdef LOG
  pan::log_DEBUG("BEGIN - remote configuration");
#endif

  // protobuf for configuration
  capkproto::configuration all_venue_config;

  // Call the configuration server
  int ret = capk::readConfigServer(g_zmq_context,
                          application_config->config_server_addr.c_str(),
                          &all_venue_config, 
                          50000);
  if (ret != 0) {
    return -1; 
  }
  capkproto::venue_configuration my_config =
      capk::get_venue_config(&all_venue_config,
                             application_config->mic_string.c_str());
#ifdef LOG
  pan::log_DEBUG("My config:\n", my_config.DebugString().c_str(), "\n");
#endif

  if (my_config.venue_id() <= 0) {
#ifdef LOG
    pan::log_CRITICAL("venue_id is not set or set to 0");
#endif
    return (-1);
  } else {
    application_config->venue_id = my_config.venue_id();
    if (application_config->venue_id == 0) {
#ifdef LOG
      pan::log_CRITICAL("venue_id received from config server is 0");
#endif
      return (-1);
   }
#ifdef LOG
    pan::log_DEBUG("My venue_id: ", pan::integer(application_config->venue_id));
#endif
  }


  // If publishing get the bind address
  if (application_config->is_publishing) {
    std::string addr = my_config.market_data_broadcast_addr();
    if (addr.length() > 0) {
      application_config->publishing_addr = addr;
#ifdef LOG
      pan::log_DEBUG("Publishing to: ",
                     my_config.market_data_broadcast_addr().c_str());
#endif
    } else {
#ifdef LOG
      pan::log_WARNING("Publishing is set but no address configured");
#endif
    }
  } else {
#ifdef LOG
    pan::log_DEBUG("Not publishing");
#endif
  }

#ifdef LOG
  pan::log_DEBUG("END - remote configuration");
#endif

  return (0);
}

/**
 * Controls run time settings.
 * Change runtime behaviour of program. Settings that can be made
 * without requiring a recompile.
 * @param argc number of arguments to main()
 * @param argv array of char* to string arguments to main
 * @param application_config application configuration settings structure
 */
int
ReadCommandLineParams(int argc,
                      char** argv,
                      ApplicationConfig* application_config)
{
  int err = 0;
  try {
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("c", po::value<std::string>(), "<FIX config file>")
    ("v", po::value<std::string>(), "<venue config file>")
    ("s", po::value<std::string>(), "<symbol file>")
    ("o", po::value<std::string>(), "<orderbook output path>")
    ("config-server", po::value<std::string>(), "<config server address>")
    ("nolog", po::value<int>()->implicit_value(0), "disable all FIX message logging")
    ("d", "debug info");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      std::cerr << desc << "\n";
      return 1;
    }

    if (vm.count("nolog")) {
      application_config->is_logging = false;
    }
    else {
      application_config->is_logging = true;
    }
#ifdef LOG
    pan::log_INFORMATIONAL("Logging: ",
                           pan::boolean(application_config->is_logging));
#endif

    application_config->root_output_dir = DEFAULT_ROOT_DIR;
    if (vm.count("o")) {
      application_config->root_output_dir = vm["o"].as<std::string>();
    }
#ifdef LOG
    pan::log_INFORMATIONAL("Root log dir: ",
                           application_config->root_output_dir.c_str());
#endif

    application_config->symbol_file_name = DEFAULT_SYMBOLS_FILE_NAME;
    if (vm.count("s")) {
      application_config->symbol_file_name = vm["s"].as<std::string>();
    } else {
#ifdef LOG
      pan::log_WARNING("Symbol file was not set.");
#endif
      err++;
    }
#ifdef LOG
    pan::log_INFORMATIONAL("Symbol file: ",
                           application_config->symbol_file_name.c_str());
#endif

    if (vm.count("v")) {
      application_config->venue_config_file_name = vm["v"].as<std::string>();
    } 
#ifdef LOG
    pan::log_INFORMATIONAL("Venue config file: ",
                           application_config->symbol_file_name.c_str());
#endif

    if (vm.count("c")) {
      application_config->config_file_name = vm["c"].as<std::string>();
#ifdef LOG
      pan::log_DEBUG(application_config->config_file_name.c_str());
#endif
    } else {
#ifdef LOG
      pan::log_WARNING("FIX config file was not set.");
#endif
      err++;
    }
#ifdef LOG
    pan::log_INFORMATIONAL("FIX config file: ",
                           application_config->config_file_name.c_str());
#endif

    application_config->print_debug = false;
    if (vm.count("d")) {
      application_config->print_debug = true;
    }
#ifdef LOG
    pan::log_INFORMATIONAL("Print debug: ",
                           pan::boolean(application_config->print_debug));
#endif

    application_config->config_server_addr = DEFAULT_CONFIG_SERVER_ADDRESS;
    if (vm.count("config-server") > 0) {
      if (vm.count("v") > 0) {
        fprintf(stderr, "Specify --config-server OR --v but not both");
        return -1;
      }
      application_config->config_server_addr =
          vm["config-server"].as<std::string>();
    }
#ifdef LOG
    pan::log_INFORMATIONAL("Using config server: ",
                           application_config->config_server_addr);
#endif

#ifdef LOG
    pan::log_DEBUG("ERRCODE: ", pan::integer(err));
#endif
    return (err);
  } catch(std::exception& e) {
    std::cerr << "EXCEPTION:" << e.what();
    return (-1);
  }
}

/**
 * Entry point for program.
 */
int
main(int argc, char** argv )
{
  // Application configuration settings
  ApplicationConfig config;
  config.print_debug = false;
  config.is_logging = true;

  SetSignalHandlers();

#ifdef LOG
      logging_init(createTimestampedLogFilename(PANTHEIOS_FE_PROCESS_IDENTITY).c_str());
#endif

  if (InitializeZMQ() != 0) {
#ifdef LOG
    pan::log_CRITICAL("Can't init ZMQ - exiting");
#endif
    return (-1);
  }

  // Must call this to use protobufs
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  if (ReadCommandLineParams(argc, argv, &config) != 0) {
#ifdef LOG
    pan::log_CRITICAL("Aborting due to missing parameters.");
#endif
    return (-1);
  }

  std::vector<std::string> symbols = capk::readSymbolsFile(config.symbol_file_name);
  if (symbols.size() <= 0) {
#ifdef LOG
    pan::log_CRITICAL("No symbols set in:", config.symbol_file_name.c_str());
#endif
    return (-1);
  }

  try {
    FIX::SessionSettings settings(config.config_file_name);
    std::set<FIX::SessionID> sessions = settings.getSessions();
    assert(sessions.size() == 1);
    FIX::SessionID sessionId = *(sessions.begin());
    const FIX::Dictionary& dict = settings.get(sessionId);


    // Get additional config settings from FIX config file
    if (ReadFIXConfig(&config, dict) != 0) {
      fprintf(stderr, "Can't read local config file - exiting\n");
      return (-1);
    }

    if (config.venue_config_file_name == "") {
      // Get config settings from config server
      if (ReadRemoteConfig(&config) != 0) {
        fprintf(stderr, "Can't read remote configuration - exiting\n");
        return (-1);
      }
    }
    else {
      if (ReadLocalVenueConfig(config.venue_config_file_name, &config) != 0) {
        fprintf(stderr, "Can't read local venue configuration - exiting\n");
        return (-1);
      }
    }

    // Create the FIX application instance
    Application application(config);
    g_papplication = &application;
    application.addSymbols(symbols);

    // Create the output directories for orderbooks, log, and store if needed.
    // Note that actual log directories are created in the location specified
    // with -o arg each time the program starts
    // @TODO (tkaria@capitalkpartners.com) Maybe better to put
    // each log in the dated directory rather than have store and
    // log on same level as dated tick dirs
    if (config.root_output_dir.length() > 0) {
      fs::path argPath = fs::path(config.root_output_dir);
      if (!fs::exists(argPath)) {
        fs::create_directories(argPath);
      }

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

#ifdef LOG
    pan::log_DEBUG("pid: ", pan::integer(pid), " ppid: ", pan::integer(ppid));
#endif
    if (WritePidFile(argv[0], config.mic_string.c_str(), pid, ppid) != 0) {
#ifdef LOG
      pan::log_CRITICAL("Can't write pid file - exiting");
#endif
      return (-1);
    }

    // ZMQ initialization
    if (config.is_publishing) {
      zmq_bind(g_pub_socket, config.publishing_addr.c_str());
      application.setZMQContext(g_zmq_context);
      application.setZMQSocket(g_pub_socket);
    }


    if (config.is_logging) {
#ifdef LOG
      pan::log_DEBUG("Logging with FileStoreFactory");
#endif
      FIX::FileStoreFactory fileStoreFactory(config.fix_store_output_dir);
      FIX::FileLogFactory logFactory(config.fix_log_output_dir);
      g_pinitiator = new FIX::SocketInitiator(application,
                                              fileStoreFactory,
                                              settings,
                                              logFactory);
      assert(g_pinitiator);
    } else {
#ifdef LOG
      pan::log_DEBUG("Logging with NullStoreFactory");
#endif
      FIX::NullStoreFactory nullStoreFactory;
      g_pinitiator = new FIX::SocketInitiator(application,
                                              nullStoreFactory,
                                              settings);
      assert(g_pinitiator);
    }

#ifdef LOG
    pan::log_DEBUG("Starting initiator");
#endif
    g_pinitiator->start();

    char x;
    while (std::cin >> x) {
      if (x == 'q') {
        break;
      }
    }
#ifdef LOG
    pan::log_DEBUG("Stopping initiator");
#endif
    g_pinitiator->stop();
    return 0;
  } catch(FIX::Exception& e) {
    std::cerr << e.what();
    return 1;
  }
}


