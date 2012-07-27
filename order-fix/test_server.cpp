#include "msg_processor.h"
#include "msg_handler.h"

#include <google/dense_hash_map>
#include "proto/new_order_single.pb.h"
#include "proto/capk_globals.pb.h"

#include "stub_handler.h"

#include "null_order_interface.h"

#include <boost/thread.hpp>

// C
//g_zmq_ctx = zmq_init(1);
//
// C++
zmq::context_t ctx(1);


int
main(int argc, char **argv)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	logging_init("testsvr.log");

	StubHandler* stubHandler = new StubHandler();

	capk::NullOrderInterface noi(123456, &ctx);

    capk::MsgProcessor k(&ctx,
				"tcp://127.0.0.1:9999", 
				"inproc://msgout",
				1,
				&noi);
	noi.setMsgProcessor(&k);
	noi.setOutAddr("inproc://msgout");
	boost::thread* t = new boost::thread(boost::bind(&capk::NullOrderInterface::run, &noi));


	return k.run();
}
