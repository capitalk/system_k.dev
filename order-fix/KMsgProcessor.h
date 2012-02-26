
#ifndef _K_MSG_PROCESSOR_
#define _K_MSG_PROCESSOR_

#include <uuid/uuid.h>
#include <utils/zhelpers.hpp>
#include <zmq.hpp>
#include <czmq.h>

#include <string>
#include <memory.h>

#include "logging.h"
#include "timing.h"
#include "KMsgHandler.h"
#include "KMsgRouter.h"

#include <boost/thread.hpp>

class KMsgProcessor
{
	public:
		KMsgProcessor(zmq::context_t* ctx, 
					const char* listen_addr, 
					const char* inproc_addr, 
					const short int num_threads,
					KMsgHandler* handler);
		~KMsgProcessor();

		int run();

	private:
		// initializer list
		zmq::context_t* _ctx;
		std::string _listen_addr;
		std::string _inproc_addr;
		short int _num_threads;

		// sockets
		void *_server;
		void *_inproc;


};




#endif // _K_MSG_PROCESSOR_
