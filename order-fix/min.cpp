#include "min.h"



KMsgProcessor::KMsgProcessor(zmq::context_t *ctx, 
							const char* listen_addr, 
							const char* inproc_addr, 
							const short int num_threads):
_ctx(ctx), 
_listen_addr(listen_addr), 
_inproc_addr(inproc_addr), 
_num_threads(num_threads)
{
	assert(_ctx);
	assert(_listen_addr.length()>0);
	assert(_inproc_addr.length()>0);
/*
	_inproc = zmq_socket(_ctx, ZMQ_DEALER);
	int zero = 0;
	zmq_setsockopt (_inproc, ZMQ_LINGER, &zero, sizeof(zero));
*/
}

KMsgProcessor::~KMsgProcessor()
{

}



