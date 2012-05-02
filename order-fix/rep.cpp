//#include <utils/zhelpers.hpp>

#include <zmq.hpp>
//#include <czmq.h>
#include <string>
#include <memory.h>
#include "logging.h"
#include "timing.h"


int 
main()
{
	zmq::context_t ctx(1);
	logging_init("rep.log");
	zmq::socket_t recv_sock(ctx, ZMQ_DEALER);

	int zero = 0;
	recv_sock.setsockopt(ZMQ_LINGER, &zero, sizeof(zero)); 
	// use below when "client" is a router
	//recv_sock.setsockopt(ZMQ_IDENTITY, "A",  strlen("A"));
	
	recv_sock.bind("tcp://127.0.0.1:9999");
	// use below when "client" is a router
	//recv_sock.connect("tcp://127.0.0.1:9999");

			
	while(1) {
		zmq::message_t m1;
		zmq::message_t m2;
		zmq::message_t m3;
		//zmq::message_t msg;
		int64_t more = 0;
		size_t more_size = sizeof(more);	
		bool rc;

		pan::log_DEBUG("---------------->");
		do {
			rc = recv_sock.recv(&m1, 0);
			rc = recv_sock.recv(&m2, 0);
			assert(rc);
			size_t msg_size = m2.size();
			pan::log_DEBUG("Received msg size: ", pan::integer(msg_size));
			recv_sock.getsockopt(ZMQ_RCVMORE, &more, &more_size);
		} while (more);	

			m3.rebuild(4);
			int s = 9999; 
			memcpy(m3.data(), &s, sizeof(int));
		//pan::log_DEBUG("Sending msg: ", pan::integer(*(int*)m1.data()));
		pan::log_DEBUG("Sending msg: ", pan::integer(*(int*)m3.data()));
		//recv_sock.send(m1, ZMQ_SNDMORE);
		recv_sock.send(m3, 0);
		pan::log_DEBUG("<----------------");
/*
		pan::log_DEBUG("---------------->");
		do {
			rc = recv_sock.recv(&msg, 0);
			assert(rc);
			size_t msg_size = msg.size();
			char* s = new char[msg_size];
			memset(s, 0, msg_size);
			memcpy(s, msg.data(), msg_size);
			pan::log_DEBUG("Received msg size: ", pan::integer(msg_size));
			recv_sock.getsockopt(ZMQ_RCVMORE, &more, &more_size);
		} while (more);	
		recv_sock.send(msg, 0);
		pan::log_DEBUG("<----------------");
*/
	}	
		
}

