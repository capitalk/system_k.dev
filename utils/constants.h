#ifndef CAPK_CONSTANTS
#define CAPK_CONSTANTS

#define MIC_LEN 9
#define SYMBOL_LEN 8
#define CURRENCY_SYMBOL_LEN 4
#define MAX_MICS 126
#define MAX_VENUES 126

#include <limits.h>

namespace capk {

const uint32_t INIT_BID  = INT_MIN;
const uint32_t INIT_ASK  = INT_MIN;
const uint32_t NO_BID = INIT_BID;
const uint32_t NO_ASK = INIT_ASK;
const double INIT_SIZE = -1;
const double NO_SIZE = 0;
}


#endif // CAPK_CONSTANTS
