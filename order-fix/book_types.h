#ifndef __CAPK_BOOK_TYPES__
#define __CAPK_BOOK_TYPES__

#include "msg_types.h"
#include "order_book/KBBO.h"

namespace capk {

struct BBO2
{
    char symbol[SYMBOL_LEN];

    //capk::venue_id_t bid_venue;
    std::string bid_venue;
    double bid_price;
    double bid_size;
    timespec bid_last_update;

    //capk::venue_id_t ask_venue;
    std::string ask_venue;
    double ask_price;
    double ask_size;
    timespec ask_last_update;
};

typedef struct BBO2 BBO2_t;

} // namespace capk
#endif //__CAPK_BOOK_TYPES__
