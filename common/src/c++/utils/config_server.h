//
// System K - A framework for building trading applications and analysis
// Copyright (C) 2013 Timir Karia <tkaria@capitalkpartners.com>
//
// This file is part of System K.
//
// System K is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// System K is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with System K.  If not, see <http://www.gnu.org/licenses/>.
//

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
