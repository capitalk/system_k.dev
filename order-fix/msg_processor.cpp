#include "msg_processor.h"
#include "msg_types.h"

namespace capk {

// ZMQ free function for zero copy
void
freenode(void* data, void* hint) {
    if (data) {
		node_t* tmp = static_cast<node_t*>(data);
		std::cerr << "freenode: ", pan::integer(*(int*)tmp->data());
        delete(static_cast<node_t*>(data));
    }
	else {
#ifdef LOG
		pan::log_CRITICAL("freenode called with NULL data - maybe not OK ");
#endif
	}
}

MsgProcessor::MsgProcessor(zmq::context_t *ctx, 
							const char* listen_addr,
							const char* out_addr, 
							const short int out_threads,
							capk::OrderInterface* oi):
_ctx(ctx), 
_listen_addr(listen_addr),
_out_addr(out_addr), 
_out_threads(out_threads),
_interface(oi)
{
	assert(_ctx);
	assert(_listen_addr.length()>0);
	//assert(_in_addr.length()>0);
	assert(_out_addr.length()>0);
	//assert(in_threads > 0 && in_threads < 4);
	assert(out_threads > 0 && out_threads < 4);
}

MsgProcessor::~MsgProcessor()
{
	pan::log_DEBUG("MsgProcessor::~MsgProcessor()");
	if (_frontend) {
		_frontend->close();
		delete _frontend;
	}
	if (_out) {
		_out->close();
		delete _out;
	}
}

/*
std::ostream& operator << (std::ostream& os, const node_t& rhs) {
	for (int i = 0; i < MSG_ADDR_LEN; i++) {
		os << rhs.addr[i];	
	}	
	return os;
};
*/

void
MsgProcessor::snd_HEARTBEAT_ACK(const strategy_id_t& sid)
{
	assert(_admin);
	bool rc;
	
	zmq::message_t header(sid.size());
	memcpy(header.data(), sid.get_uuid(), sid.size());
	rc = _admin->send(header, ZMQ_SNDMORE);
	assert(rc);

	zmq::message_t msg_type(sizeof(capk::HEARTBEAT_ACK));
	memcpy(msg_type.data(), &capk::HEARTBEAT_ACK, sizeof(capk::HEARTBEAT_ACK));
	rc = _admin->send(msg_type, 0);	
	assert(rc);
}

void
MsgProcessor::snd_STRATEGY_HELO_ACK(const strategy_id_t& sid)
{
	assert(_admin);
	bool rc;

#ifdef LOG
	pan::log_DEBUG("MsgProcessor::snd_STRATEGY_HELO_ACK() to SID: ", 
			pan::blob(static_cast<const unsigned char*>(sid.get_uuid()), sid.size()));	
#endif 
	zmq::message_t header(sid.size());
	memcpy(header.data(), sid.get_uuid(), sid.size());
#ifdef LOG
	pan::log_DEBUG("Sending routing header");
#endif
	rc = _admin->send(header, ZMQ_SNDMORE);
	assert(rc);

	zmq::message_t msg_type(sizeof(capk::STRATEGY_HELO_ACK));
	memcpy(msg_type.data(), &capk::STRATEGY_HELO_ACK, sizeof(capk::STRATEGY_HELO_ACK));
#ifdef LOG
	pan::log_DEBUG("Sending helo ack: ", pan::blob(msg_type.data(), msg_type.size()));
#endif
	rc = _admin->send(msg_type, ZMQ_SNDMORE);	
	assert(rc);

	zmq::message_t msg(sizeof(capk::venue_id_t));
	capk::venue_id_t vid = _interface->getVenueID();
	memcpy(msg.data(), &vid, sizeof(vid));
#ifdef LOG
	pan::log_DEBUG("Sending helo data: ", pan::blob(msg.data(), msg.size()));
#endif
	rc = _admin->send(msg, 0);	
	assert(rc);

			
};

void
MsgProcessor::handleIncomingClientMessage()
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
	route_t ret_route;

	// get the routing information from ZMQ_ROUTER socket
	// NB TODO this only works for a one hop route!
    rc = _frontend->recv(&header1, 0);
    assert(rc);
	ret = ret_route.addNode(static_cast<const char*>(header1.data()), header1.size());
	assert(ret == 0);

#ifdef LOG
	pan::log_DEBUG("MsgProcessor::handleIncomingClientMessage() frontend rcvd route header[", 
		pan::integer(header1.size()), "] ", pan::integer(*(int*)header1.data()));
#endif
	// rcv msg_type 
    rc = _frontend->recv(&msg_type, 0);
    assert(rc);
	// rcv strategy id
	rc = _frontend->recv(&sidframe, 0);
	assert(rc);

	// convert msg_type and strategy_id
	capk::msg_t msgType = *(static_cast<capk::msg_t*>(msg_type.data()));	
#ifdef LOG 
		char sidbuf[UUID_STRLEN];
		pan::log_DEBUG("MsgProcessor::handleIncomingClientMessage() rcvd msg_type: ", pan::integer(msgType));
		pan::log_DEBUG("MsgProcessor::handleIncomingClientMessage() rcvd SID [", pan::integer(sidframe.size()), "]: ", 
			pan::blob(static_cast<const void*>(sidframe.data()), sidframe.size()));	
#endif
	strategy_id_t sid;
	sid.set(static_cast<const char*>(sidframe.data()), sidframe.size());

	// TRAP THE ADMIN MESSAGES
	if (msgType == capk::STRATEGY_HELO) {
#ifdef LOG 
		pan::log_DEBUG("MsgProcessor::handleIncomingClientMessage() rcvd STRATEGY_HELO from SID: ", sid.c_str(sidbuf), " - adding route to cache");
#endif
		_scache.add(sid, ret_route);
		T0(a)	
		_scache.write(STRATEGY_CACHE_FILENAME);	
		TN(b)
#ifdef LOG
		TDIFF(tdiff, a, b)	
		pan::log_DEBUG("Strategy cache write time: ", ptime_string(tdiff));
#endif
		snd_STRATEGY_HELO_ACK(sid);
	}
	else if (msgType == capk::HEARTBEAT) {
		pan::log_DEBUG("MsgProcessor::handleIncomingClientMessage() rcvd HEARTBEAT from SID: ", sid.c_str(sidbuf));
		snd_HEARTBEAT_ACK(sid);
	}
	// PROCESS ALL ORDER RELATED MESSAGES
	else {
		rc = _frontend->recv(&oidframe, 0);
		assert(rc);

		rc = _frontend->recv(&data, 0);
		assert(rc);

		_frontend->getsockopt(ZMQ_RCVMORE, &more, &more_size);
		assert(more == 0);

		// Get the order id and data 
		order_id_t oid;
		oid.set(static_cast<const char*>(oidframe.data()), oidframe.size());

		OrderInfo_ptr op = boost::make_shared<OrderInfo>(oid, sid);	
	
		// Add to order cache
#ifdef LOG
		char oidbuf[UUID_STRLEN+1];
		oid.c_str(oidbuf);
		pan::log_DEBUG("Adding to order cache: ", oidbuf);	
#endif 
		_ocache.add(oid, op);
		T0(a)	
		_ocache.write(ORDER_CACHE_FILENAME);	
		TN(b)
#ifdef LOG
		TDIFF(tdiff, a, b)	
		pan::log_DEBUG("Order cache write time: ", ptime_string(tdiff));
#endif

		// Dispatch all other messages to interface
		char* d = new char[data.size()];		
		memcpy(d, data.data(), data.size());
		_interface->dispatch(msgType, d, data.size());
	}

}

void
MsgProcessor::rcv_internal()
{
    char sidbuf[UUID_STRLEN+1];
	zmq::message_t sidframe;

    int64_t more;
    size_t more_size;
    more_size = sizeof(more);
    bool rc;
    int ret;

	// get the sid so we can fetch the reply route
    rc = _out->recv(&sidframe, 0);
    assert(rc);
#ifdef LOG
	pan::log_DEBUG("MsgProcessor::rcv_internal() rcvd SID: ", 
		pan::blob(static_cast<const void*>(sidframe.data()), sidframe.size()));	
#endif

	strategy_id_t sid;
	sid.set(static_cast<const char*>(sidframe.data()), sidframe.size());

	route_t ret_path = _scache.get(sid);
	size_t num_nodes = ret_path.size();
	assert(num_nodes > 0);

	// if there are nodes in the path
	if (num_nodes > 0) {
#ifdef LOG
		pan::log_DEBUG("Found: ", pan::integer(num_nodes), " nodes for SID: ", sid.c_str(sidbuf));
#endif
		// send routing headers to frontend
		for (size_t i = 0; i<num_nodes; i++) {
			// allocate new node to fetch into
			node_t* pnode = new node_t();
			assert(pnode);
			// fetch the ith node from the route
			ret = ret_path.getNode(i, pnode);
			assert(ret == 0);
#ifdef LOG
			pan::log_DEBUG("\trouting node is: ", pan::integer(*(int*)pnode->data()));
#endif
			// prepend the routing information to the message
			zmq::message_t m(pnode->data(), pnode->size(), freenode,  NULL);
			rc = _frontend->send(m, ZMQ_SNDMORE);
		}
		// rcv and forward remaining frames to frontend 
		do {
			zmq::message_t frame;
			rc = _out->recv(&frame, 0); 
			assert(rc);
#ifdef LOG
			pan::log_DEBUG("Forwarding message [", pan::integer(frame.size()), "]: ", pan::blob(frame.data(), frame.size()));
#endif
			_out->getsockopt(ZMQ_RCVMORE, &more, &more_size);
			// relay to fron end - "broker"
			rc = _frontend->send(frame, more ? ZMQ_SNDMORE : 0);
			assert(rc);
#ifdef LOG
			pan::log_DEBUG("Has more parts? : ", pan::integer(more));
#endif
		} while (more != 0);
	}
	else {
		pan::log_CRITICAL("Strategy id: ", sid.c_str(sidbuf), " not found in cache - appears to be an orphan");
	}
}

void
MsgProcessor::init()
{
	bool bOK;
	bOK =_scache.read(STRATEGY_CACHE_FILENAME);
	//assert(bOK);// blow up if we can't read the cache file
	bOK =_ocache.read(ORDER_CACHE_FILENAME);
	//assert(bOK);// blow up if we can't read the cache file


	_frontend = new zmq::socket_t(*_ctx, ZMQ_ROUTER);
	//_in = new zmq::socket_t(*_ctx, ZMQ_DEALER);
	_out = new zmq::socket_t(*_ctx, ZMQ_DEALER);
	
	int zero = 0;
	_frontend->setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 
	//_in->setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 
	_out->setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 

	#ifdef LOG
	pan::log_DEBUG("MsgProcessor binding frontend: ", _listen_addr.c_str());	
	pan::log_DEBUG("MsgProcessor binding outbound: ", _out_addr.c_str());	
	pan::log_DEBUG("MsgProcessor connecting admin: ", _out_addr.c_str());	
	#endif
	_frontend->bind(_listen_addr.c_str());
	_out->bind(_out_addr.c_str());

	// create internal socket to send to reply admin messages on internal queue
	// using recv_internal()
	sleep(2);
	_admin = new zmq::socket_t(*_ctx, ZMQ_DEALER);
	_admin->setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 
	_admin->connect(_out_addr.c_str());

}

int 
MsgProcessor::run()
{
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
	//pan::log_DEBUG("Sleeping before receiving messages");
	// only need the sleep if we're spinning threads that connect back to the 
	// sockets that this thread creates - as in KMsgRouters above
	//sleep(3);
	//int64_t more = 0;
	//size_t more_size = sizeof(more);	
	//bool rc;
	int ret; 
	int msgcount = 0;
	zmq::pollitem_t poll_items[] =  {
		{ *_frontend, NULL, ZMQ_POLLIN, 0},
		//{ *_in, NULL, ZMQ_POLLIN, 0},
		{ *_out, NULL, ZMQ_POLLIN, 0}
	};
	while(_stop != true) {
		ret = zmq::poll(poll_items, 2, -1);
		if (ret == -1) {
			int err = zmq_errno();
			return err;
		}		
		// fan-in from multiple strategies (clients)
		if (poll_items[0].revents & ZMQ_POLLIN) {
			msgcount++;
			handleIncomingClientMessage();
		}
		if (poll_items[1].revents & ZMQ_POLLIN) {
			rcv_internal();
		}

/*	
		if (poll_items[1].revents & ZMQ_POLLIN) {
			do {
				zmq::message_t reply;
				rc = _in->recv(&reply, 0);
				assert(rc);	
#ifdef LOG
				pan::log_CRITICAL("Should never receive on in socket");
				pan::log_DEBUG("MsgProcessor in recv'd [", 
					pan::integer(reply.size()), "] ", 
					pan::integer(*(int*)reply.data()));	
#endif
				_in->getsockopt(ZMQ_RCVMORE, &more, &more_size);
				rc = _frontend->send(reply, more ? ZMQ_SNDMORE : 0);
				assert(rc);
			} while (more);	
		}
*/
	
	}	
	return 0;
}

} // namespace capk
