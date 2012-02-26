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

	pan::log_DEBUG("Sleeping...");
	//sleep(3);
	pan::log_DEBUG("starting");

	zmq::context_t ctx(1);

	zmq::socket_t socket(ctx, ZMQ_DEALER);

	socket.connect("tcp://127.0.0.1:9999");

	zmq::pollitem_t items[] = {
		//{ socket, NULL, ZMQ_POLLOUT, 0 },
		{ socket, NULL, ZMQ_POLLIN, 0 }
	};

	bool rc;
	boost::posix_time::ptime start_ptime(boost::posix_time::microsec_clock::local_time()); 
	////while (1) {
		////zmq::poll(items, 2, -1);	
		////sleep(1);

		//if (items[0].revents & ZMQ_POLLOUT) {
			pan::log_DEBUG("Sending message");
			char msgbuf[256];
			capitalk::new_order_single nos;
			uuid_t oid;	
			uuid_generate(oid);
			nos.set_order_id(oid, 16);			
			nos.set_symbol("EUR/USD");
			nos.set_side(capitalk::BID);
			nos.set_order_qty(100000);
			nos.set_order_type(capitalk::LIM);
			nos.set_price(1);
			nos.set_time_in_force(capitalk::GFD);
			// nos.set_account("FOOBAR");
			size_t msgsize = nos.ByteSize();
			assert(msgsize < sizeof(msgbuf));
			nos.SerializeToArray(msgbuf, msgsize);	
			zmq::message_t msg(msgsize);
			memcpy(msg.data(), msgbuf, msgsize);
			int order_new_type = ORDER_NEW;

			zmq::message_t msgtype(&order_new_type, sizeof(order_new_type), NULL, NULL);
			pan::log_DEBUG("Sending type: ", pan::integer(ORDER_NEW));
			rc = socket.send(msgtype, ZMQ_SNDMORE);
			assert(rc == true);

			pan::log_DEBUG("Sending message ");
			rc = socket.send(msg, 0);
			assert(rc == true);
		//}
	while (1) {
		zmq::poll(items, 1, -1);	
		if (items[0].revents & ZMQ_POLLIN) {
			pan::log_DEBUG("OK to recv");
			int64_t more = 0;
			size_t more_size = sizeof(more);
		
			do {
				pan::log_DEBUG("Receiving message");
				zmq::message_t incoming;
				socket.recv(&incoming, 0); 
				pan::log_DEBUG("Received msg size: ", pan::integer(incoming.size()));
				zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &more_size);
			} while (more);
			break; // outer while...
		}
	}

	////}


	boost::posix_time::ptime stop_ptime(boost::posix_time::microsec_clock::local_time()); 
	boost::posix_time::time_duration ptime_duration(stop_ptime - start_ptime); 
	std::cerr << ptime_duration << "\n";
	socket.close();
	//zmq_term(ctx);

}




