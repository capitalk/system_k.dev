#include "MarketDataMux.h"
#include "KMsgTypes.h"

#include "proto/spot_fx_md_1.pb.h"

MarketDataMux::~MarketDataMux()
{
	if (_inproc) {
		delete _inproc;
	}		
	if (_poll_items) {
		delete [] _poll_items;
	}
}

void 
MarketDataMux::stop()
{
	_stopRequested = true;
}

int	
MarketDataMux::run()
{
	//try {
		assert(_context != NULL);

		_inproc = new zmq::socket_t(*_context, ZMQ_PAIR);
		assert(_inproc);
        pan::log_DEBUG("Binding MarketDataMux inproc addr: ", _inprocAddr.c_str());
        _inproc->bind(_inprocAddr.c_str());
/*
		for (size_t i = 0; i<_mdArraySize; i++) {
			_mdArray[i]->init();					
		}
*/
		
		// 0th item in poll_items is always inproc socket
		_poll_items = new zmq::pollitem_t[_mdArraySize + 1];
		
		_poll_items[0].socket = *_inproc;
		_poll_items[0].fd = NULL;
		_poll_items[0].events = ZMQ_POLLIN;
		_poll_items[0].revents = 0;
		
		for (size_t i = 0; i < _mdArraySize; i++) {
			_poll_items[i+1].socket = *(_mdArray[i]->getInterfaceSocket());
			_poll_items[i+1].fd = NULL;
			_poll_items[i+1].events = ZMQ_POLLIN;
			_poll_items[i+1].revents = 0;
		}
		
		bool rc = false;
		int ret = -1;	
		int64_t more = 0;
		size_t more_size = sizeof(more);

		while (1 && _stopRequested == false) {
			ret = zmq::poll(_poll_items, _mdArraySize + 1, -1);
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
					pan::log_DEBUG("MDMUX received msg for iterface id: ", pan::integer(venue_id));

					size_t sockIdx;
					for (sockIdx = 0; sockIdx < _mdArraySize; sockIdx++) {
						if (_mdArray[sockIdx]->getInterfaceID() == venue_id) {
							venue_sock = _mdArray[sockIdx]->getInterfaceSocket();
							assert(venue_sock);
							pan::log_DEBUG("MDMUX found interface socket for id: ", pan::integer(venue_id));
						}
					}
					if (sockIdx > _mdArraySize) {
						pan::log_CRITICAL("MDMUX cant find interface for id: ", pan::integer(venue_id));
					}

				do {
					// recv and forward remaining frames 
					zmq::message_t msg;
					rc = _inproc->recv(&msg, 0);
					pan::log_DEBUG("MDMUX forwarding frame from inproc: ", pan::blob(msg.data(), msg.size()));
					assert(rc);		
					_inproc->getsockopt(ZMQ_RCVMORE, &more, &more_size);
					rc = venue_sock->send(msg, more ? ZMQ_SNDMORE : 0);	
				} while (more);
				pan::log_DEBUG("MDMUX finished forwarding");
			}
			else {	
			// messages returning from venue
			// don't need to be routed
			    //pan::log_DEBUG("MDMUX there are ", pan::integer(_mdArraySize), " items in oiArray");
				for (size_t j = 0; j<_mdArraySize; j++) {
					//pan::log_DEBUG("MDMUX checking incoming messages on oiArray: ", pan::integer(j));
					if (_poll_items[j+1].revents && ZMQ_POLLIN) {
						zmq::socket_t* sock = _mdArray[j]->getInterfaceSocket();
						assert(sock);
						_msgCount++;	
						rcv_RESPONSE(sock);
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
MarketDataMux::rcv_RESPONSE(zmq::socket_t* sock)
{
	int64_t more = 0;
	size_t more_size = sizeof(more);
	//pan::log_DEBUG("Entering recv loop");
	do {
		zmq::message_t msgframe;
		sock->recv(&msgframe, 0); 
        /*
		pan::log_DEBUG("Received msgframe: size=", 
						pan::integer(msgframe.size()), 
						" data=", 
						pan::blob(static_cast<const void*>(msgframe.data()), msgframe.size()));
                        */
		zmq_getsockopt(*sock, ZMQ_RCVMORE, &more, &more_size);
        //pan::log_DEBUG("Forwarding data to inproc sock");
		_inproc->send(msgframe, more ? ZMQ_SNDMORE : 0);
	} while (more);
	//pan::log_DEBUG("Exiting recv loop");
}



