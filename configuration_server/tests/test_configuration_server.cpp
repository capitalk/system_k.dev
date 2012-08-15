#include "logging.h"
#include "timing.h"
#include "gtest/gtest.h"
#include <zmq.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#define ZC 1

TEST(CacheTest, GetConfig) 
{

}

int
main(int argc, char **argv)
{
	logging_init("test_config_server.log");

    //int result = RUN_ALL_TESTS();
	void *ctx = zmq_init(1);
	void *socket = zmq_socket(ctx, ZMQ_REQ);
		
	int rc = zmq_connect(socket, "tcp://127.0.0.1:11111");
	assert(rc == 0);

	zmq_msg_t outbound_msg;
    //zmq_msg_init_size(&outbound_msg, sizeof(char));

	boost::posix_time::ptime start_ptime(boost::posix_time::microsec_clock::local_time()); 
    const char REQ_CONFIG = 'C';
    const char REQ_REFRESH = 'R';
#ifdef ZC
	// with zerocopy
	pan::log_DEBUG("Using zerocopy");
	rc = zmq_msg_init_data(&outbound_msg, (void*)&REQ_CONFIG, sizeof(char), NULL, NULL);
	assert(rc == 0);
#else
	// without zerocopy
	pan::log_DEBUG("NOT using zerocopy");
	rc = zmq_msg_init_size(&outbound_msg, sizeof(char));
	assert(rc == 0);
	memcpy(zmq_msg_data(&outbound_msg), (void*)&REQ_CONFIG, msg_size);
#endif

	rc = zmq_send(socket, &outbound_msg, 0);
	assert(rc == 0);
	zmq_msg_close(&outbound_msg);

    // inbound message
    zmq_msg_t inbound_msg;
    zmq_msg_init(&inbound_msg);

    int64_t more = 0;
    size_t more_size = sizeof(more);	
	zmq_msg_init(&inbound_msg);
	pan::log_DEBUG("Receiving message");
    do {
	    rc = zmq_recv(socket, &inbound_msg, 0); 
        assert(rc);
        pan::log_DEBUG("Received msg size: ", pan::integer(zmq_msg_size(&inbound_msg)));
	    pan::log_DEBUG("Received msg: ", pan::blob(zmq_msg_data(&inbound_msg), zmq_msg_size(&inbound_msg)));
        zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &more_size);
    } while (more);	


	zmq_close(&inbound_msg);


	boost::posix_time::ptime stop_ptime(boost::posix_time::microsec_clock::local_time()); 
	boost::posix_time::time_duration ptime_duration(stop_ptime - start_ptime); 
	std::cerr << ptime_duration << "\n";
	zmq_close(socket);
	zmq_term(ctx);

}




