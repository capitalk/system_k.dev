#ifndef CAPK_CONFIG_SERVER
#define CAPK_CONFIG_SERVER

#include <fstream>

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include "proto/venue_configuration.pb.h"

#include "types.h" 
#include "constants.h" 
#include "constants.h" 
#include "venue_globals.h"
#include "config_server.h"

#include <zmq.hpp>

namespace pt = boost::property_tree;

namespace capk
{

  const char REQ_CONFIG = 'C';
  const char REQ_CONFIG_REFRESH = 'R';


  int readConfigServer(void* zmq_context,
                        const char* config_server_addr,
                        capkproto::configuration* cfg,
                        int request_timeout = -1);

  capkproto::venue_configuration get_venue_config(capkproto::configuration* config,
                                                  const char* mic_name);

  std::vector<std::string> readSymbolsFile(const std::string& symbol_file_name);

  void readVenueConfigFile(const std::string& config_filename,
                           capkproto::configuration* all_venue_config);

}


#endif // CAPK_CONFIG_SERVER
