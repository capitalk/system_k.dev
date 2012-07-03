#ifndef CAPK_VENUE_GLOBALS
#define CAPK_VENUE_GLOBALS

#include "types.h" 
#include "constants.h" 

namespace capk
{
    // Invalid venue id indicating error
    const venue_id_t kNULL_VENUE_ID = 0;
    const char* const kNULL_MIC_STRING = "NVLL";
    
    // Aggregated book 
    const int kCAPK_AGGREGATED_BOOK = 982132;
    const char* const kCAPK_AGGREGATED_BOOK_MD_INTERFACE_ADDR = "tcp://127.0.0.1:9000";

    // CAPK test venue info
    const venue_id_t kCAPK_VENUE_ID = 12345;
    const char* const kCAPK_ORDER_INTERFACE_ADDR = "tcp://127.0.0.1:2001";

    // FXCM MIC and venue info
    const char* const kFXCM_MIC_STRING = "FXCM";
    const char* const kFXCM_DEV_MIC_STRING = "FXCM.dev";
    const venue_id_t kFXCM_VENUE_ID = 890778;
    const char* const kFXCM_ORDER_INTERFACE_ADDR = "tcp://127.0.0.1:9999";
    const char* const kFXCM_BROADCAST_ADDR = "tcp://127.0.0.1:5273";

    // XCDE (Baxter)  MIC and venue info
    const char* const kXCDE_MIC_STRING = "XCDE";
    const char* const kXCDE_DEV_MIC_STRING = "XCDE.dev";
    const venue_id_t kXCDE_VENUE_ID = 908239;
    const char* const kXCDE_ORDER_INTERFACE_ADDR = "tcp://127.0.0.1:9998";
    const char* const kXCDE_BROADCAST_ADDR = "tcp://127.0.0.1:5271";

    // GAIN MIC and venue info
    const char* const kGAIN_BROADCAST_ADDR = "tcp://127.0.0.1:5272";

    // Aggregated book broadcast information
    const char* const kAGGREGATED_BOOK_BROADCAST_ADDR = "tcp://127.0.0.1:9000";

}

#endif // CAPK_VENUE_GLOBALS
