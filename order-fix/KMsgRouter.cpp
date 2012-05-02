#include "KMsgRouter.h"


KMsgRouter::KMsgRouter(zmq::context_t* context, 
						const std::string& inprocAddr, 
						KMsgProcessor *msgProcessor): 
	_context(context), 
	_inprocAddr(inprocAddr),  
	_stopRequested(false),
	_msgProcessor(msgProcessor)
{

}

KMsgRouter::~KMsgRouter() 
{
    if (_inproc) delete _inproc;
}

void 
KMsgRouter::stop() 
{
    _stopRequested = true;
}

void
KMsgRouter::rcvMsg()
{

	zmq::message_t header1;
	zmq::message_t header2;
	zmq::message_t msg_type;
	zmq::message_t data;
	
	int64_t more;
	size_t more_size;
	more_size = sizeof(more);
	bool rc;
	int ret;
	//	KTK TODO - use a stack to process messages rather than assuming 
	//	that all messages will have the same number of frames
	// blocking receive
	rc = _inproc->recv(&header1, 0);
	assert(rc);
	rc = _inproc->recv(&header2, 0);
	assert(rc);
	rc = _inproc->recv(&msg_type, 0);
	assert(rc);
	rc = _inproc->recv(&data, 0);
	assert(rc);
	_inproc->getsockopt(ZMQ_RCVMORE, &more, &more_size);
	assert(more == 0);
	boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time());
// HandleMsgType?

	capkproto::new_order_single nos; 
	if (*(int*)msg_type.data() == ORDER_NEW) {
		nos.ParseFromArray(data.data(), data.size());
		capk::OrderInterface* oi = getOrderInterface();
		order_id_t oid;
		oid.set(nos.order_id().c_str(), nos.order_id().size());
/*
		oi->sndNewOrder(oid, 
						nos.symbol().c_str(),
						static_cast<side_t>(nos.side()),
						nos.order_qty(),
						nos.order_type(),
						nos.price(),
						static_cast<short int>(nos.time_in_force()),
						nos.account().c_str());
					
*/
/// New order entry in interface
#ifdef LOG
		pan::log_DEBUG(nos.DebugString());
#endif
	}
	else {
		pan::log_CRITICAL("Unknown order type: ", __FILE__, " ", pan::integer(__LINE__));
		pan::log_CRITICAL("\t Order type is: ",  pan::integer(*(int*)msg_type.data()));
	}

// HandleMsgType
// Add to cache
// KTK TODO Put oid and sid in header so we dont' have to parse the entire message 
// in order to get those two fields
	strategy_id_t sid;
	sid.set(nos.strategy_id().c_str(), nos.strategy_id().size());
	order_id_t oid;
	oid.set(nos.order_id().c_str(), nos.order_id().size());
	OrderInfo_ptr order_ptr = boost::make_shared<OrderInfo>(oid, sid);
	char buf[UUID_STRLEN + 1];
#ifdef LOG
	pan::log_DEBUG("Creating hash entry for: ", order_ptr->getOrderID().c_str(buf));
	pan::log_DEBUG(__FILE__, 
					" ", 
					pan::integer(__LINE__),
					 " : Cache Size: ", 
					pan::integer(_cache.getCache()->size()));
#endif

	ret = order_ptr->addRoute((const char*)header2.data(), header2.size());
	assert(ret == 0);
	ret = order_ptr->addRoute((const char*)header1.data(), header1.size());
	assert(ret == 0);
	_cache.add(oid, order_ptr);

#ifdef LOG
	std::stringstream strstr;
	strstr << boost::this_thread::get_id(); 
	pan::log_DEBUG("KMsgRouter RCV header 1 [", 
		pan::integer(header1.size()), "] thread(", strstr.str().c_str(), ")");
		pan::log_DEBUG("Header 1: ", pan::integer(header1.size()), " ", pan::integer(*(int*)header1.data()));
		
	pan::log_DEBUG("KMsgRouter RCV header 2 [", 
		pan::integer(header2.size()), "] thread(", strstr.str().c_str(), ")");
		pan::log_DEBUG("Header 2: ", pan::integer(header2.size()), " ", pan::integer(*(int*)header2.data()));

	pan::log_DEBUG("KMsgRouter RCV msg_type [", 
		pan::integer(msg_type.size()), "] thread(", strstr.str().c_str(), ")");

	pan::log_DEBUG("KMsgRouter RCV data [", 
		pan::integer(data.size()), "] thread(", strstr.str().c_str(), ")");
#endif

	boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time());
	boost::posix_time::time_duration duration(time_end - time_start);

#ifdef LOG
	std::stringstream  s;
	s << "Time(us) to recv and parse: " << duration << "\n";
	pan::log_DEBUG(s.str().c_str());
#endif

	repMsg(oid);

}

void
KMsgRouter::run () 
{
    try {
        assert(_context != NULL);
        assert(_connectAddr.c_str() != NULL);
#ifdef LOG
		pan::log_DEBUG("KMsgRouter: inproc addr: ", _inprocAddr.c_str());	
		pan::log_DEBUG("KMsgRouter: stop requested: ", pan::boolean(_stopRequested));	
#endif
        
		pan::log_DEBUG("KMsgRouter creating socket");	
        _inproc = new zmq::socket_t(*_context, ZMQ_DEALER);
        assert(_inproc);
		pan::log_DEBUG("KMsgRouter connecting");	
        _inproc->connect(_inprocAddr.c_str());
        
		int64_t more;
		size_t more_size;
		more_size = sizeof(more);
        while (1 && _stopRequested == false) {
#if 0 
            zmq::message_t header1;
            zmq::message_t header2;
            zmq::message_t msg_type;
			zmq::message_t data;
			
//			KTK TODO - use a stack to process messages rather than assuming 
//			that all messages will have the same number of frames
			// blocking receive
			boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time());
			rc = _inproc->recv(&header1, 0);
			assert(rc);
			rc = _inproc->recv(&header2, 0);
			assert(rc);
			rc = _inproc->recv(&msg_type, 0);
			assert(rc);
			rc = _inproc->recv(&data, 0);
			assert(rc);
			_inproc->getsockopt(ZMQ_RCVMORE, &more, &more_size);
			assert(more == 0);
// HandleMsgType?
			assert(*(int*)msg_type.data() == ORDER_NEW);
			capk::new_order_single nos; 
			if (*(int*)msg_type.data() == ORDER_NEW) {
				nos.ParseFromArray(data.data(), data.size());
				_app->
				pan::log_DEBUG(nos.DebugString());
			}
			else {
				pan::log_CRITICAL("Unknown order type: ", __FILE__, " ", pan::integer(__LINE__));
				pan::log_CRITICAL("\t Order type is: ",  pan::integer(*(int*)msg_type.data()));
			}

// HandleMsgType
// Add to cache
// Put oid and sid in header so we dont' have to parse the entire message 
// in order to get those two fields
			strategy_id_t sid;
			sid.set(nos.strategy_id().c_str(), nos.strategy_id().size());
			order_id_t oid;
			oid.set(nos.order_id().c_str(), nos.order_id().size());
			OrderInfo_ptr order_ptr = boost::make_shared<OrderInfo>(oid, sid);
			ret = order_ptr->addRoute((const char*)header2.data(), header2.size());
			assert(ret == 0);
			ret = order_ptr->addRoute((const char*)header1.data(), header1.size());
			assert(ret == 0);
			_cache.add(oid, order_ptr);
// Add to cache

#ifdef LOG
			std::stringstream strstr;
			strstr << boost::this_thread::get_id(); 
			pan::log_DEBUG("KMsgRouter RCV header 1 [", 
				pan::integer(header1.size()), "] thread(", strstr.str().c_str(), ")");
				
			pan::log_DEBUG("KMsgRouter RCV header 2 [", 
				pan::integer(header2.size()), "] thread(", strstr.str().c_str(), ")");

			pan::log_DEBUG("KMsgRouter RCV msg_type [", 
				pan::integer(msg_type.size()), "] thread(", strstr.str().c_str(), ")");

			pan::log_DEBUG("KMsgRouter RCV data [", 
				pan::integer(data.size()), "] thread(", strstr.str().c_str(), ")");
#endif

/*
// 
			// assemble the reply and the second reply
			zmq::message_t reply_header1(header1.size());
			memcpy(reply_header1.data(), header1.data(), header1.size());

			zmq::message_t reply_header2(header2.size());
			memcpy(reply_header2.data(), header2.data(), header2.size());

			zmq::message_t reply(data.size());
			memcpy(reply.data(), data.data(), data.size());

			rc = _inproc->send(reply_header1, ZMQ_SNDMORE);
			assert(rc);
			rc = _inproc->send(reply_header2, ZMQ_SNDMORE);
			assert(rc);
			rc = _inproc->send(reply, 0);
			assert(rc);
			
*/
			boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time());
			boost::posix_time::time_duration duration(time_end - time_start);
			std::stringstream  s;
			s << "Time(us) to recv and parse: " << duration << "\n";
			pan::log_DEBUG(s.str().c_str());

#endif 
			rcvMsg();
        }
    } 
    catch (std::exception& e) {
        std::cerr << "EXCEPTION: " << __FILE__ << __LINE__ << e.what();
    }
}

// ZMQ free function for zero copy
void
freenode(void* data, void* hint) {
    if (data) {
		node_t* tmp = static_cast<node_t*>(data);
		std::cerr << "freenode: ", pan::integer(*(int*)tmp->data());
        delete(static_cast<node_t*>(data));
#ifdef LOG
		pan::log_DEBUG("freenode called with non-null data - this is OK");
#endif
    }
	else {
#ifdef LOG
		pan::log_INFORMATIONAL("freenode called with NULL data - maybe not OK ");
#endif
	}
}


void 
KMsgRouter::repMsg(order_id_t& oid) 
{
	// Use the order id to lookup the reply route in the hashtable
	// populate zmq msg and send
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
	int ret;
	size_t num_nodes = p->routeSize();
	if (num_nodes > 0) {
		for (size_t i = 1; i<num_nodes; i++) {
			node_t* pnode = new node_t();
			assert(pnode);
			ret = p->getRoute(pnode);
			assert(ret == 0);
			zmq::message_t m(pnode->data(), pnode->size(), freenode,  NULL);
			rc = _inproc->send(m, ZMQ_SNDMORE);
		}
	}	
	else {
		pan::log_CRITICAL("Trying to reply to oid(", oid.c_str(oidbuf), " but no nodes found.");
	}

	zmq::message_t reply(oid.size());
	memcpy(reply.data(), oid.get_uuid(), oid.size());

	rc = _inproc->send(reply, 0);
	assert(rc);
	

/* STATIC VERSION OF PATH RETRIEVAL FROM MSG
	node_t n1;
	node_t n2;
	assert(p->popRoute(&n1) == 0);
	assert(p->popRoute(&n2) == 0);

	// assemble the reply and the second reply
	zmq::message_t reply_header1(n1.size());
	memcpy(reply_header1.data(), n1.data(), n1.size());

	zmq::message_t reply_header2(n2.size());
	memcpy(reply_header2.data(), n2.data(), n2.size());

	zmq::message_t reply(oid.size());
	memcpy(reply.data(), oid.get_uuid(), oid.size());

	bool rc;
	rc = _inproc->send(reply_header1, ZMQ_SNDMORE);
	assert(rc);
	rc = _inproc->send(reply_header2, ZMQ_SNDMORE);
	assert(rc);
	rc = _inproc->send(reply, 0);
	assert(rc);
*/
			
}  

void
KMsgRouter::setOrderInterface(capk::OrderInterface& interface) 
{
	this->_interface = &interface;
}

capk::OrderInterface*
KMsgRouter::getOrderInterface() 
{
	return _interface;
}

KMsgCache*
KMsgRouter::getCache()
{
	return &_cache;
}
