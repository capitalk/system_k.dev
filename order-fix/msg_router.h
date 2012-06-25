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
#include "msg_types.h"
#include "msg_cache.h"
#include "order_interface.h"
#include "proto/capk_globals.pb.h"
#include "proto/new_order_single.pb.h"

#include "quickfix/Application.h"

#include "msg_processor.h"

namespace capk {

class MsgProcessor;

// I hate having FIX objects exposed here - should have a class in between 
// the fix application and the router that implements sending/receiving of orders

class MsgRouter
{   
    public:
        Msgrouter(zmq::context_t* context, const std::string& inprocAddr, MsgProcessor* msgProcessor);
        ~Msgrouter();

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
		MsgCache _cache;
		MsgProcessor* _msgProcessor;
        
};

} // namespace capk

#endif // _KMSG_ROUTER_H_
