#include "logging.h"
#include "timing.h"

#include <zmq.hpp>
#include <signal.h>

#include "utils/KTimeUtils.h"
#include "msg_types.h"
#include "google/dense_hash_map"

#include "proto/new_order_single.pb.h"
#include "proto/capk_globals.pb.h"
#include "proto/execution_report.pb.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <uuid/uuid.h>

#include "msg_cache.h"
#include "msg_cache.h"

using google::dense_hash_map;

const char* STRATEGY_ID =  "7020f42e-b6c6-42d1-9b1e-65d968961a06";
strategy_id_t sid;

#define MAX_MSGSIZE 256

static int s_interrupted = 0;

static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
}

void 
free_int(void* data, void* hint)
{
	if (data) {
		free(data);
	}
}
// global zmq context
zmq::context_t ctx(1);

// global sockets
// order entry socket
zmq::socket_t* poe_interface;
// market data socket
zmq::socket_t* pmd_interface;

int 
snd_HELO(const strategy_id_t&) {
	// Send the HELO msg to set the route
	int64_t more = 0;
	size_t more_size = sizeof(more);
	bool rc;
	pan::log_DEBUG("sending HELO");
	zmq::message_t msg_helo(sizeof(capk::STRATEGY_HELO));
	memcpy(msg_helo.data(), &capk::STRATEGY_HELO, sizeof(capk::STRATEGY_HELO));
	rc = poe_interface->send(msg_helo, ZMQ_SNDMORE);

	zmq::message_t msg_sid(sid.size());
	memcpy(msg_sid.data(), sid.get_uuid(), sid.size());
	rc = poe_interface->send(msg_sid, 0);
	
	pan::log_DEBUG("waiting for HELO ACK");
	zmq::message_t msg_helo_ack;
	rc = poe_interface->recv(&msg_helo_ack, 0); 
	zmq_getsockopt(*poe_interface, ZMQ_RCVMORE, &more, &more_size);
	assert(more == 0);
	
	pan::log_DEBUG("rcvd for HELO ACK");
}

void
snd_flood()
{
	const int NUM_TEST_MSGS = 1;
	bool rc;

	zmq::pollitem_t items[] = {
		//{ socket, NULL, ZMQ_POLLOUT, 0 },
		{ *poe_interface, NULL, ZMQ_POLLIN, 0 }
	};

	boost::posix_time::ptime start_ptime(boost::posix_time::microsec_clock::local_time()); 
	for (int i = 0; i < NUM_TEST_MSGS; i++) {
	//while (1) {
		////zmq::poll(items, 2, -1);	
		//sleep(1);

		//if (items[0].revents & ZMQ_POLLOUT) {
#ifdef LOG
			pan::log_DEBUG("BEGIN SND >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
			pan::log_DEBUG("Preparing message");
#endif
			char msgbuf[256];
			capkproto::new_order_single nos;
			nos.set_strategy_id(sid.get_uuid(), UUID_LEN);			
			order_id oid(true);
			char buf[UUID_STRLEN + 1];
#ifdef LOG
			pan::log_DEBUG("Creating order id: ", oid.c_str(buf));
#endif
			nos.set_order_id(oid.get_uuid(), UUID_LEN);	
			nos.set_symbol("EUR/USD");
			nos.set_side(capkproto::BID);
			nos.set_order_qty(10000);
			//nos.set_order_type(capkproto::LIM);
			nos.set_order_type('2');
			nos.set_price(1);
			//nos.set_time_in_force(capkproto::GFD);
			nos.set_time_in_force('0');
			// nos.set_account("FOOBAR");
#ifdef LOG
			pan::log_DEBUG(nos.DebugString());
#endif
			size_t msgsize = nos.ByteSize();
			assert(msgsize < sizeof(msgbuf));
			nos.SerializeToArray(msgbuf, msgsize);	
			zmq::message_t msg(msgsize);
			memcpy(msg.data(), msgbuf, msgsize);
			int order_new_type = capk::ORDER_NEW;

			zmq::message_t msgtype(&order_new_type, sizeof(order_new_type), NULL, NULL);
#ifdef LOG
			pan::log_DEBUG("Sending type: ", pan::integer(capk::ORDER_NEW));
#endif
			rc = poe_interface->send(msgtype, ZMQ_SNDMORE);
			assert(rc == true);

			zmq::message_t sidframe(sid.uuid(), UUID_LEN,  NULL, NULL);
#ifdef LOG
			pan::log_DEBUG("Sending sid: ", sid.c_str(buf));
#endif
			rc = poe_interface->send(sidframe, ZMQ_SNDMORE);
			assert(rc == true);


			zmq::message_t oidframe((void*)oid.get_uuid(), UUID_LEN,  NULL, NULL);
#ifdef LOG
			pan::log_DEBUG("Sending oid: ", oid.c_str(buf));
#endif
			rc = poe_interface->send(oidframe, ZMQ_SNDMORE);
			assert(rc == true);

#ifdef LOG
			pan::log_DEBUG("Sending message ");
#endif
			rc = poe_interface->send(msg, 0);
			assert(rc == true);
			//if (i % 1000 == 0) std::cerr << i << " Messages" << "\n";
		//}
			pan::log_DEBUG("END SND>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
	}

	int rcvcount = 0;
	//for (int i = 0; i < NUM_TEST_MSGS; i++) {
	while (1) {
		pan::log_DEBUG("BEGIN RCV<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
		zmq::poll(items, 1, -1);	
		if (items[0].revents & ZMQ_POLLIN) {
			int64_t more = 0;
			size_t more_size = sizeof(more);
			//char oidbuf[UUID_STRLEN + 1];	
			do {
				zmq::message_t msgtype;
				zmq::message_t oidframe;
				zmq::message_t data;
				// rcv first frame
				poe_interface->recv(&oidframe, 0); 
#ifdef LOG
				pan::log_DEBUG("Received msg: size=", pan::integer(oidframe.size()), "data=", pan::blob(static_cast<const void*>(oidframe.data()), oidframe.size()));
#endif
				order_id_t oid;
				oid.set(static_cast<char*>(oidframe.data()), oidframe.size());
/* this should be in while loop
				// rcv remaining frames
				poe_interface->recv(&data);	
				order_id_t oid2;
				oid2.set(static_cast<char*>(data.data()), data.size());
#ifdef LOG
				pan::log_DEBUG("Received msg: size=", pan::integer(oidframe.size()), "data=", pan::blob(static_cast<const void*>(oidframe.data(), oidframe.size())));
#endif
*/
				zmq_getsockopt(*poe_interface, ZMQ_RCVMORE, &more, &more_size);
			} while (more);
			rcvcount++;
			std::cerr << "Received: " << rcvcount << "\n";	
			pan::log_DEBUG("END RCV<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
		}
	}
	//}

	std::cerr << "Total msgs recieved: " << rcvcount << "\n";	
	////}


	boost::posix_time::ptime stop_ptime(boost::posix_time::microsec_clock::local_time()); 
	boost::posix_time::time_duration ptime_duration(stop_ptime - start_ptime); 
	std::cerr << ptime_duration << "\n";

}

void
main_event_loop()
{
	zmq::pollitem_t items[] = {
		{ *pmd_interface, NULL, ZMQ_POLLIN, 0},
		{ *poe_interface, NULL, ZMQ_POLLIN, 0 }
	};
	int rc;
	while (1) {
		rc = zmq::poll(items, 1, -1);	
		if (rc == -1) {
			pan::log_DEBUG("main_event_loop() poll returned -1");
		}
		// incoming message from market data
		if (items[0].revents & ZMQ_POLLIN) {

		}
		// incoming message from order interface
		if (items[1].revents & ZMQ_POLLIN) {
				
		}
	}
}

capkproto::new_order_single
query_order()
{
	pan::log_DEBUG("query_order()");

	
	capkproto::new_order_single nos;
	nos.set_strategy_id(sid.get_uuid(), UUID_LEN);			

	// symbol
	std::string str_symbol;
	std::cout << "Enter symbol: " << std::endl;
	std::cin >> str_symbol;
	nos.set_symbol(str_symbol);

	// side	
	std::string str_side;
	std::cout << "Enter side: " << std::endl;
	std::cin >> str_side;
	if (str_side == "B" || str_side == "b" || str_side == "0") {
		nos.set_side(capkproto::BID);
	}
	if (str_side == "S" || str_side == "s" || str_side == "1") {
		nos.set_side(capkproto::ASK);
	}
	
	// quantity
	double d_quantity;
	std::cout << "Enter order quantity: " << std::endl;
	std::cin >> d_quantity;
	if (d_quantity > 0) {	
		nos.set_order_qty(d_quantity);
	}
	
	// order type - LIMIT only for now
	nos.set_order_type(capkproto::LIM);

	// price 
	double d_price; 
	std::cout << "Enter order price: " << std::endl;
	std::cin >> d_price;
	nos.set_price(d_price);
	
	// set tif - GFD only for now
	nos.set_time_in_force(capkproto::GFD);

	// account 
/*
	std::string str_account; 
	std::cout << "Enter account: " << std::endl;
	std::cin >> str_account;
	nos.set_account(str_account);
*/

	pan::log_DEBUG("Created message [", pan::integer(nos.ByteSize()), "]\n",  nos.DebugString(), "\n");

	return nos;
}

void 
snd_NEW_ORDER(capkproto::new_order_single& nos) 
{
	bool rc;
	char msgbuf[MAX_MSGSIZE];

	// create an order id for this order
	order_id oid(true);
	char oidbuf[UUID_STRLEN + 1];
	pan::log_DEBUG("Creating order id: ", oid.c_str(oidbuf));
	nos.set_order_id(oid.get_uuid(), UUID_LEN);	

	size_t msgsize = nos.ByteSize();
	assert(msgsize < sizeof(msgbuf));
	nos.SerializeToArray(msgbuf, msgsize);	

	zmq::message_t msg(msgsize);
	memcpy(msg.data(), msgbuf, msgsize);
	int order_new_type = capk::ORDER_NEW;

	// send the message type 
	zmq::message_t msgtype(&order_new_type, sizeof(order_new_type), NULL, NULL);
	rc = poe_interface->send(msgtype, ZMQ_SNDMORE);
	assert(rc == true);

	// send the strategy ID
	zmq::message_t sidframe(sid.uuid(), UUID_LEN,  NULL, NULL);
	rc = poe_interface->send(sidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the order ID
	zmq::message_t oidframe((void*)oid.get_uuid(), UUID_LEN,  NULL, NULL);
	rc = poe_interface->send(oidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the data
	pan::log_DEBUG("Sending message ");
	rc = poe_interface->send(msg, 0);
	assert(rc == true);
	pan::log_DEBUG("Msg sent");
}

void 
rcv_RESPONSE()
{
	int64_t more = 0;
	size_t more_size = sizeof(more);
	pan::log_DEBUG("Entering recv loop");
	do {
		zmq::message_t msgtypeframe;
		poe_interface->recv(&msgtypeframe, 0); 
		pan::log_DEBUG("Received msgtypeframe: size=", 
						pan::integer(msgtypeframe.size()), 
						" data=", 
						pan::blob(static_cast<const void*>(msgtypeframe.data()), msgtypeframe.size()));

		zmq::message_t msgframe;
		poe_interface->recv(&msgframe, 0);
		pan::log_DEBUG("Received msgframe: size=", 
						pan::integer(msgframe.size()), 
						" data=", 
						pan::blob(static_cast<const void*>(msgframe.data()), msgframe.size()));

		if (*(static_cast<capk::msg_t*>(msgtypeframe.data())) == capk::EXEC_RPT) {
			bool parseOK;
			pan::log_DEBUG("Received msg type: ", pan::integer(capk::EXEC_RPT), " - capk::EXEC_RPT)");
			capkproto::execution_report er;
			parseOK = er.ParseFromArray(msgframe.data(), msgframe.size());
			pan::log_DEBUG(er.DebugString());
		}
		
		zmq_getsockopt(*poe_interface, ZMQ_RCVMORE, &more, &more_size);
	} while (more);
	pan::log_DEBUG("Exiting recv loop");
}

int
main(int argc, char **argv)
{
	s_catch_signals();
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	assert(sid.set(STRATEGY_ID) == 0);

	logging_init("testcli.log");
	int zero = 0;
	poe_interface = new zmq::socket_t(ctx, ZMQ_DEALER);
	//poe_interface->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
	assert(poe_interface);
	poe_interface->connect("tcp://127.0.0.1:9999");

	pmd_interface = new zmq::socket_t(ctx, ZMQ_SUB);
	assert(pmd_interface);
	// get all data - i.e. no filter
	const char* filter = ""; 
	pmd_interface->setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
	pmd_interface->connect("tcp://127.0.0.1:9000");

	snd_HELO(sid);
	capkproto::new_order_single order =  query_order();
	snd_NEW_ORDER(order);
	while (s_interrupted != 1) {
		rcv_RESPONSE();
	}
	//main_event_loop();
	
	//snd_flood();

	poe_interface->close();
	pmd_interface->close();

}




