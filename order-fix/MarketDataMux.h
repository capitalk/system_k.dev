#ifndef __MARKET_DATA_MUX__
#define __MARKET_DATA_MUX__

#include <zmq.hpp>

#include <string>

#include "ClientMarketDataInterface.h"
#include "logging.h"

const int MAX_MD_INTERFACES = 10;

class MarketDataMux
{
	public: 
		MarketDataMux(zmq::context_t* context, 
				const std::string& inprocAddr):
				_context(context),
				_inprocAddr(inprocAddr),
				_mdArraySize(0),
				_stopRequested(false),
				_msgCount(0)
		{
		};

		~MarketDataMux();

		// TODO - change to return int = num of installed interfaces
		void addMarketDataInterface(ClientMarketDataInterface* mdi) {
			if (!mdi) { 
				return;
			}
			_mdArray[_mdArraySize] = mdi;	
			_mdArraySize++;
		}

		int run();
		void stop();

	private:
		void rcv_RESPONSE(zmq::socket_t* sock);
		// initializer list 
		zmq::context_t* _context;
		std::string _inprocAddr;
		size_t _mdArraySize;
		volatile bool _stopRequested;
		int64_t _msgCount;

		ClientMarketDataInterface* _mdArray[MAX_MD_INTERFACES];
		zmq::pollitem_t* _poll_items;
		zmq::socket_t* _inproc;	// from strategy->venue
		

};

#endif // __MARKET_DATA_MUX__
