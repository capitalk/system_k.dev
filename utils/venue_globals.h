#ifndef CAPK_VENUE_GLOBALS
#define CAPK_VENUE_GLOBALS

#include "types.h" 

namespace capk
{
    
    const int kCAPK_AGGREGATED_BOOK = 982132;
    const char* kCAPK_AGGREGATED_BOOK_MD_INTERFACE_ADDR = "tcp://127.0.0.1:9000";

    const char* const kFXCM_MIC_STRING = "FXCM";
    const char* const kFXCM_DEV_MIC_STRING = "FXCM.dev";
    const venue_id_t kFXCM_ID = 890778;
    const char* const kFXCM_ORDER_INTERFACE_ADDR = "tcp://127.0.0.1:9999";

    const char* const kXCDE_MIC_STRING = "XCDE";
    const char* const kXCDE_DEV_MIC_STRING = "XCDE.dev";
    const venue_id_t kXCDE_ID = 908239;
    const char* const kXCDE_ORDER_INTERFACE_ADDR = "tcp://127.0.0.1:9998";


}

#endif // CAPK_VENUE_GLOBALS
