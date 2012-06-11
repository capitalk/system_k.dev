#include "OrderMux.h"
#include "KMsgTypes.h"

#include "proto/execution_report.pb.h"
#include "proto/order_cancel_reject.pb.h"


OrderMux::~OrderMux()
{
	if (_inproc) {
		delete _inproc;
	}		
/*
	if (_oiArray) {
		delete [] _oiArray;
	}
*/
	if (_poll_items) {
		delete [] _poll_items;
	}
}

void 
OrderMux::stop()
{
	_stopRequested = true;
}

int	
OrderMux::run()
{
	//try {
		assert(_context != NULL);

		_inproc = new zmq::socket_t(*_context, ZMQ_PAIR);
		assert(_inproc);
		pan::log_DEBUG("Binding OrderMux inproc addr: ", _inprocAddr.c_str());
		_inproc->bind(_inprocAddr.c_str());
/*
		for (size_t i = 0; i<_oiArraySize; i++) {
			_oiArray[i]->init();					
		}
*/
		
		// 0th item in poll_items is always inproc socket
		_poll_items = new zmq::pollitem_t[_oiArraySize + 1];
		
		_poll_items[0].socket = *_inproc;
		_poll_items[0].fd = NULL;
		_poll_items[0].events = ZMQ_POLLIN;
		_poll_items[0].revents = 0;
		
		for (size_t i = 0; i < _oiArraySize; i++) {
			_poll_items[i+1].socket = *(_oiArray[i]->getInterfaceSocket());
			_poll_items[i+1].fd = NULL;
			_poll_items[i+1].events = ZMQ_POLLIN;
			_poll_items[i+1].revents = 0;
		}
		
		bool rc = false;
		int ret = -1;	
		int64_t more = 0;
		size_t more_size = sizeof(more);

		while (1 && _stopRequested == false) {
			ret = zmq::poll(_poll_items, _oiArraySize + 1, -1);
			if (ret < 0) {
				pan::log_CRITICAL("zmq::poll returned errno: ", pan::integer(zmq_errno()));
				return -1;
			}		
			// outbound orders routed to correct venue 
			if (_poll_items[0].revents & ZMQ_POLLIN) {
				_msgCount++;	
					// get the venue id so we can route
					zmq::message_t venue_id_msg;
					rc = _inproc->recv(&venue_id_msg, 0);
					assert(rc);
					// lookup the socket for the venue
					zmq::socket_t* venue_sock;
					int venue_id = *(static_cast<int*>(venue_id_msg.data()));
					pan::log_DEBUG("MUX received msg for iterface id: ", pan::integer(venue_id));

					size_t sockIdx;
					for (sockIdx = 0; sockIdx < _oiArraySize; sockIdx++) {
						if (_oiArray[sockIdx]->getInterfaceID() == venue_id) {
							venue_sock = _oiArray[sockIdx]->getInterfaceSocket();
							assert(venue_sock);
							pan::log_DEBUG("MUX found interface socket for id: ", pan::integer(venue_id));
						}
					}
					if (sockIdx > _oiArraySize) {
						pan::log_CRITICAL("MUX cant find interface for id: ", pan::integer(venue_id));
					}

				do {
					// recv and forward remaining frames 
					zmq::message_t msg;
					rc = _inproc->recv(&msg, 0);
					pan::log_DEBUG("MUX forwarding frame from inproc: ", pan::blob(msg.data(), msg.size()));
					assert(rc);		
					_inproc->getsockopt(ZMQ_RCVMORE, &more, &more_size);
					rc = venue_sock->send(msg, more ? ZMQ_SNDMORE : 0);	
				} while (more);
				pan::log_DEBUG("MUX finished forwarding");
			}
			else {	
			// messages returning from venue
			// don't need to be routed
				for (size_t j = 0; j<_oiArraySize; j++) {
					pan::log_DEBUG("MUX checking incoming messages on oiArray: ", pan::integer(j));
					if (_poll_items[j+1].revents && ZMQ_POLLIN) {
						zmq::socket_t* sock = _oiArray[j]->getInterfaceSocket();
						assert(sock);
						_msgCount++;	
						rcv_RESPONSE(sock);
/*
						do {
							zmq::message_t msg;
							rc = sock->recv(&msg, 0);
							assert(rc);
							pan::log_DEBUG("MUX received from interface: ", pan::blob(msg.data(), msg.size()));
							sock->getsockopt(ZMQ_RCVMORE, &more, &more_size);
							rc = _inproc->send(msg, more ? ZMQ_SNDMORE : 0);	
						} while (more);
*/
					}
				}
			}		
		}
	//}
/*
	catch(std::exception& e) {
		pan::log_CRITICAL("EXCEPTION: ", __FILE__, pan::integer(__LINE__), " ", e.what());
	}	
*/
	return 0;
}


void 
OrderMux::rcv_RESPONSE(zmq::socket_t* sock)
{
	int64_t more = 0;
	size_t more_size = sizeof(more);
	pan::log_DEBUG("Entering recv loop");
	do {
		zmq::message_t msgtypeframe;
		sock->recv(&msgtypeframe, 0); 
		pan::log_DEBUG("Received msgtypeframe: size=", 
						pan::integer(msgtypeframe.size()), 
						" data=", 
						pan::blob(static_cast<const void*>(msgtypeframe.data()), msgtypeframe.size()));

		
		zmq::message_t msgframe;
		sock->recv(&msgframe, 0);
		pan::log_DEBUG("Received msgframe: size=", 
						pan::integer(msgframe.size()), 
						" data=", 
						pan::blob(static_cast<const void*>(msgframe.data()), msgframe.size()));

		

		if (*(static_cast<capk::msg_t*>(msgtypeframe.data())) == capk::STRATEGY_HELO_ACK) {
			pan::log_DEBUG("Received msg type: ", pan::integer(capk::STRATEGY_HELO_ACK),
							" - capk::STRATEGY_HELO_ACK from venue ID: ",
							pan::integer(*(static_cast<capk::venue_id_t*>(msgframe.data()))));
		}
		
        else {
			_inproc->send(msgtypeframe, ZMQ_SNDMORE);
			_inproc->send(msgframe, 0);
        }
/* Don't inspect the protobufs here - just pass them up to the inrpoc socket 
 * and then let the application thread process the messages - more synchronous - easier
 * to digest, understand, debug */
/* 
		if (*(static_cast<capk::msg_t*>(msgtypeframe.data())) == capk::EXEC_RPT) {
			bool parseOK;
			pan::log_DEBUG("Received msg type: ", pan::integer(capk::EXEC_RPT), " - capk::EXEC_RPT");
			capkproto::execution_report er;
			parseOK = er.ParseFromArray(msgframe.data(), msgframe.size());
			assert(parseOK);
			pan::log_DEBUG(er.DebugString());
			// forward msg to application thread
			_inproc->send(msgtypeframe, ZMQ_SNDMORE);
			_inproc->send(msgframe, 0);
		}
		if (*(static_cast<capk::msg_t*>(msgtypeframe.data())) == capk::ORDER_CANCEL_REJ) {
			bool parseOK;
			pan::log_DEBUG("Received msg type: ", pan::integer(capk::ORDER_CANCEL_REJ), " - capk::ORDER_CANCEL_REJ");
			capkproto::order_cancel_reject ocr;
			parseOK = ocr.ParseFromArray(msgframe.data(), msgframe.size());
			assert(parseOK);
			pan::log_DEBUG(ocr.DebugString());
			// forward msg to application thread
			_inproc->send(msgtypeframe, ZMQ_SNDMORE);
			_inproc->send(msgframe, 0);
		}
*/
		zmq_getsockopt(*sock, ZMQ_RCVMORE, &more, &more_size);
		assert(more == 0);
	} while (more);
	pan::log_DEBUG("Exiting recv loop");
}



