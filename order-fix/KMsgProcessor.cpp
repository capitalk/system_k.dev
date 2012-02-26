#include "KMsgProcessor.h"



KMsgProcessor::KMsgProcessor(zmq::context_t *ctx, 
							const char* listen_addr,
							const char* inproc_addr, 
							const short int num_threads,
							KMsgHandler* handler):
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


int 
KMsgProcessor::run()
{

	zmq::socket_t frontend(*_ctx, ZMQ_ROUTER);
	zmq::socket_t backend(*_ctx, ZMQ_DEALER);
	
	int zero = 0;
	frontend.setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 
	backend.setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 

	#ifdef LOG
	pan::log_DEBUG("KMsgProcessor binding: ", _listen_addr.c_str());	
	pan::log_DEBUG("KMsgProcessor binding: ", _inproc_addr.c_str());	
	#endif
	frontend.bind(_listen_addr.c_str());
	backend.bind(_inproc_addr.c_str());

	// container for routers 
	KMsgRouter *routers[_num_threads];
	
	pan::log_DEBUG("Starting threads: ",  pan::integer(_num_threads));
	boost::thread_group threads;	
	for (int nthreads = 0; nthreads < _num_threads; nthreads++) {
		// can't create array of objects on stack with non-default ctor
		// so do it one at a time as we create threads
		routers[nthreads] = new KMsgRouter(_ctx, _inproc_addr);
		// create worker
		threads.create_thread(boost::bind(&KMsgRouter::run, routers[nthreads]));
	}
			
	// pass messages to the backend inproc sockets
	// TODO need to wait until sockets are bound and working
	// before we start sending messages - so for now sleep but we should
	// really use a more robust method
	pan::log_DEBUG("Sleeping before receiving messages");
	sleep(5);
	int64_t more = 0;
	size_t more_size = sizeof(more);	
	bool rc;
	int count = 0;
	zmq::pollitem_t poll_items[] =  {
		{ frontend, NULL, ZMQ_POLLIN, 0},
		{ backend, NULL, ZMQ_POLLIN, 0}
	};

	while(1) {
		zmq::poll(poll_items, 2, -1);		
		if (poll_items[0].revents & ZMQ_POLLIN) {
			do {
				zmq::message_t msg;
				rc = frontend.recv(&msg, 0);
				assert(rc);
#ifdef LOG
				pan::log_DEBUG("KMsgProcessor frontend recv'd [", 
					pan::integer(msg.size()), "] ", 
					pan::integer(*(int*)msg.data()), " ", pan::integer(count++));	
#endif
				frontend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
				rc = backend.send(msg, more ? ZMQ_SNDMORE : 0);
				assert(rc);
			} while (more);	
		}

		if (poll_items[1].revents & ZMQ_POLLIN) {
			do {
				zmq::message_t reply;
				rc = backend.recv(&reply, 0);
				assert(rc);	
#ifdef LOG
				pan::log_DEBUG("KMsgProcessor backend recv'd [", 
					pan::integer(reply.size()), "] ", 
					pan::integer(*(int*)reply.data()));	
#endif
				backend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
				rc = frontend.send(reply, more ? ZMQ_SNDMORE : 0);
				assert(rc);
			} while (more);	
		}
	}	
}

