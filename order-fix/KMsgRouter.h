#ifndef _KMSG_ROUTER_H_
#define _KMSG_ROUTER_H_

#include <iostream>
#include <string>
#include <exception>
#include <sstream>

#include <stdio.h>

#include <boost/program_options.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>

#include <zmq.hpp>
#include <czmq.h>
#include "utils/zhelpers.hpp"

#include "logging.h"
#include "KMsgTypes.h"
#include "KOrderManager.h"
#include "proto/new_order_single.pb.h"
#include "proto/capk_globals.pb.h"

class KMsgRouter
{   
    public:
        KMsgRouter(zmq::context_t* context, const std::string& inprocAddr);
        void run();
        void stop();
        ~KMsgRouter();
 
    private:
        zmq::context_t* _context;
        std::string _connectAddr;
        std::string _inprocAddr;
        zmq::socket_t* _inproc;
        volatile bool _stopRequested;
        
};

#endif // _KMSG_ROUTER_H_
