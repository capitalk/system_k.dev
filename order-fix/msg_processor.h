
#ifndef _K_MSG_PROCESSOR_
#define _K_MSG_PROCESSOR_

#include <uuid/uuid.h>

//#include <utils/zhelpers.hpp>
#include <zmq.hpp>
#include <czmq.h>

#include <string>
#include <memory.h>

#include "logging.h"
#include "timing.h"
#include "order_interface.h"
#include "msg_cache.h"

#include <boost/thread.hpp>

namespace capk {
void freenode(void* data, void* hint);

class MsgProcessor
{
	public:
		MsgProcessor(zmq::context_t* ctx, 
					const char* listen_addr, 
					//const char* in_addr, 
					//const short int in_threads,
					const char* out_addr, 
					const short int out_threads,
					capk::OrderInterface* oi);

		~MsgProcessor();

		void init();

		int run();
		void stop() { _stop = true; };

		void setOrderInterface(capk::OrderInterface* interface) {
			this->_interface = interface;
		}
		capk::OrderInterface* getOrderInterface() {
			return _interface;
		}

		inline const std::string& getOutboundAddr() const {
			return _out_addr;
		}
		
		inline unsigned short int getOutThreadCount() const {
			return _out_threads;
		}
		
		inline KOrderCache* getOrderCache() {
			return &_ocache;
		}

		inline KStrategyCache* getStrategyCache() {
			return &_scache;
		}

		inline zmq::context_t* getZMQContext() const { return _ctx;}

		// recv incoming requests from strategies
		void req();

		// recv ipc messages from order interface
		void rcv_internal();

		// admin message handlers
		//
		// handle strategy helo ack
		void snd_STRATEGY_HELO_ACK(const strategy_id_t&);

		// handle heartbeat
		void snd_HEARTBEAT_ACK(const strategy_id_t&);

	private:
		// initializer list
		zmq::context_t* _ctx;
		std::string _listen_addr;
		//std::string _in_addr;
		//short int _in_threads;
		std::string _out_addr;
		short int _out_threads;

		// ZMQ sockets
		zmq::socket_t *_frontend;
		//zmq::socket_t *_in;
		zmq::socket_t *_out;
		zmq::socket_t *_admin;

		// order interface 
		capk::OrderInterface* _interface;

		// cache for current orders
		KOrderCache _ocache;
		// cache for strategy reply routes
		KStrategyCache _scache;

		bool _stop;

};

} // namespace capk


#endif // _K_MSG_PROCESSOR_
