#ifndef __CLIENT_MARKET_DATA_INTERFACE__
#define __CLIENT_MARKET_DATA_INTERFACE__

#include <zmq.hpp>
#include <string>

class ClientMarketDataInterface
{
	public:
		ClientMarketDataInterface(const int venueID,
							zmq::context_t* context, 
							const std::string& interfaceAddr, 
							const std::string& inprocAddr):
		_interfaceID(venueID),
		_context(context), 
		_interfaceAddr(interfaceAddr),
		_inprocAddr(inprocAddr),
		_stopRequested(false),
		_initComplete(false)
		{}
		
		~ClientMarketDataInterface();

		void init();
		//int run();
		//void stop();
		
		const std::string& getInterfaceAddr() const { return _interfaceAddr;}
		const std::string& getInproAddr() const { return _inprocAddr;}
		const int getInterfaceID() const { return _interfaceID;}
		zmq::socket_t* getInterfaceSocket() { return _interface;}
		zmq::socket_t* getInprocSocket() { return _inproc;}
		bool subscribe(const char*);
		bool unsubscribe(const char*);

	private:
		int _interfaceID;
		zmq::context_t* _context;

		std::string _interfaceAddr;
		zmq::socket_t* _interface;

		std::string _inprocAddr;
		zmq::socket_t* _inproc;

		volatile bool _stopRequested;
		int64_t _msgCount;
		bool _initComplete;

};

#endif // __CLIENT_MARKET_DATA_INTERFACE__
