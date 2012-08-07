#ifndef CAPK_VENUE_GLOBALS
#define CAPK_VENUE_GLOBALS

#include "types.h" 
#include "constants.h" 

/*
 * NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
 *
 * YOU MUST MAKE SURE THAT VALUES IN CONFIG FILES (.cfg) FOR THE ORDER
 * AND MARKET DATA INTERFACES MATCH THE SETTINGS IN THIS FILE!
 */


namespace capk
{
    // Invalid venue id indicating error
    const venue_id_t kNULL_VENUE_ID = 0;
    const char* const kNULL_MIC_STRING = "NVLL";
    
    // Aggregated book  - uses CAPK id
    const char* const kCAPK_AGGREGATED_BOOK_MD_INTERFACE_ADDR = "tcp://127.0.0.1:10000";
    // CAPK test venue info
    const venue_id_t kCAPK_VENUE_ID = 12345;
    const char* const kCAPK_ORDER_INTERFACE_ADDR = "tcp://127.0.0.1:9001";
    const char* const kCAPK_ORDER_PING_ADDR = "tcp://127.0.0.1:7001";

    // FXCM MIC and venue info
    const char* const kFXCM_MIC_STRING = "FXCM";
    const char* const kFXCM_DEV_MIC_STRING = "FXCM.dev";
    const venue_id_t kFXCM_VENUE_ID = 890778;
    // N.B. Must match values in ini file used to start interface!
    const char* const kFXCM_ORDER_INTERFACE_ADDR = "tcp://127.0.0.1:9999";
    const char* const kFXCM_ORDER_PING_ADDR = "tcp://127.0.0.1:7999";
    const char* const kFXCM_BROADCAST_ADDR = "tcp://127.0.0.1:5999";

    // XCDE (Baxter)  MIC and venue info
    const char* const kXCDE_MIC_STRING = "XCDE";
    const char* const kXCDE_DEV_MIC_STRING = "XCDE.dev";
    const venue_id_t kXCDE_VENUE_ID = 908239;
    const char* const kXCDE_ORDER_INTERFACE_ADDR = "tcp://127.0.0.1:9998";
    const char* const kXCDE_ORDER_PING_ADDR = "tcp://127.0.0.1:7998";
    const char* const kXCDE_BROADCAST_ADDR = "tcp://127.0.0.1:5998";

    // FastMatch  MIC and venue info
    const char* const kFAST_MIC_STRING = "FAST";
    const char* const kFAST_DEV_MIC_STRING = "FAST.dev";
    const venue_id_t kFAST_VENUE_ID = 327878;
    const char* const kFAST_ORDER_INTERFACE_ADDR = "tcp://127.0.0.1:9997";
    const char* const kFAST_ORDER_PING_ADDR = "tcp://127.0.0.1:7997";
    const char* const kFAST_BROADCAST_ADDR = "tcp://127.0.0.1:5997";



    // GAIN MIC and venue info
    const char* const kGAIN_BROADCAST_ADDR = "tcp://127.0.0.1:5996";

    // IB MIC and venue info
    const char* const kIBRK_BROADCAST_ADDR = "tcp://127.0.0.1:5995";
    const char* const kIBRK_MIC_STRING = "IBRK";
    const char* const kIBRK_DEV_MIC_STRING = "IBRK.dev";
    const venue_id_t kIBRK_VENUE_ID = 342234;


    // Trade serialization service address
    //const char* const kTRADE_SERIALIZATION_ADDR = "tcp://127.0.0.1:9898";
    const char* const kTRADE_SERIALIZATION_ADDR = "ipc:///tmp/trade_serializer";
    const char* const kRECOVERY_LISTENER_ADDR = "ipc:///tmp/recovery";

}

#endif // CAPK_VENUE_GLOBALS
