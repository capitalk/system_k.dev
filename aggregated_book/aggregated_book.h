

// TODO(tkaria@capitalkpartners.com) 
// 1) ZMQ sleep fixes
// 2) BBO and depth fix
// 3) config file


#ifndef AGGREGATED_BOOK_H_
#define AGGREGATED_BOOK_H_

// For printing size_t across platforms where size_t differs
// @see http://stackoverflow.com/questions/174612/cross-platform-format-string-for-variables-of-type-size-t
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <boost/program_options.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <zmq.hpp>

#include <string>
#include <exception>

#include "utils/zhelpers.hpp"
#include "utils/time_utils.h"
#include "utils/bbo_book_types.h"
#include "utils/config_server.h"
#include "utils/venue_globals.h"
#include "utils/types.h"
#include "utils/constants.h"

#include "proto/spot_fx_md_1.pb.h"
#include "proto/venue_configuration.pb.h"

namespace po = boost::program_options;
namespace pt = boost::property_tree;

#define MAX_SYMBOLS 500
#define MSGBUF_SIZ 2048
#define UPDATE_TIMEOUT_MILLIS 5000


const char* CONFIG_SERVER_ADDR = "tcp://127.0.0.1:11111";
const std::string AGGREGATE_OB_ADDR = "inproc://ob";

/*
 * Unrolled loops if we like
 */
// for mic info
#define STRCPY5(dst, src) do { \
  dst[0] = src[0]; \
  dst[1] = src[1]; \
  dst[2] = src[2]; \
  dst[3] = src[3]; \
  dst[4] = src[4]; \
} while(0);

#define STRCMP5(dst, src) \
  (dst[0] == src[0] && \
  dst[1] == src[1] && \
  dst[2] == src[2] && \
  dst[3] == src[3] && \
  dst[4] == src[4]) 

// for pair name info
#define STRCPY8(dst, src) do { \
  dst[0] = src[0]; \
  dst[1] = src[1]; \
  dst[2] = src[2]; \
  dst[3] = src[3]; \
  dst[4] = src[4]; \
  dst[5] = src[5]; \
  dst[6] = src[6]; \
  dst[7] = src[7]; \
} while(0);

#define STRCMP8(dst, src) \
  (dst[0] == src[0] && \
  dst[1] == src[1] && \
  dst[2] == src[2] && \
  dst[3] == src[3] && \
  dst[4] == src[4] && \
  dst[5] == src[5] && \
  dst[6] == src[6] && \
  dst[7] == src[7]) 


capkproto::configuration all_venue_config;

//#define SYMBOL_COUNT (sizeof(symbols) / SYMBOL_LEN)

capk::venue_id_t venues[MAX_VENUES];

uint32_t VENUE_COUNT = 0;

zmq::socket_t* bcast_sock;

uint32_t seq_num = 0;

class OrderBookListener;
struct MarketDataReceiver {
  OrderBookListener* listener;
  boost::thread* thread;
};

struct instrument_info {
  char symbol[SYMBOL_LEN];
  zmq::socket_t* broadcast_socket;
  capk::SingleMarketBBO_t venues[MAX_VENUES];
};

instrument_info instruments[MAX_SYMBOLS];

void initialize_instrument_info(zmq::context_t* context,
                                const capkproto::configuration& conf,
                                const std::vector<std::string> symbols,
                                const uint32_t symbol_count);
void instrument_reset(capk::SingleMarketBBO_t& bbo);
bool instrument_is_unset(const capk::SingleMarketBBO_t& bbo);
bool isZeroTimespec(const struct timespec& ts);


class OrderBookAggregator {
public:
  OrderBookAggregator(zmq::context_t* context,
                      const std::string& bindAddr, 
                      const uint32_t symbol_count):
    _context(context),
    _bindAddr(bindAddr),
    _stopRequested(false),
    _symbol_count(symbol_count) {};
  void updateOrderBooks(const capk::InstrumentBBO_t& bbo);
  void broadcastBBO(zmq::socket_t* bcast_sock, 
                    const capk::InstrumentBBO_t& bbo);
 
  void run();
  void stop();
private:
  zmq::context_t* _context;
  std::string _bindAddr;
  zmq::socket_t* _bcast_sock;
  volatile bool _stopRequested;
  uint32_t _symbol_count;
};

class OrderBookListener {
public:
  OrderBookListener(zmq::context_t* context,
                    const std::string& market_data_addr,
                    const std::string& aggregator_addr):
      _context(context),
      _market_data_addr(market_data_addr),
      _aggregator_addr(aggregator_addr),
      _stopRequested(false) {};
  void run();
  void stop();
  ~OrderBookListener();

private:
  zmq::context_t* _context;
  std::string _market_data_addr;
  std::string _aggregator_addr;
  zmq::socket_t* _aggregator;
  volatile bool _stopRequested;

};


#endif  // AGGREGATED_BOOK_H_
