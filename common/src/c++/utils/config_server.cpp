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

#include "config_server.h"

namespace capk 
{

  capkproto::venue_configuration
  get_venue_config(capkproto::configuration* config,
                   const char* mic_name) 
  {
      if (config != NULL) {
          for (int i = 0; i< config->configs_size(); i++) {
              const capkproto::venue_configuration vc  = config->configs(i);
              if (vc.mic_name() == mic_name) {
                  //fprintf(stderr, "venue_configuration: %s", vc.DebugString().c_str());
                  //fprintf(stderr, "venue_id: %d", vc.venue_id());
                  return vc;
              }
          }
      }
      return capkproto::venue_configuration();
  } 

  bool isQuote(const char c) {
    return c == '\"';
  }

  void readVenueConfigFile(const std::string& config_filename,
                           capkproto::configuration* all_venue_config) {
    if (!all_venue_config) {
      return;
    }
    if (all_venue_config == NULL) {
      return;
    }
    std::string name;
    std::string value;
    pt::ptree root;
    pt::ini_parser::read_ini(config_filename, root);

    pt::ptree global_tree = root.get_child("GLOBAL");
    pt::ptree::const_assoc_iterator it;

    it = global_tree.find("trade_serialization_addr");
    if (it != global_tree.not_found()) {
      all_venue_config->set_trade_serialization_addr(boost::lexical_cast<std::string>(it->second.data()));
    }
    
    it = global_tree.find("recovery_listener_addr");
    if (it != global_tree.not_found()) {
      all_venue_config->set_recovery_listener_addr(boost::lexical_cast<std::string>(it->second.data()));
    }

    it = global_tree.find("aggregated_bbo_book_addr");
    if (it != global_tree.not_found()) {
      all_venue_config->set_aggregated_bbo_book_addr(boost::lexical_cast<std::string>(it->second.data()));
    }
    
    it = global_tree.find("aggregated_bbo_book_id");
    if (it != global_tree.not_found()) {
      all_venue_config->set_aggregated_bbo_book_id(boost::lexical_cast<int32_t>(it->second.data()));
    }

    pt::ptree subtree = root.get_child("VENUES");
    BOOST_FOREACH(pt::ptree::value_type &v, subtree) {
      capkproto::venue_configuration* single_venue_config = all_venue_config->add_configs();

      name = v.first;
      // Remove quotes from boost returned find value
      value = v.second.get<std::string>("");
      value.erase(std::remove_if(value.begin(), value.end(), &isQuote), value.end());
    
      //std::cout << "name: " << name << " value: " << value << std::endl;
      // Get the subtree for the value 
      pt::ptree venue_config_tree = root.get_child(value);
    
      it = venue_config_tree.find("venue_id");
      if (it != venue_config_tree.not_found()) {
        single_venue_config->set_venue_id(boost::lexical_cast<int32_t>(it->second.data()));
      }
      it = venue_config_tree.find("mic_name");
      if (it != venue_config_tree.not_found()) {
        single_venue_config->set_mic_name(boost::lexical_cast<std::string>(it->second.data()));
      }
      it = venue_config_tree.find("order_interface_addr");
      if (it != venue_config_tree.not_found()) {
        single_venue_config->set_order_interface_addr(boost::lexical_cast<std::string>(it->second.data()));
      }
      it = venue_config_tree.find("order_ping_addr");
      if (it != venue_config_tree.not_found()) {
        single_venue_config->set_order_ping_addr(boost::lexical_cast<std::string>(it->second.data()));
      }
      it = venue_config_tree.find("market_data_broadcast_addr");
      if (it != venue_config_tree.not_found()) {
        single_venue_config->set_market_data_broadcast_addr(boost::lexical_cast<std::string>(it->second.data()));
      }
      it = venue_config_tree.find("logging_broadcast_addr");
      if (it != venue_config_tree.not_found()) {
        single_venue_config->set_logging_broadcast_addr(boost::lexical_cast<std::string>(it->second.data()));
      }
      it = venue_config_tree.find("use_synthetic_cancel_replace");
      if (it != venue_config_tree.not_found()) {
        single_venue_config->set_use_synthetic_cancel_replace(boost::lexical_cast<bool>(it->second.data()));
      }
    }
    std::cout << all_venue_config->DebugString() << std::endl; 
  }

int 
readConfigServer(void* zmq_context, 
        const char* config_server_addr, 
        capkproto::configuration* cfg, 
        int request_timeout)
{
#if 0
  int rc;
  zmq_msg_t request_frame;
  int linger = 0;

  void* context = zmq_init(1);
  void* config_socket = zmq_socket(context, ZMQ_REQ);
  zmq_setsockopt(config_socket, ZMQ_LINGER, &linger, sizeof(linger));
  rc = zmq_connect(config_socket, config_server_addr);

  rc = zmq_msg_init_data(&request_frame,
                         (void*)&capk::REQ_CONFIG,
                         sizeof(capk::REQ_CONFIG), NULL, NULL);
 
  rc = zmq_send(config_socket, &request_frame, ZMQ_NOBLOCK);
  std::cerr << "zmq_send returned: " << rc << std::endl;
  // inbound messages
  zmq_msg_t in_reply_to_frame;
  zmq_msg_init(&in_reply_to_frame);
  zmq_msg_t config_frame;
  zmq_msg_init(&config_frame);

  int64_t more = 0;
  size_t more_size = sizeof(more);
  uint32_t retry_count = 3;
  while (1) {
      zmq_pollitem_t items [] = { { config_socket,  0, ZMQ_POLLIN, 0 } };
      rc = zmq_poll (items, 1, request_timeout);
      assert(rc == 0);
      if (items[0].revents & ZMQ_POLLIN) {
        do {
          rc = zmq_recv(config_socket, &in_reply_to_frame, 0);
          rc = zmq_recv(config_socket, &config_frame, 0);
          zmq_getsockopt(config_socket, ZMQ_RCVMORE, &more, &more_size);
      } while (more);
      break;
    }
    else {
      retry_count--;
    }
    if (retry_count == 0) {
      zmq_close(config_socket);
      zmq_term(context);
      return -1;
    }
  }
#endif
  int zero = 0;
  int rc; 
  assert(zmq_context != NULL);
  void* config_socket = zmq_socket(zmq_context, ZMQ_REQ);
  assert(config_socket);
  zmq_setsockopt(config_socket, ZMQ_LINGER, &zero, sizeof(zero));
  rc = zmq_connect(config_socket, config_server_addr);
  assert(rc == 0);
  if (rc != 0) {
      zmq_close(config_socket);
      return -1;
  }
  zmq_msg_t request_frame;

  rc = zmq_msg_init_data(&request_frame,
                         (void*)&capk::REQ_CONFIG,
                         sizeof(capk::REQ_CONFIG), NULL, NULL);
  assert(rc == 0);
  if (rc != 0) {
      zmq_close(config_socket);
      return -1;
  }
  //memcpy(zmq_msg_data(&request_frame), (void*)&capk::REQ_CONFIG, sizeof(capk::REQ_CONFIG));
  rc = zmq_send(config_socket, &request_frame, 0);
  assert(rc == 0);
  zmq_msg_close(&request_frame);
  if (rc != 0) {
      zmq_close(config_socket);
      return -1;
  }

  // inbound messages
  zmq_msg_t in_reply_to_frame;
  zmq_msg_init(&in_reply_to_frame);
  zmq_msg_t config_frame;
  zmq_msg_init(&config_frame);

  int64_t more = 0;
  size_t more_size = sizeof(more);
  uint32_t retry_count = 3;
  while (1) {
      zmq_pollitem_t items [] = { { config_socket,  0, ZMQ_POLLIN, 0 } };
      int rc = zmq_poll (items, 1, request_timeout);
      if (rc == -1) {
        return -1;
      } 
      if (items[0].revents & ZMQ_POLLIN) {
        do {
          rc = zmq_recv(config_socket, &in_reply_to_frame, 0);
          assert(rc == 0);
          if (rc != 0) {
            zmq_close(config_socket);
            return -1;
          }

          rc = zmq_recv(config_socket, &config_frame, 0);
          assert(rc == 0);
          if (rc != 0) {
              zmq_close(config_socket);
              return -1;
          }
          zmq_getsockopt(config_socket, ZMQ_RCVMORE, &more, &more_size);
      } while (more);
      break;
    }
    else {
      retry_count--;
    }
    if (retry_count == 0) {
      zmq_close(config_socket);
      return -1;
    }
  }
/*
      do {
          rc = zmq_recv(config_socket, &in_reply_to_frame, 0);
          assert(rc == 0);
          if (rc != 0) {
              zmq_close(config_socket);
              return -1;
          }

          rc = zmq_recv(config_socket, &config_frame, 0);
          assert(rc == 0);
          if (rc != 0) {
              zmq_close(config_socket);
              return -1;
          }
          zmq_getsockopt(config_socket, ZMQ_RCVMORE, &more, &more_size);
      } while (more);
*/
  bool parseOK = cfg->ParseFromArray(zmq_msg_data(&config_frame), zmq_msg_size(&config_frame));
  zmq_close(config_socket);
  if (!parseOK) {
      return -1;
  }
  else {
      return 0;
  }
  return -1;
}
  
/**
 * Read symbol config file - one symbol per line
 * @param string filename of symbol config file
 * @return std::vector of strings that contain symbol names
 */
std::vector<std::string> readSymbolsFile(const std::string& symbol_file_name) {
  std::vector<std::string> symbols;
  if (symbol_file_name == "") {
    return symbols;
  }
  std::string line;
  std::ifstream symbols_stream(symbol_file_name);
  if (symbols_stream.is_open()) {
    while (symbols_stream.good()) {
      std::getline(symbols_stream, line);
      boost::algorithm::trim(line);
      symbols.push_back(line);
    }
  }
  return symbols;
}

} // namespae capk
