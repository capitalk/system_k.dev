#ifndef CAPK_VENUE_UTILS
#define CAPK_VENUE_UTILS

#include "types.h" 
#include "constants.h" 
#include "venue_globals.h"
#include "config_server.h"
#include <zmq.h>
#include <zmq.hpp>

#include "proto/venue_configuration.pb.h"

namespace capk
{



    int get_config_params(void* zmq_context, 
            const char* config_server_addr, 
            capkproto::configuration* cfg);

    capkproto::venue_configuration get_venue_config(capkproto::configuration* config, 
            const char* mic_name);
    
}

#endif // CAPK_VENUE_GLOBALS
