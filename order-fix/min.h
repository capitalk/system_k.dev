
#ifndef _K_MSG_PROCESSOR_
#define _K_MSG_PROCESSOR_

#include <utils/zhelpers.hpp>
#include <string>
#include <memory.h>
class KMsgProcessor
{
	public:
		KMsgProcessor(zmq::context_t* ctx, 
					const char* listen_addr, 
					const char* inproc_addr, 
					const short int num_threads);
		~KMsgProcessor();


	private:
		zmq::context_t* _ctx;
		std::string _listen_addr;
		std::string _inproc_addr;
		short int _num_threads;


		void *_server;
		void *_inproc;


};

#endif // _K_MSG_PROCESSOR_
