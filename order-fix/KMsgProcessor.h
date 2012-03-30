
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
#include "NullOrderInterface.h"
#include "KMsgCache.h"

#include <boost/thread.hpp>



class KMsgProcessor
{
	public:
		KMsgProcessor(zmq::context_t* ctx, 
					const char* listen_addr, 
					const char* in_addr, 
					const short int in_threads,
					const char* out_addr, 
					const short int out_threads,
					KMsgHandler* handler, 
					capk::OrderInterface* oi);
		~KMsgProcessor();

		int run();

		void snd();
		void rcv();
	
		void setOrderInterface(capk::OrderInterface& interface) {
			this->_interface = &interface;
		}
		capk::OrderInterface* getOrderInterface() {
			return _interface;
		}

		inline const std::string& getInboundAddr() const {
			return _in_addr;
		}

		inline const std::string& getOutboundAddr() const {
			return _out_addr;
		}
		
		inline unsigned short int getInThreadCount() const {
			return _in_threads;
		}

		inline unsigned short int getOutThreadCount() const {
			return _out_threads;
		}
		
		inline KMsgCache* getCache() {
			return &_cache;
		}

		void req();
		void rep(order_id_t&, const char*, size_t);

	private:
		// initializer list
		zmq::context_t* _ctx;
		std::string _listen_addr;
		std::string _in_addr;
		short int _in_threads;
		std::string _out_addr;
		short int _out_threads;

		// ZMQ sockets
		zmq::socket_t *_frontend;
		zmq::socket_t *_in;
		zmq::socket_t *_out;

		// order interface 
		capk::OrderInterface* _interface;

		// cache for current orders
		KMsgCache _cache;

};




#endif // _K_MSG_PROCESSOR_
