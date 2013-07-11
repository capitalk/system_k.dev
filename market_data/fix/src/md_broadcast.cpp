
#include "md_broadcast.h"
#include "utils/types.h"
#include "proto/spot_fx_md_1.pb.h"
#ifdef LOG 
#include "utils/logging.h"
#endif // LOG

#include <iostream>

#include <zmq.hpp>

namespace capk {

void broadcast_bbo_book(void* bcast_socket,
                        const char* symbol,
                        const double best_bid,
                        const double best_ask,
                        const double bbsize,
                        const double basize,
                        const capk::venue_id_t venue_id) {
  zmq_msg_t msg;
  char msgbuf[MAX_MSG_SIZE];
  capkproto::instrument_bbo bbo;

  bbo.set_symbol(symbol);
  bbo.set_bid_price(best_bid);
  bbo.set_ask_price(best_ask);
  bbo.set_bid_size(bbsize);
  bbo.set_ask_size(basize);
  bbo.set_bid_venue_id(venue_id);
  bbo.set_ask_venue_id(venue_id);

  size_t msgsize = bbo.ByteSize();
  assert(msgsize < sizeof(msgbuf));
  if (msgsize > sizeof(msgbuf)) {
#ifdef LOG
    pan::log_CRITICAL("Buffer too small for protobuf serializaion");
#endif
    std::cerr << "CRITIAL: Buffer too small for protobuf serialization"
              << std::endl;
  }
  bbo.SerializeToArray(msgbuf, msgsize);

  zmq_msg_init_size(&msg, msgsize);
  memcpy(zmq_msg_data(&msg), msgbuf, msgsize);
#ifdef LOG
    pan::log_DEBUG("Sending ", pan::integer(msgsize), " bytes\n");
    pan::log_DEBUG("Protobuf string:\n", bbo.DebugString().c_str(), "\n");
#endif
  zmq_send(bcast_socket, &msg, 0);
}


} // namespace capk
