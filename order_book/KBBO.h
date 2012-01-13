#ifndef CAPK_BBO
#define CAPK_BBO

#include <time.h>
#include "KTypes.h"

#define MIC_LEN 5

struct KBBO
{
    char MIC_name[MIC_LEN];
    double bid_price;
    double ask_price;
    double bid_size;
    double ask_size;
    timespec last_update; 
    
};

#define MAX_MICS 126


#endif // CAPK_BBO
