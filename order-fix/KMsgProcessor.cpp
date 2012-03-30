#include "KMsgProcessor.h"

void freenode(void* data, void* hint);

KMsgProcessor::KMsgProcessor(zmq::context_t *ctx, 
							const char* listen_addr,
							const char* in_addr, 
							const short int in_threads,
							const char* out_addr, 
							const short int out_threads,
							KMsgHandler* handler,
							capk::OrderInterface* oi):
_ctx(ctx), 
_listen_addr(listen_addr),
_in_addr(in_addr), 
_in_threads(in_threads),
_out_addr(out_addr), 
_out_threads(out_threads),
_interface(oi)
{
	assert(_ctx);
	assert(_listen_addr.length()>0);
	assert(_in_addr.length()>0);
	assert(_out_addr.length()>0);
	assert(in_threads > 0 && in_threads < 4);
	assert(out_threads > 0 && out_threads < 4);
/*
	_inproc = zmq_socket(_ctx, ZMQ_DEALER);
	int zero = 0;
	zmq_setsockopt (_inproc, ZMQ_LINGER, &zero, sizeof(zero));
*/
}

KMsgProcessor::~KMsgProcessor()
{
	if (_frontend) {
		delete _frontend;
	}
	if (_in) {
		delete _in;
	}
	if (_out) {
		delete _out;
	}
}

void
KMsgProcessor::req()
{
	zmq::message_t header1;
	zmq::message_t msg_type;
	zmq::message_t oidframe;
	zmq::message_t sidframe;
	zmq::message_t data;

    int64_t more;
    size_t more_size;
    more_size = sizeof(more);
    bool rc;
    int ret;

    rc = _frontend->recv(&header1, 0);
    assert(rc);
    rc = _frontend->recv(&msg_type, 0);
    assert(rc);
    rc = _frontend->recv(&oidframe, 0);
    assert(rc);
    rc = _frontend->recv(&sidframe, 0);
    assert(rc);
    rc = _frontend->recv(&data, 0);
    assert(rc);
    _frontend->getsockopt(ZMQ_RCVMORE, &more, &more_size);
    assert(more == 0);

	// Get the order id and strateagy id 
	order_id_t oid;
	oid.set(static_cast<const char*>(oidframe.data()), oidframe.size());

	strategy_id_t sid;
	oid.set(static_cast<const char*>(sidframe.data()), sidframe.size());

	OrderInfo_ptr op = boost::make_shared<OrderInfo>(oid, sid);	
	ret = op->pushRoute(static_cast<const char*>(header1.data()), header1.size());
	assert(ret);

	// Add to cache
	_cache.add(oid, op);

	int msgType;
	msgType = *(static_cast<int*>(msg_type.data()));	
	
	// Dispacth message to interface
	char* d = new char[data.size()];		
	_interface->dispatch(msgType, d, data.size());

}


void
KMsgProcessor::rep(order_id_t& oid, const char* s, size_t len)
{
    char oidbuf[UUID_STRLEN+1];
    oid.c_str(oidbuf);
    #ifdef LOG
    pan::log_DEBUG("Replying to: ", oidbuf);
    #endif
    // lookup in cache
    OrderInfo_map* map = _cache.getCache();
    assert(map);
    OrderInfo_ptr p = (*map)[oid];

    // send routing headers
    bool rc = false;
    size_t num_nodes = p->routeSize();
    if (num_nodes > 0) {
        for (size_t i = 0; i<num_nodes; i++) {
            node_t* pnode = new node_t();
            assert(pnode);
            assert(p->popRoute(pnode) == 0);
            zmq::message_t m(pnode->data(), pnode->size(), freenode,  NULL);
            rc = _frontend->send(m, ZMQ_SNDMORE);
        }
    }
    else {
        pan::log_CRITICAL("Trying to reply to oid(", oid.c_str(oidbuf), " but no nodes found.");
    }

    zmq::message_t reply(oid.size());
    memcpy(reply.data(), oid.get_uuid(), oid.size());

    rc = _frontend->send(reply, 0);
    assert(rc);

}

int 
KMsgProcessor::run()
{

	_frontend = new zmq::socket_t(*_ctx, ZMQ_ROUTER);
	_in = new zmq::socket_t(*_ctx, ZMQ_DEALER);
	_out = new zmq::socket_t(*_ctx, ZMQ_DEALER);
	
	int zero = 0;
	_frontend->setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 
	_in->setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 
	_out->setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 

	#ifdef LOG
	pan::log_DEBUG("KMsgProcessor binding frontend: ", _listen_addr.c_str());	
	pan::log_DEBUG("KMsgProcessor binding inbound: ", _in_addr.c_str());	
	pan::log_DEBUG("KMsgProcessor binding outbound: ", _out_addr.c_str());	
	#endif
	_frontend->bind(_listen_addr.c_str());
	_in->bind(_in_addr.c_str());
	_out->bind(_out_addr.c_str());
/*
	// container for routers 
	KMsgRouter *routers[_num_threads];
	
	pan::log_DEBUG("Starting threads: ",  pan::integer(_num_threads));
	boost::thread_group threads;	
	for (int nthreads = 0; nthreads < _num_threads; nthreads++) {
		// can't create array of objects on stack with non-default ctor
		// so do it one at a time as we create threads
		routers[nthreads] = new KMsgRouter(_ctx, _in_addr, this);
		routers[nthreads]->setOrderInterface(*getOrderInterface());
		// create worker
		threads.create_thread(boost::bind(&KMsgRouter::run, routers[nthreads]));
		pan::log_DEBUG("Created thread: ", pan::integer(nthreads));
	}
*/			
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
		{ *_frontend, NULL, ZMQ_POLLIN, 0},
		{ *_in, NULL, ZMQ_POLLIN, 0},
		{ *_out, NULL, ZMQ_POLLIN, 0}
	};

	while(1) {
		zmq::poll(poll_items, 3, -1);		
		// fan-in from multiple strategies (clients)
		if (poll_items[0].revents & ZMQ_POLLIN) {
			req();
/*
			do {
				zmq::message_t msg;
				rc = _frontend->recv(&msg, 0);
				assert(rc);
#ifdef LOG
				pan::log_DEBUG("KMsgProcessor frontend recv'd [", 
					pan::integer(msg.size()), "] ", 
					pan::integer(*(int*)msg.data()), " ", pan::integer(count++));	
#endif
				_frontend->getsockopt(ZMQ_RCVMORE, &more, &more_size);
				rc = _in->send(msg, more ? ZMQ_SNDMORE : 0);
				assert(rc);
			} while (more);	
*/
		}

		
		if (poll_items[1].revents & ZMQ_POLLIN) {
			do {
				zmq::message_t reply;
				rc = _in->recv(&reply, 0);
				assert(rc);	
#ifdef LOG
				pan::log_CRITICAL("Should never receive on in socket");
				pan::log_DEBUG("KMsgProcessor in recv'd [", 
					pan::integer(reply.size()), "] ", 
					pan::integer(*(int*)reply.data()));	
#endif
				_in->getsockopt(ZMQ_RCVMORE, &more, &more_size);
				rc = _frontend->send(reply, more ? ZMQ_SNDMORE : 0);
				assert(rc);
			} while (more);	
		}
	
		if (poll_items[2].revents & ZMQ_POLLIN) {
			do {
				zmq::message_t reply;
				rc = _out->recv(&reply, 0);
				assert(rc);	
#ifdef LOG
				pan::log_DEBUG("KMsgProcessor out recv'd [", 
					pan::integer(reply.size()), "] ", 
					pan::integer(*(int*)reply.data()));	
#endif
				_out->getsockopt(ZMQ_RCVMORE, &more, &more_size);
				rc = _frontend->send(reply, more ? ZMQ_SNDMORE : 0);
				assert(rc);
			} while (more);	
		}
	}	
}

/* Moved to header inline
void
KMsgProcessor::setOrderInterface(capk::OrderInterface& interface)
{
	this->_interface = &interface;
}

capk::OrderInterface* 
KMsgProcessor::getOrderInterface()
{
	return _interface;
}
*/
