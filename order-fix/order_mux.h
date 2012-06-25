#ifndef __ORDER_MUX__
#define __ORDER_MUX__

#include <zmq.hpp>

#include <string>

#include "client_order_interface.h"
#include "logging.h"

const size_t MAX_OE_INTERFACES = 10;

class OrderMux
{
	public: 
		OrderMux(zmq::context_t* context, 
				const std::string& inprocAddr):
				_context(context),
				_inprocAddr(inprocAddr),
				_oiArraySize(0),
				_stopRequested(false),
				_msgCount(0)
		{
		};

		~OrderMux();

		// TODO - change to return int = num of installed interfaces
		bool addOrderInterface(capk::ClientOrderInterface* oi) {
			if (!oi) { 
				return false;
			}
            if (_oiArraySize+1 < MAX_OE_INTERFACES) {
			    _oiArray[_oiArraySize] = oi;	
			    _oiArraySize++;
                return true;
            }
            return false;
		}

		int run();
		void stop();

	private:
		void rcv_RESPONSE(zmq::socket_t* sock);
		// initializer list 
		zmq::context_t* _context;
		std::string _inprocAddr;
		size_t _oiArraySize;
		volatile bool _stopRequested;
		int64_t _msgCount;

        capk::ClientOrderInterface* _oiArray[MAX_OE_INTERFACES];
		zmq::pollitem_t* _poll_items;
		zmq::socket_t* _inproc;	// from strategy->venue
		

};

#endif // __ORDER_MUX__
