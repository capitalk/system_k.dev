#include "msg_processor.h"
#include "msg_types.h"

namespace capk {

// ZMQ free function for zero copy
void
freenode(void* data, void* hint) {
    if (data) {
#ifdef LOG
		node_t* tmp = static_cast<node_t*>(data);
        pan::log_DEBUG("freenode: ", pan::integer(*(int*)tmp->data()));
#endif
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
                            const char* ping_addr,
							const short int out_threads,
							capk::OrderInterface* oi):
_ctx(ctx), 
_listen_addr(listen_addr),
_inproc_addr(out_addr), 
_ping_addr(ping_addr), 
_out_threads(out_threads),
_interface(oi),
_stop(false)
{
	assert(_ctx);
	assert(_listen_addr.length()>0);
	//assert(_in_addr.length()>0);
	assert(_inproc_addr.length()>0);
	assert(_ping_addr.length()>0);
	//assert(in_threads > 0 && in_threads < 4);
	assert(out_threads > 0 && out_threads < 4);
}

MsgProcessor::~MsgProcessor()
{
	pan::log_DEBUG("MsgProcessor::~MsgProcessor()");
	if (_strategy_msgs_socket) {
		_strategy_msgs_socket->close();
		delete _strategy_msgs_socket;
	}
	if (_inproc_addressing_socket) {
		_inproc_addressing_socket->close();
		delete _inproc_addressing_socket;
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
	assert(_inproc_admin_socket);
	bool rc;
	
	zmq::message_t header(sid.size());
	memcpy(header.data(), sid.get_uuid(), sid.size());
	rc = _inproc_admin_socket->send(header, ZMQ_SNDMORE);
	assert(rc);

	zmq::message_t msg_type(sizeof(capk::HEARTBEAT_ACK));
	memcpy(msg_type.data(), &capk::HEARTBEAT_ACK, sizeof(capk::HEARTBEAT_ACK));
	rc = _inproc_admin_socket->send(msg_type, 0);	
	assert(rc);
}

void
MsgProcessor::snd_STRATEGY_HELO_ACK(const strategy_id_t& sid)
{
	assert(_inproc_admin_socket);
	bool rc;

#ifdef LOG
	pan::log_DEBUG("snd_STRATEGY_HELO_ACK() to SID: ", 
			pan::blob(static_cast<const unsigned char*>(sid.get_uuid()), sid.size()));	
#endif 
	zmq::message_t header(sid.size());
	memcpy(header.data(), sid.get_uuid(), sid.size());
#ifdef LOG
	pan::log_DEBUG("snd_STRATEGY_HELO_ACK() Sending routing header");
#endif
	rc = _inproc_admin_socket->send(header, ZMQ_SNDMORE);
	assert(rc);

	zmq::message_t msg_type(sizeof(capk::STRATEGY_HELO_ACK));
	memcpy(msg_type.data(), &capk::STRATEGY_HELO_ACK, sizeof(capk::STRATEGY_HELO_ACK));
#ifdef LOG
	pan::log_DEBUG("snd_STRATEGY_HELO_ACK() Sending STRATEGY_HELO_ACK: ", pan::integer(*static_cast<int*>(msg_type.data())));
#endif
	rc = _inproc_admin_socket->send(msg_type, ZMQ_SNDMORE);	
	assert(rc);

	zmq::message_t msg(sizeof(capk::venue_id_t));
	capk::venue_id_t vid = _interface->getVenueID();
	memcpy(msg.data(), &vid, sizeof(vid));
#ifdef LOG
    pan::log_DEBUG("snd_STRATEGY_HELO_ACK() venue_id is: ", pan::integer(vid));
	pan::log_DEBUG("snd_STRATEGY_HELO_ACK() Sending venue_id to HELO: ", pan::blob(msg.data(), msg.size()));
#endif
	rc = _inproc_admin_socket->send(msg, 0);	
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
#ifdef LOG
    uuidbuf_t sidbuf;
#endif

	// get the routing information from ZMQ_ROUTER socket
	// NB TODO this only works for a one hop route!
    rc = _strategy_msgs_socket->recv(&header1, 0);
pan::log_DEBUG("Receiving header: ", pan::blob(header1.data(), header1.size()));
    assert(rc);
	ret = ret_route.addNode(static_cast<const char*>(header1.data()), header1.size());
	assert(ret == 0);

#ifdef LOG
	pan::log_DEBUG("MsgProcessor::handleIncomingClientMessage() frontend rcvd route header[", 
		pan::integer(header1.size()), "] ", pan::integer(*(int*)header1.data()));
#endif
	// rcv msg_type 
    rc = _strategy_msgs_socket->recv(&msg_type, 0);
#ifdef LOG
    pan::log_DEBUG("Receiving msg_type: ", pan::blob(msg_type.data(), msg_type.size()));
#endif
    assert(rc);
	// rcv strategy id
	rc = _strategy_msgs_socket->recv(&sidframe, 0);
#ifdef LOG
    pan::log_DEBUG("Receiving strategy_id: ", pan::blob(sidframe.data(), sidframe.size()));
#endif
	assert(rc);

	// convert msg_type and strategy_id
    if (msg_type.size() < sizeof(uint32_t)) {
        pan::log_ALERT("Received msgtype_t that is less than size of int - can't interpret - DRAINING SOCKET");
        zmq::message_t dead_frame;
		_strategy_msgs_socket->getsockopt(ZMQ_RCVMORE, &more, &more_size);
        while (more)  { 
            pan::log_DEBUG("Dropping dead frame");
            _strategy_msgs_socket->recv(&dead_frame, 0);
		    _strategy_msgs_socket->getsockopt(ZMQ_RCVMORE, &more, &more_size);
        }
        return;
    }
    capk::msg_t msgType = *(static_cast<capk::msg_t*>(msg_type.data()));	
#ifdef LOG
    pan::log_DEBUG("Received msg_type: ", pan::integer(msgType));
    pan::log_DEBUG("Received msg_type: ", pan::blob(&msgType, sizeof(msgType)));
#endif
	strategy_id_t sid;
	sid.set(static_cast<const char*>(sidframe.data()), sidframe.size());

	// TRAP NON ORDER-RELATED MESSAGES
	if (msgType == capk::STRATEGY_HELO) {
#ifdef LOG 
		pan::log_DEBUG("MsgProcessor::handleIncomingClientMessage() rcvd STRATEGY_HELO from SID: ", sid.c_str(sidbuf), " - adding route to cache");
		T0(a)	
#endif
		_scache.add(sid, ret_route);
		_scache.write(STRATEGY_CACHE_FILENAME);	
#ifdef LOG
		TN(b)
		TDIFF(tdiff, a, b)	
		pan::log_DEBUG("Strategy cache write time: ", ptime_string(tdiff));
#endif
		snd_STRATEGY_HELO_ACK(sid);
	}
	else if (msgType == capk::HEARTBEAT) {
#ifdef LOG
		pan::log_DEBUG("MsgProcessor::handleIncomingClientMessage() rcvd HEARTBEAT from SID: ", sid.c_str(sidbuf));
#endif 
		snd_HEARTBEAT_ACK(sid);
	}
	// PROCESS ALL ORDER RELATED MESSAGES
	else {
        assert(msgType == ORDER_NEW || 
                msgType == ORDER_CANCEL || 
                msgType == ORDER_REPLACE);

		rc = _strategy_msgs_socket->recv(&oidframe, 0);
		assert(rc);

		rc = _strategy_msgs_socket->recv(&data, 0);
		assert(rc);

		_strategy_msgs_socket->getsockopt(ZMQ_RCVMORE, &more, &more_size);
		assert(more == 0);

		// Get the order id and data 
		order_id_t oid;
		oid.set(static_cast<const char*>(oidframe.data()), oidframe.size());

		OrderInfo_ptr op = boost::make_shared<OrderInfo>(oid, sid);	
	
		// Add to order cache but don't serialize to disk 
        // KTK TODO - make the order interfaces use RECOVERY SERVICE
		_ocache.add(oid, op);
#if 0
#if SERIALIZE_ORDER_CACHE
        // KTK - don't write orders to disk anymore 
        // since we store the order state and trades in database
        // USE SERIALIZATION SERVICE INSTEAD
#ifdef LOG
        uuidbuf_t oidbuf;
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
#endif // SERIALIZE_ORDER_CACHE
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
    //char sidbuf[UUID_STRLEN+1];
    uuidbuf_t sidbuf;
	zmq::message_t sidframe;

    int64_t more;
    size_t more_size;
    more_size = sizeof(more);
    bool rc;
    int ret;

	// get the sid so we can fetch the reply route
    rc = _inproc_addressing_socket->recv(&sidframe, 0);
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
    // KTK TODO - change this so that it uses memcpy of each node rather than 
    // allocating off the heap for each node. Ugh. 
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
			rc = _strategy_msgs_socket->send(m, ZMQ_SNDMORE);
		}
		// rcv and forward remaining frames to frontend 
		do {
			zmq::message_t frame;
			rc = _inproc_addressing_socket->recv(&frame, 0); 
			assert(rc);
#ifdef LOG
			pan::log_DEBUG("Forwarding message [", pan::integer(frame.size()), "]: ", pan::blob(frame.data(), frame.size()));
#endif
			_inproc_addressing_socket->getsockopt(ZMQ_RCVMORE, &more, &more_size);
			// relay to fron end - "broker"
			rc = _strategy_msgs_socket->send(frame, more ? ZMQ_SNDMORE : 0);
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
#ifdef SERIALIZE_ORDER_CACHE
	bOK =_ocache.read(ORDER_CACHE_FILENAME);
	//assert(bOK);// blow up if we can't read the cache file
#endif // SERIALIZE_CACHE_FILE

	int zero = 0;

#ifdef LOG
	pan::log_DEBUG("MsgProcessor creating and binding strategy msg socket: ", _listen_addr.c_str());	
#endif 
	_strategy_msgs_socket = new zmq::socket_t(*_ctx, ZMQ_ROUTER);
	_strategy_msgs_socket->setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 
	_strategy_msgs_socket->bind(_listen_addr.c_str());
	
#ifdef LOG
	pan::log_DEBUG("MsgProcessor creating and binding inproc addressing socket: ", _inproc_addr.c_str());	
#endif
	_inproc_addressing_socket = new zmq::socket_t(*_ctx, ZMQ_DEALER);
	_inproc_addressing_socket->setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 
	_inproc_addressing_socket->bind(_inproc_addr.c_str());


#ifdef LOG
	pan::log_DEBUG("MsgProcessor creating and binding synchronous ping socket to: ", _ping_addr.c_str());	
#endif 
	_ping_socket = new zmq::socket_t(*_ctx, ZMQ_REP);
	_ping_socket->setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 
	_ping_socket->bind(_ping_addr.c_str());

	// create internal socket to send to reply admin messages on internal queue
	// using recv_internal() to ensure that address headers are set properly
	sleep(2);
#ifdef LOG
	pan::log_DEBUG("MsgProcessor creating and connecting admin socket connecting to: ", _inproc_addr.c_str());	
#endif 
	_inproc_admin_socket = new zmq::socket_t(*_ctx, ZMQ_DEALER);
	_inproc_admin_socket->setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 
	_inproc_admin_socket->connect(_inproc_addr.c_str());



}

void 
MsgProcessor::runPingService()
{
    bool rc = false;
    zmq::message_t ping_frame;
    pan::log_INFORMATIONAL("PING service listening on: ", _ping_addr.c_str()); 
    
    int zero = 0;
    _ping_socket->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));

    int ping_ack = capk::PING_ACK;
    while(_stop != true) {
        zmq::message_t ping_ack_frame(&ping_ack, sizeof(capk::PING_ACK), NULL,  NULL);
        rc = _ping_socket->recv(&ping_frame, 0);
        assert(rc);
#ifdef LOG 
        T0(a);
        pan::log_DEBUG("Received PING msg (", to_simple_string(a).c_str(), ")");
#endif
        rc = _ping_socket->send(ping_ack_frame);
        assert(rc);
#ifdef LOG 
        TN(b);
        pan::log_DEBUG("Sent PING_ACK msg (", to_simple_string(b).c_str(), ")");
#endif
    } 
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
   
    boost::thread* ping_thread = new boost::thread(boost::bind(&MsgProcessor::runPingService, this));

	int ret; 
	int msgcount = 0;
	zmq::pollitem_t poll_items[] =  {
		{ *_strategy_msgs_socket, NULL, ZMQ_POLLIN, 0},
		//{ *_in, NULL, ZMQ_POLLIN, 0},
		{ *_inproc_addressing_socket, NULL, ZMQ_POLLIN, 0}
	};
	while(_stop != true) {
		//ret = zmq::poll(poll_items, 2, -1);
        /* N.B
         * DO NOT USE THE C++ version of poll since this will throw
         * an exception when the spurious EINTR is returned. Simply
         * check for it here, trap it, and move on.
         */
        ret = zmq_poll(poll_items, 2, -1);
        if (ret == -1 and zmq_errno() == EINTR) {
            pan::log_ALERT("EINTR received - FILE: ", __FILE__, " LINE: ", pan::integer(__LINE__));
            continue;
        }

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
				rc = _strategy_msgs_socket->send(reply, more ? ZMQ_SNDMORE : 0);
				assert(rc);
			} while (more);	
		}
*/
	
	}	
	return 0;
}

} // namespace capk
