#include "KMsgRouter.h"



KMsgRouter::KMsgRouter(zmq::context_t* context, const std::string& inprocAddr): 
	_context(context), 
	_inprocAddr(inprocAddr),  
	_stopRequested(false)
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
        _inproc = new zmq::socket_t(*_context, ZMQ_ROUTER);
        assert(_inproc);
		pan::log_DEBUG("KMsgRouter connecting");	
        _inproc->connect(_inprocAddr.c_str());
        
		int64_t more;
		size_t more_size;
		more_size = sizeof(more);
		bool rc;
        while (1 && _stopRequested == false) {
            zmq::message_t header1;
            zmq::message_t header2;
            zmq::message_t msg_type;
			zmq::message_t data;
			
//			do {
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
			if (*(int*)msg_type.data() == ORDER_NEW) {
				capitalk::new_order_single nos; 
				nos.ParseFromArray(data.data(), data.size());
				
				pan::log_DEBUG(nos.DebugString());
			}
			else {
				pan::log_CRITICAL("Unknown order type: ", __FILE__, " ", pan::integer(__LINE__));
				pan::log_CRITICAL("\t Order type is: ",  pan::integer(*(int*)msg_type.data()));
			}

// HandleMsgType
#ifdef LOG
			std::stringstream sid;
			sid << boost::this_thread::get_id(); 
			pan::log_DEBUG("KMsgRouter RCV header 1 [", 
				pan::integer(header1.size()), "] thread(", sid.str().c_str(), ")");
				
			pan::log_DEBUG("KMsgRouter RCV header 2 [", 
				pan::integer(header2.size()), "] thread(", sid.str().c_str(), ")");

			pan::log_DEBUG("KMsgRouter RCV msg_type [", 
				pan::integer(msg_type.size()), "] thread(", sid.str().c_str(), ")");

			pan::log_DEBUG("KMsgRouter RCV data [", 
				pan::integer(data.size()), "] thread(", sid.str().c_str(), ")");
#endif

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
			

			boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time());
			boost::posix_time::time_duration duration(time_end - time_start);
			std::stringstream  s;
			s << "Time(us) to recv and parse: " << duration << "\n";
			pan::log_DEBUG(s.str().c_str());

        }
    } 
    catch (std::exception& e) {
        std::cerr << "EXCEPTION: " << __FILE__ << __LINE__ << e.what();
    }
}

