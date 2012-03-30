#include "logging.h"
#include "timing.h"

#include <zmq.hpp>

#include "utils/KTimeUtils.h"
#include "KMsgTypes.h"
#include "google/dense_hash_map"

#include "proto/new_order_single.pb.h"
#include "proto/capk_globals.pb.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <uuid/uuid.h>

#include "KMsgCache.h"

using google::dense_hash_map;

#define ZC 1
//#define SND_EMPTY 1
//#define SND_IDENT 1

#define STRAT_UUID 7020f42e-b6c6-42d1-9b1e-65d968961a06


void 
free_int(void* data, void* hint)
{
	if (data) {
		free(data);
	}
}

int
main(int argc, char **argv)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	logging_init("testcli.log");

	//pan::log_DEBUG("Sleeping...");
	//sleep(3);

	zmq::context_t ctx(1);

	zmq::socket_t socket(ctx, ZMQ_DEALER);

	socket.connect("tcp://127.0.0.1:9999");

	zmq::pollitem_t items[] = {
		//{ socket, NULL, ZMQ_POLLOUT, 0 },
		{ socket, NULL, ZMQ_POLLIN, 0 }
	};
	const int NUM_TEST_MSGS = 1;
	bool rc;
	pan::log_DEBUG("Starting");
	boost::posix_time::ptime start_ptime(boost::posix_time::microsec_clock::local_time()); 
	for (int i = 0; i < NUM_TEST_MSGS; i++) {
	//while (1) {
		////zmq::poll(items, 2, -1);	
		////sleep(1);

		//if (items[0].revents & ZMQ_POLLOUT) {
#ifdef LOG
			pan::log_DEBUG("Preparing message");
#endif
			char msgbuf[256];
			capkproto::new_order_single nos;
			strategy_id_t sid(true);	
			nos.set_strategy_id(sid.get_uuid(), UUID_LEN);			
			order_id oid(true);
			char buf[UUID_STRLEN + 1];
#ifdef LOG
			pan::log_DEBUG("Creating order id: ", oid.c_str(buf));
#endif
			nos.set_order_id(oid.get_uuid(), UUID_LEN);	
			nos.set_symbol("EUR/USD");
			nos.set_side(capkproto::BID);
			nos.set_order_qty(100000);
			nos.set_order_type(capkproto::LIM);
			nos.set_price(1);
			nos.set_time_in_force(capkproto::GFD);
			// nos.set_account("FOOBAR");
#ifdef LOG
			pan::log_DEBUG(nos.DebugString());
#endif
			size_t msgsize = nos.ByteSize();
			assert(msgsize < sizeof(msgbuf));
			nos.SerializeToArray(msgbuf, msgsize);	
			zmq::message_t msg(msgsize);
			memcpy(msg.data(), msgbuf, msgsize);
			int order_new_type = ORDER_NEW;

			zmq::message_t msgtype(&order_new_type, sizeof(order_new_type), NULL, NULL);
#ifdef LOG
			pan::log_DEBUG("Sending type: ", pan::integer(ORDER_NEW));
#endif
			rc = socket.send(msgtype, ZMQ_SNDMORE);
			assert(rc == true);

			zmq::message_t oidframe(oid.get_uuid(), UUID_LEN,  NULL, NULL);
#ifdef LOG
			pan::log_DEBUG("Sending oid: ", oid.c_str(buf));
#endif
			rc = socket.send(oidframe, ZMQ_SNDMORE);
			assert(rc == true);

			zmq::message_t sidframe(sid.get_uuid(), UUID_LEN,  NULL, NULL);
#ifdef LOG
			pan::log_DEBUG("Sending sid: ", sid.c_str(buf));
#endif
			rc = socket.send(sidframe, ZMQ_SNDMORE);
			assert(rc == true);

#ifdef LOG
			pan::log_DEBUG("Sending message ");
#endif
			rc = socket.send(msg, 0);
			assert(rc == true);
			//if (i % 1000 == 0) std::cerr << i << " Messages" << "\n";
		//}
	}
	int rcvcount = 0;
	for (int i = 0; i < NUM_TEST_MSGS; i++) {
	while (1) {
		zmq::poll(items, 1, -1);	
		if (items[0].revents & ZMQ_POLLIN) {
#ifdef LOG
			pan::log_DEBUG("OK to recv");
#endif
			int64_t more = 0;
			size_t more_size = sizeof(more);
			char oidbuf[UUID_STRLEN + 1];	
			do {
#ifdef LOG
				pan::log_DEBUG("Receiving message");
#endif
				zmq::message_t incoming_type;
				zmq::message_t incoming;
				socket.recv(&incoming_type, 0); 
#ifdef LOG
				pan::log_DEBUG("Received msg size: ", pan::integer(incoming_type.size()));
#endif
			order_id_t oid;
			oid.set(static_cast<char*>(incoming_type.data()), incoming_type.size());
#ifdef LOG
			pan::log_DEBUG("Received reply for oid: ", oid.c_str(oidbuf));	
#endif
				//////socket.recv(&incoming, 0); 
				//////pan::log_DEBUG("Received msg size: ", pan::integer(incoming.size()));
				zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &more_size);
			} while (more);
			rcvcount++;
			break; // outer while...
		}
	}
	}

	std::cerr << "Received: " << rcvcount << "\n";	
	////}


	boost::posix_time::ptime stop_ptime(boost::posix_time::microsec_clock::local_time()); 
	boost::posix_time::time_duration ptime_duration(stop_ptime - start_ptime); 
	std::cerr << ptime_duration << "\n";
	socket.close();
	//zmq_term(ctx);

}




