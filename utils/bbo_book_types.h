#ifndef CAPK_BBO
#define CAPK_BBO

#include <time.h>
#include "types.h"
#include "constants.h"


namespace capk {

struct IBSingleMarketBBO
{
    //char MIC_name[MIC_LEN];
	char symbol[SYMBOL_LEN];
    char currency[CURRENCY_SYMBOL_LEN];
    double bid_price;
    double ask_price;
    double bid_size;
    double ask_size;
    timespec last_update; 
    double last_price;
    double last_size; 
};

typedef struct IBSingleMarketBBO IBSingleMarketBBO_t;


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

    venue_id_t bid_venue_id;
    //std::string bid_venue;
    double bid_price;
    double bid_size;
    timespec bid_last_update;

    capk::venue_id_t ask_venue_id;
    //std::string ask_venue;
    double ask_price;
    double ask_size;
    timespec ask_last_update;
};

typedef struct MultiMarketBBO MultiMarketBBO_t;



} // namespace capk

#endif // CAPK_BBO
