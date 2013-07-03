#ifndef CAPK_CONSTANTS
#define CAPK_CONSTANTS

#define MIC_LEN 9
#define SYMBOL_LEN 8
#define CURRENCY_SYMBOL_LEN 4
#define MAX_MICS 126
#define MAX_VENUES 126

#include <limits.h>

namespace capk {

const int32_t INIT_BID  = INT_MIN;
const int32_t INIT_ASK  = INT_MAX;
const int32_t NO_BID = INIT_BID;
const int32_t NO_ASK = INIT_ASK;
const double INIT_SIZE = -1;
const double NO_SIZE = 0;

const uint32_t ZMQ_MAX_MSG_HOPS = 4;
const uint32_t ZMQ_ADDR_LEN = 17;
const uint32_t UUID_LEN = 16;
const uint32_t UUID_STRLEN  = 36;
}


#endif // CAPK_CONSTANTS
