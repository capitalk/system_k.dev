
#ifndef _K_MSG_PROCESSOR_
#define _K_MSG_PROCESSOR_

#include <uuid/uuid.h>

//#include <utils/zhelpers.hpp>
#include <zmq.hpp>
//#include <czmq.h>

#include <string>
#include <memory.h>

#include "utils/logging.h"
#include "utils/timing.h"

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
                    const char* ping_addr, 
					const short int out_threads,
					capk::OrderInterface* oi);

		~MsgProcessor();

		void init();

		int run();
		void stop() { _stop = true; };

		capk::OrderInterface* getOrderInterface() {
			return _interface;
		}

		inline const std::string& getOutboundAddr() const {
			return _inproc_addr;
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

        inline const char* getListenerAddr() {
            return _listen_addr.c_str();
        }

		inline zmq::context_t* getZMQContext() const { return _ctx;}

		// recv incoming requests from strategies
		void handleIncomingClientMessage();
		
	private:
		// recv inproc msgs messages from order interface to return 
        // to strategy via _inproc_addressing_socket
		void rcv_internal();

        // run the ping responder in a new thread
        void runPingService();

        // admin message handlers
		//
		// handle strategy helo ack
		void snd_STRATEGY_HELO_ACK(const strategy_id_t&);

		// handle heartbeat
		void snd_HEARTBEAT_ACK(const strategy_id_t&);

		// initializer list
		zmq::context_t* _ctx;
		std::string _listen_addr;
		std::string _inproc_addr;
		std::string _ping_addr;
		short int _out_threads;

		// ZMQ sockets
        // Msgs are received and replied to on this socket only.
        // The inproc socket  simply prepends route information before
        // sending the msg out on _strategy_msgs_socket
		zmq::socket_t *_strategy_msgs_socket;

        // Msgs returning to strategies must be sent through this socket 
        // in order to have correct routing information prepended before 
        // leaving (ultimately on the _strategy_msgs_socket
		zmq::socket_t *_inproc_addressing_socket;

        // Send adming msgs back to clients on the _strategy_msgs_socket
        // N.B only for ASYNCHRONOUS msgs
		zmq::socket_t *_inproc_admin_socket;;

        // Send adming msgs back to clients on the _strategy_msgs_socket
        // N.B only for ASYNCHRONOUS msgs
		zmq::socket_t *_ping_socket;;


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
