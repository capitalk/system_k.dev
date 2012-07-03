#ifndef CAPK_BBO
#define CAPK_BBO

#include <time.h>
#include "types.h"
#include "constants.h"


namespace capk {

struct SingleMarketBBO
{
    //char MIC_name[MIC_LEN];
    venue_id_t venue_id;
	char symbol[SYMBOL_LEN];
    double bid_price;
    double ask_price;
    double bid_size;
    double ask_size;
    timespec last_update; 
    
};

typedef struct SingleMarketBBO SingleMarketBBO_t;


struct MultiMarketBBO
{
    char symbol[SYMBOL_LEN];

    venue_id_t bid_venue;
    //std::string bid_venue;
    double bid_price;
    double bid_size;
    timespec bid_last_update;

    capk::venue_id_t ask_venue;
    //std::string ask_venue;
    double ask_price;
    double ask_size;
    timespec ask_last_update;
};

typedef struct MultiMarketBBO MultiMarketBBO_t;



} // namespace capk

#endif // CAPK_BBO
