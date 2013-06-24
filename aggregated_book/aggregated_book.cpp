#include "aggregated_book.h"

void instrument_reset(capk::SingleMarketBBO_t* bbo)  {
  if (!bbo) {
    return;
  }
  bbo->bid_price = capk::INIT_BID;
  bbo->ask_price = capk::INIT_ASK;
  bbo->bid_size = capk::INIT_SIZE;
  bbo->ask_size = capk::INIT_SIZE;
}

bool instrument_is_unset(const capk::SingleMarketBBO_t& bbo) {
  return (bbo.bid_price == capk::INIT_BID &&
          bbo.ask_price == capk::INIT_ASK &&
          bbo.bid_size == capk::INIT_SIZE &&
          bbo.ask_size == capk::INIT_SIZE);
}

bool isZeroTimespec(const struct timespec& ts) {
  return (ts.tv_sec == 0 && ts.tv_nsec == 0);
}

void OrderBookAggregator::stop() {
  _stopRequested = true;
}

void OrderBookAggregator::broadcastBBO(zmq::socket_t* bcast_sock, 
                                       const capk::InstrumentBBO_t& bbo) {
  try {
      // Setup BBO and re-broadcast
      fprintf(stderr, "\nBroadcasting:\n<%s> BB: %d:%f(%f) BA: %d:%f(%f)\n\n",
              bbo.symbol,
              bbo.bid_venue_id,
              bbo.bid_price,
              bbo.bid_size,
              bbo.ask_venue_id,
              bbo.ask_price,
              bbo.ask_size);

      std::string bbo_msg;
      capkproto::instrument_bbo bbo_proto;
      bbo_proto.set_symbol(bbo.symbol);
      bbo_proto.set_bid_venue_id(bbo.bid_venue_id);
      bbo_proto.set_bid_price(bbo.bid_price);
      bbo_proto.set_bid_size(bbo.bid_size);
      bbo_proto.set_ask_venue_id(bbo.ask_venue_id);
      bbo_proto.set_ask_price(bbo.ask_price);
      bbo_proto.set_ask_size(bbo.ask_size);
      bbo_proto.set_sequence(seq_num++);

      int msgsize = bbo_proto.ByteSize();
      if (msgsize > MSGBUF_SIZ) {
        fprintf(stderr, "***\nbbo_proto msg too large for buffer - DROPPING MSG!\n***");
        return;
      } else {
        bbo_proto.SerializeToString(&bbo_msg);
#ifdef DEBUG
        fprintf(stderr, "Sending %d bytes\n", msgsize);
        fprintf(stderr, "Sending %s \n", bbo_proto.DebugString().c_str());
#endif
      }
      // Send header frame with symbol
      s_sendmore(*bcast_sock, bbo.symbol);
      // Send bbo data
      s_send(*bcast_sock, bbo_msg);
  } catch(const std::exception& e) {
    fprintf(stderr, "EXCEPTION: %s %d %s\n", __FILE__, __LINE__, e.what());
  }
}

void OrderBookAggregator::updateOrderBooks(const capk::InstrumentBBO_t& bbo) {
  bool found_symbol = false;
  bool found_venue = false;

  // Add the price to the correct orderbook
  // That is, receive an update for a SINGLE symbol on a SINGLE mic
  // so just blindly update it in the book
  for (uint32_t i = 0; i < _symbol_count; i++) {
    //if (strncmp(sym, instruments[i].symbol, SYMBOL_LEN) == 0) {
    if (STRCMP8(bbo.symbol, instruments[i].symbol) == 1) {
      found_symbol = true;
      for (uint32_t j = 0; j < VENUE_COUNT; j++) {
        // update the specific book that the price came from
        if (instruments[i].venues[j].venue_id != capk::NULL_VENUE_ID &&
            instruments[i].venues[j].venue_id == bbo.bid_venue_id &&
            instruments[i].venues[j].venue_id == bbo.ask_venue_id) {
          found_venue = true;
          instruments[i].venues[j].bid_price = bbo.bid_price;
          instruments[i].venues[j].ask_price = bbo.ask_price;
          instruments[i].venues[j].bid_size = bbo.bid_size;
          instruments[i].venues[j].ask_size = bbo.ask_size;
          clock_gettime(CLOCK_MONOTONIC,
                        &instruments[i].venues[j].last_update);
          break;
        }
      }
    }
  }

  if (!found_symbol) {
    fprintf(stderr,
            "No matching symbol in OrderBookAggregator for: %s\n", bbo.symbol);
    return;
  }
  if (!found_venue) {
    fprintf(stderr,
            "No matching venue in OrderBookAggregator for bid_venue_id: %d ask_venue_id %d\n",
            bbo.bid_venue_id,
            bbo.ask_venue_id);
    return;
  }

  // Now go through all the books and pick the bbo
  // check the time since last update and reset if too long
  timespec now;
  int64_t last_update_millis;

  capk::InstrumentBBO_t multi_market_bbo;
  InstrumentBBO_init(&multi_market_bbo);

  // find the symbol and venue id in the book structure
  for (uint32_t i = 0; i < _symbol_count; i++) {
    if (strncmp(bbo.symbol, instruments[i].symbol, SYMBOL_LEN) == 0) {
      found_symbol = true;

      for (uint32_t j = 0; j < VENUE_COUNT; j++) {
        if (instruments[i].venues[j].venue_id != capk::NULL_VENUE_ID) {
          found_venue = true;
          // check the elapsed time since last update
          clock_gettime(CLOCK_MONOTONIC, &now);
          last_update_millis =
            capk::timespec_delta_millis(instruments[i].venues[j].last_update, now);
          // was the item ever updated?
          /*
          if (isZeroTimespec(instruments[i].mics[j].last_update)) {
          fprintf(stderr, "<%s:%s>: NEVER UPDATED\n",
                      instruments[i].mics[j].MIC_name,
                      instruments[i].symbol);
          }
          */
          fprintf(stderr, "<%d:%s>: %f@%f (last update: %lu ms)\n",
                  instruments[i].venues[j].venue_id,
                  instruments[i].symbol,
                  instruments[i].venues[j].bid_price,
                  instruments[i].venues[j].ask_price,
                  last_update_millis);
          if (last_update_millis > UPDATE_TIMEOUT_MILLIS) {
#ifdef DEBUG
            fprintf(stderr, "*** TIMEOUT *** on %s (%d ms)",
                    instruments[i].symbol,
                    UPDATE_TIMEOUT_MILLIS);
#endif
            instrument_reset(&instruments[i].venues[j]);
          }
#ifdef DEBUG
          fprintf(stderr, "Last update: %ld:%ld\n",
                  instruments[i].venues[j].last_update.tv_sec,
                  instruments[i].venues[j].last_update.tv_nsec);
          fprintf(stderr, "Now        : %ld:%ld(ns)\n",
                  now.tv_sec,
                  now.tv_nsec);

          timespec tdelta =
            capk::timespec_delta(instruments[i].venues[j].last_update,
                                 now);
          fprintf(stderr, "Tdelta     : %ld:%ld(ns)\n",
                  tdelta.tv_sec,
                  tdelta.tv_nsec);
          fprintf(stderr, "<%s:%d>: ms since last update: %lu\n",
                  instruments[i].symbol,
                  instruments[i].venues[j].venue_id,
                  last_update_millis);
          fprintf(stderr, "<%d:%s>: %f@%f\n",
                  instruments[i].venues[j].venue_id,
                  instruments[i].symbol,
                  instruments[i].venues[j].bid_price,
                  instruments[i].venues[j].ask_price);
#endif
        }

        if (instruments[i].venues[j].bid_price > multi_market_bbo.bid_price) {
          multi_market_bbo.bid_price = instruments[i].venues[j].bid_price;
          multi_market_bbo.bid_venue_id = instruments[i].venues[j].venue_id;
          multi_market_bbo.bid_size = instruments[i].venues[j].bid_size;
        }
        if (instruments[i].venues[j].ask_price < multi_market_bbo.ask_price) {
          multi_market_bbo.ask_price = instruments[i].venues[j].ask_price;
          multi_market_bbo.ask_venue_id = instruments[i].venues[j].venue_id;
          multi_market_bbo.ask_size = instruments[i].venues[j].ask_size;
        }
      }
      broadcastBBO(_bcast_sock, multi_market_bbo);
    }
  }
}

void OrderBookAggregator::run() {
  try {
    //  Create the outbound broadcast socket
    _bcast_sock = new zmq::socket_t(*_context, ZMQ_PUB);
    assert(bcast_sock);

    zmq::socket_t receiver(*_context, ZMQ_SUB);
    const char* filter = "";
    receiver.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
    receiver.bind(_bindAddr.c_str());
    printf("OrderBookAggregator bind address: %s\n",
            _bindAddr.c_str());

    capkproto::instrument_bbo bbo_proto;
    while (1 && _stopRequested == false) {
      // Extract the message from protobufs
      zmq::message_t msg;
      receiver.recv(&msg);
#ifdef DEBUG
      boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time());
#endif
      capk::InstrumentBBO_t bbo;
      bbo_proto.ParseFromArray(msg.data(), msg.size());
      bbo.bid_venue_id = bbo_proto.bid_venue_id();
      bbo.ask_venue_id = bbo_proto.ask_venue_id();
#ifdef DEBUG
      printf("OrderBookAggregator received: %" PRIuPTR" bytes\n",
              msg.size());
      printf("OrderBookAggregator received protobuf:\n%s\n",
             bbo_proto.DebugString().c_str());
#endif
      STRCPY8(bbo.symbol, bbo_proto.symbol());
      bbo.bid_size = bbo_proto.bid_size();
      bbo.ask_size = bbo_proto.ask_size();
      bbo.bid_price = bbo_proto.bid_price();
      bbo.ask_price = bbo_proto.ask_price();
      updateOrderBooks(bbo);
#ifdef DEBUG
      boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time());
      boost::posix_time::time_duration duration(time_end - time_start);
      fprintf(stderr, "BookUpdate Time(us) to recv and parse: %s\n",
              to_simple_string(duration).c_str());
#endif
    }
  } catch(const std::exception& e) {
    fprintf(stderr, "EXCEPTION: %s %d %s\n", __FILE__, __LINE__, e.what());
  }
}

OrderBookListener::~OrderBookListener() {
  if (_aggregator) {
    delete _aggregator;
  }
}

void
OrderBookListener::stop() {
  _stopRequested = true;
}

void
OrderBookListener::run() {
  try {
    assert(_context != NULL);
    assert(_market_data_addr.c_str() != NULL);

    int zero = 0;
    bcast_sock->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
    printf("Attempting to bind to: %s ... ",
           capk::CAPK_AGGREGATED_BOOK_MD_INTERFACE_ADDR);
    bcast_sock->bind(capk::CAPK_AGGREGATED_BOOK_MD_INTERFACE_ADDR);
    printf(" complete.\n");


    fprintf(stdout, "OrderBookListener connecting to: %s\n",
            _market_data_addr.c_str());
    zmq::socket_t subscriber(*_context, ZMQ_SUB);
    subscriber.connect(_market_data_addr.c_str());
    const char* filter = "";
    subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

    _aggregator = new zmq::socket_t(*_context, ZMQ_PUB);
    assert(_aggregator);
    _aggregator->connect(_aggregator_addr.c_str());

    zmq::message_t msg;
    while (1 && _stopRequested == false) {
      subscriber.recv(&msg);
#ifdef DEBUG
      boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time());
#endif

      int64_t more;
      size_t more_size;
      more_size = sizeof(more);
      subscriber.getsockopt(ZMQ_RCVMORE, &more, &more_size);
      // send to orderbook on inproc socket - no locks needed according to zmq
      _aggregator->send(msg, more ? ZMQ_SNDMORE : 0);

#ifdef DEBUG
      boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time());
      boost::posix_time::time_duration duration(time_end - time_start);
      fprintf(stderr, "Inbound from md interface time to receive and parse: %s\n",
              to_simple_string(duration).c_str());
#endif
    }
  } catch(const std::exception& e) {
    fprintf(stderr, "EXCEPTION: %s %d %s\n", __FILE__, __LINE__, e.what());
  }
}


void initialize_instrument_info(zmq::context_t* context,
                                const capkproto::configuration& conf, 
                                const std::vector<std::string> symbols,
                                const uint32_t symbol_count) {
  assert(context);
  assert(symbol_count > 0);
  assert(VENUE_COUNT > 0);
  printf("Initializing instruments: %ld bytes for %u symbols\n",
         sizeof(instruments), symbol_count);
  for (uint32_t i = 0; i < symbol_count; i++) {
    printf("Initializing symbol: %d to %s\n", i, symbols[i].c_str());
    STRCPY8(instruments[i].symbol, symbols[i].c_str());
    instruments[i].broadcast_socket = new zmq::socket_t(*context, ZMQ_PUB);
    assert(instruments[i].broadcast_socket);

    int num_venues = conf.configs_size();
    printf("Initializing: %d venues\n", num_venues);
    for (int j = 0; j < num_venues; j++) {
      const capkproto::venue_configuration vc = conf.configs(j);
      printf("Initializing venue: %d to %d\n", i, vc.venue_id());
      instruments[i].venues[j].venue_id = vc.venue_id();
      instruments[i].venues[j].last_update = {0};
      instruments[i].venues[j].bid_price = capk::INIT_BID;
      instruments[i].venues[j].ask_price = capk::INIT_ASK;
      instruments[i].venues[j].bid_size = -1;
      instruments[i].venues[j].ask_size = -1;
    }
  }

#ifdef DEBUG
  for (uint32_t i = 0; i < symbol_count; i++) {
    for (uint32_t j = 0; j < VENUE_COUNT; j++) {
      fprintf(stderr, "%s <%d>:%f@%f\n",
              instruments[i].symbol,
              instruments[i].venues[j].venue_id,
              instruments[i].venues[j].bid_price,
              instruments[i].venues[j].ask_price);
    }
  }
#endif
}

int main(int argc, char** argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  // ZMQ context setup - ONLY ONE!
  zmq::context_t context(1);

  // make sure counts are less that allocated space
  assert(VENUE_COUNT < MAX_VENUES);

  std::vector<std::string> sym_vector;
  uint32_t num_symbols = 0;
  // TODO(tkaria@capitalkpartners.com) - use config file  and symbol file
  // tkaria@capitalkpartners.com - completed config and symbol
 
  // check program options
  try {
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("c", po::value<std::string>(), "<config file>")
    ("s", po::value<std::string>(), "<symbol file>")
    ("d", "debug info");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      std::cerr << desc << std::endl;
      return -1;
    }

    if (vm.count("s")) {
      std::string symbol_file = vm["s"].as<std::string>();
      printf("Using symbol file: %s\n", symbol_file.c_str());
      sym_vector = capk::readSymbolsFile(symbol_file);
      num_symbols = sym_vector.size();
      assert(num_symbols < MAX_SYMBOLS);
      if (num_symbols == 0) {
        return -1;
      }
      printf("%s contains %d symbols", symbol_file.c_str(), num_symbols);
    } else {
      std::cerr << desc << std::endl;
      fprintf(stderr, "Symbol file was not set - exiting\n");
      return -1;
    }


    if (vm.count("c")) {
      std::string config_file = vm["c"].as<std::string>();
      printf("Using config file: %s\n", config_file.c_str());
      capk::readVenueConfigFile(config_file, &all_venue_config);
    } else {
      fprintf(stderr, "Config file was not set - using config server\n");
      // Request config params from configuration server
      if (capk::readConfigServer(context,
                              CONFIG_SERVER_ADDR,
                              &all_venue_config, 
                              500000) != 0) {
        fprintf(stderr, "Cannot reach config server at: %s\n", CONFIG_SERVER_ADDR);
        return -1;
      }
    }

    //  Program options and init are OK - now start sockets and threads
    //  Connect to inproc socket for orderbook
    OrderBookAggregator ob(&context, AGGREGATE_OB_ADDR, num_symbols);
    boost::thread* t0 = new boost::thread(boost::bind(&OrderBookAggregator::run, &ob));
    //sleep(2);
    

    // subscribers
    // subscribe to all available venues as exist in the config server
    uint32_t num_venues = all_venue_config.configs_size();
    VENUE_COUNT = num_venues;
    initialize_instrument_info(&context, all_venue_config, sym_vector, num_symbols);
    printf("Number of configured venues (not necessarily broadcasting): %d\n",
           num_venues);
    MarketDataReceiver* receivers = new MarketDataReceiver[num_venues];
    boost::thread_group thread_group;
    for (uint32_t i = 0; i< num_venues; i++) {
      const capkproto::venue_configuration vc = all_venue_config.configs(i);
      printf("Creating listener for: %s\n", vc.mic_name().c_str());
      receivers[i].listener =
          new OrderBookListener(&context,
                                vc.market_data_broadcast_addr(),
                                AGGREGATE_OB_ADDR);
      receivers[i].thread =
          new boost::thread(boost::bind(&OrderBookListener::run,
                                        receivers[i].listener));
      thread_group.add_thread(receivers[i].thread);
    }
    thread_group.join_all();
    t0->join();
    assert(thread_group.size() == num_venues);
    google::protobuf::ShutdownProtobufLibrary();
  }
  catch(const std::exception& e) {
    fprintf(stderr, "EXCEPTION: %s %d %s\n", __FILE__, __LINE__, e.what());
    return (-1);
  }
}
