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
#include "KMsgCache.h"
#include "OrderInterface.h"
#include "proto/capk_globals.pb.h"
#include "proto/new_order_single.pb.h"

#include "quickfix/Application.h"

#include "KMsgProcessor.h"


class KMsgProcessor;

// I hate having FIX objects exposed here - should have a class in between 
// the fix application and the router that implements sending/receiving of orders

class KMsgRouter
{   
    public:
        KMsgRouter(zmq::context_t* context, const std::string& inprocAddr, KMsgProcessor* msgProcessor);
        ~KMsgRouter();

        void run();
        void stop();
		void rcvMsg();
		void repMsg(order_id_t& oid);	
		void setOrderInterface(capk::OrderInterface& interface);
		capk::OrderInterface* getOrderInterface();
		KMsgCache* getCache();
 
    private:
        zmq::context_t* _context;
        std::string _connectAddr;
        std::string _inprocAddr;
        zmq::socket_t* _inproc;
        volatile bool _stopRequested;

		capk::OrderInterface* _interface;
		KMsgCache _cache;
		KMsgProcessor* _msgProcessor;
        
};

#endif // _KMSG_ROUTER_H_
