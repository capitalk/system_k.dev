#include "logging.h"
//#include "timing.h"
//#include "utils/zhelpers.h"
#include <zmq.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#define ZC 1

int
main(int argc, char **argv)
{
	logging_init("cli.log");

	void *ctx = zmq_init(1);
	void *socket = zmq_socket(ctx, ZMQ_DEALER);
		
	int rc = zmq_connect(socket, "tcp://127.0.0.1:9999");
	// use below for when this is a router socket
	//int rc = zmq_bind(socket, "tcp://127.0.0.1:9999");
	assert(rc == 0);

	zmq_msg_t msg;
	size_t msg_size = sizeof(int);
	int x = 99;

	boost::posix_time::ptime start_ptime(boost::posix_time::microsec_clock::local_time()); 
	sleep(3);

#ifdef ZC
	// with zerocopy
	rc = zmq_msg_init_data(&msg, (void*)&x, msg_size, NULL, NULL);
	assert(rc == 0);
#else
	// without zerocopy
	pan::log_DEBUG("NOT using zerocopy");
	rc = zmq_msg_init_size(&msg, msg_size);
	assert(rc == 0);
	memcpy(zmq_msg_data(&msg), (void*)&x, msg_size);
#endif

	// use below line for router socket
	// must be the same identity as the receiving socket has set
	//s_sendmore(socket, "A");
	
	rc = zmq_send(socket, &msg, 0);
	assert(rc == 0);
	zmq_msg_close(&msg);
	pan::log_DEBUG("Receiving message");
	zmq_msg_init(&msg);
	rc = zmq_recv(socket, &msg, 0); 
	pan::log_DEBUG("Received msg: ", pan::integer(*(int*)zmq_msg_data(&msg)));
	zmq_close(&msg);


	boost::posix_time::ptime stop_ptime(boost::posix_time::microsec_clock::local_time()); 
	boost::posix_time::time_duration ptime_duration(stop_ptime - start_ptime); 
	std::cerr << ptime_duration << "\n";
	zmq_close(socket);
	zmq_term(ctx);

}




