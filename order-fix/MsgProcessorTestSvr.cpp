#include "KMsgProcessor.h"
#include "KMsgHandler.h"
#include "KOrderCache.h"

#include <google/dense_hash_map>
#include "proto/new_order_single.pb.h"
#include "proto/capk_globals.pb.h"
#include "StubHandler.h"

// C
//void *g_zmq_ctx;
// 
// C++
//zmq::context_t ctx;

using google::dense_hash_map;
using std::tr1::hash;			// for hash function - TODO change to Murmur3

struct eqstr
{
	bool operator()(const char* s1, const char* s2) const
	{
		return (s1 == s2) || (s1 && s2 && strcmp(s1, s2) == 0);
	}
};


dense_hash_map<const char*, int, hash<const char*>, eqstr> gMsgs;

int
main(int argc, char **argv)
{
	// C
	//g_zmq_ctx = zmq_init(1);
	//
	// C++
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	
	gMsgs.set_empty_key("E");
	gMsgs.set_deleted_key("D");

	zmq::context_t ctx(1);

	logging_init("testsvr.log");

	StubHandler* stubHandler = new StubHandler();

	KMsgProcessor k(&ctx,
				"tcp://127.0.0.1:9999", 
				"inproc://whatthefuck",
				1,
				stubHandler);

	return k.run();
}
