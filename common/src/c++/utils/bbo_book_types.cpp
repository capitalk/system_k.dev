#include "bbo_book_types.h"
#include "venue_globals.h"

namespace capk {

void
InstrumentBBO_init(InstrumentBBO_t* i) 
{
    assert(i);
    if (!i) {
        return;
    }
    memset(i->symbol, 0, SYMBOL_LEN);
    
    i->bid_venue_id = NULL_VENUE_ID;
    i->bid_price = NO_BID;
    i->bid_size = INIT_SIZE;
    memset(&(i->bid_last_update), 0, sizeof(timespec));

    i->ask_venue_id = NULL_VENUE_ID;
    i->ask_price = NO_ASK;
    i->ask_size = INIT_SIZE;
    memset(&(i->ask_last_update), 0, sizeof(timespec));
}

} // namespace capk

