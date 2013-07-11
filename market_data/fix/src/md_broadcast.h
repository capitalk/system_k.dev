#ifndef COLLECT_FIX_BROADCAST_H_
#define COLLECT_FIX_BROADCAST_H_

#include "utils/types.h"

namespace capk {

const int MAX_MSG_SIZE = 256;

void broadcast_bbo_book(void* bcast_socket,
                        const char* symbol,
                        const double best_bid,
                        const double best_ask,
                        const double bbsize,
                        const double basize,
                        const capk::venue_id_t venue_id);

} // namespace capk
#endif // COLLECT_FIX_BROADCAST_H_


