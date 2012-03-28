#include "KMsgProcessor.h"
#include "KMsgHandler.h"
//#include "KMsgCache.h"

#include <google/dense_hash_map>
#include "proto/new_order_single.pb.h"
#include "proto/capk_globals.pb.h"
#include "StubHandler.h"

#include "NullOrderInterface.h"

// C
//void *g_zmq_ctx;
// 
// C++
//zmq::context_t ctx;

int
main(int argc, char **argv)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	// C
	//g_zmq_ctx = zmq_init(1);
	//
	// C++
	zmq::context_t ctx(1);

	logging_init("testsvr.log");

	StubHandler* stubHandler = new StubHandler();

	KMsgProcessor k(&ctx,
				"tcp://127.0.0.1:9999", 
				"inproc://whatthefuck",
				1,
				stubHandler);
	capk::NullOrderInterface noi;
	k.setOrderInterface(noi);

	return k.run();
}
