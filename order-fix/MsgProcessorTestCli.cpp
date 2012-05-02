#include "logging.h"
#include "timing.h"

#include <zmq.hpp>
#include <signal.h>

#include "utils/KTimeUtils.h"
#include "KMsgTypes.h"
#include "google/dense_hash_map"

#include "proto/new_order_single.pb.h"
#include "proto/capk_globals.pb.h"
#include "proto/execution_report.pb.h"
#include "proto/order_cancel.pb.h"
#include "proto/order_cancel_reject.pb.h"
#include "proto/order_cancel_replace.pb.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>

#include <uuid/uuid.h>

#include "KMsgCache.h"
#include "KMsgTypes.h"
#include "ClientOrderInterface.h"
#include "OrderMux.h"

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
snd_HELO(const int venueID, const strategy_id_t&) {
	// Send the HELO msg to set the route
	int64_t more = 0;
	size_t more_size = sizeof(more);
	bool rc;
	pan::log_DEBUG("Sending venueID");
	zmq::message_t iid(sizeof(venueID));
	memcpy(iid.data(), &venueID, sizeof(venueID));
	rc = poe_interface->send(iid, ZMQ_SNDMORE);

	pan::log_DEBUG("sending HELO msg type");
	zmq::message_t msg_helo(sizeof(capk::STRATEGY_HELO));
	memcpy(msg_helo.data(), &capk::STRATEGY_HELO, sizeof(capk::STRATEGY_HELO));
	rc = poe_interface->send(msg_helo, ZMQ_SNDMORE);

	pan::log_DEBUG("sending HELO msg body");
	zmq::message_t msg_sid(sid.size());
	memcpy(msg_sid.data(), sid.get_uuid(), sid.size());
	rc = poe_interface->send(msg_sid, 0);
/*	
	pan::log_DEBUG("waiting for HELO ACK");
	zmq::message_t msg_helo_ack;
	rc = poe_interface->recv(&msg_helo_ack, 0); 
	zmq_getsockopt(*poe_interface, ZMQ_RCVMORE, &more, &more_size);
	assert(more == 0);
	
	pan::log_DEBUG("rcvd for HELO ACK");
*/
	return 0;
}

capkproto::order_cancel
query_cancel()
{
	pan::log_DEBUG("query_cancel()");

	
	capkproto::order_cancel oc;
	oc.set_strategy_id(sid.get_uuid(), UUID_LEN);			

	// orig order id - i.e. order to cancel
	std::string str_origoid;
	std::cout << "CANCEL: Enter orig order id: " << std::endl;	
	std::cin >> str_origoid;
	order_id_t origoid;
	origoid.set(str_origoid.c_str());
	oc.set_orig_order_id(origoid.get_uuid(), UUID_LEN);
	//char origoidbuf[UUID_LEN + 1];
	//origoid.c_str(origoidbuf);
	

	// symbol
	std::string str_symbol;
	std::cout << "CANCEL: Enter symbol: " << std::endl;
	std::cin >> str_symbol;
	oc.set_symbol(str_symbol);

	// side	
	std::string str_side;
	std::cout << "CANCEL: Enter side: " << std::endl;
	std::cin >> str_side;
	if (str_side == "B" || str_side == "b" || str_side == "0") {
		oc.set_side(capkproto::BID);
	}
	if (str_side == "S" || str_side == "s" || str_side == "1") {
		oc.set_side(capkproto::ASK);
	}
	
	// quantity
	double d_quantity;
	std::cout << "CANCEL: Enter order quantity: " << std::endl;
	std::cin >> d_quantity;
	if (d_quantity > 0) {	
		oc.set_order_qty(d_quantity);
	}
	
	pan::log_DEBUG("CANCEL: Created message [", pan::integer(oc.ByteSize()), "]\n",  oc.DebugString(), "\n");

	return oc;
}

capkproto::order_cancel_replace
query_cancel_replace()
{
	pan::log_DEBUG("query_cancel_replace()");

	
	capkproto::order_cancel_replace ocr;
	ocr.set_strategy_id(sid.get_uuid(), UUID_LEN);			

	// orig order id - i.e. order to cancel and replace
	std::string str_origoid;
	std::cout << "CANCEL REPLACE: Enter orig order id: " << std::endl;	
	std::cin >> str_origoid;
	order_id_t origoid;
	origoid.set(str_origoid.c_str());
	ocr.set_orig_order_id(origoid.get_uuid(), UUID_LEN);

	// order type
	ocr.set_order_type(capkproto::LIM);

	// symbol
	std::string str_symbol;
	std::cout << "CANCEL REPLACE: Enter symbol: " << std::endl;
	std::cin >> str_symbol;
	ocr.set_symbol(str_symbol);

	// side	
	std::string str_side;
	std::cout << "CANCEL REPLACE: Enter side: " << std::endl;
	std::cin >> str_side;
	if (str_side == "B" || str_side == "b" || str_side == "0") {
		ocr.set_side(capkproto::BID);
	}
	if (str_side == "S" || str_side == "s" || str_side == "1") {
		ocr.set_side(capkproto::ASK);
	}

	// price 
	double d_price; 
	std::cout << "Enter order price: " << std::endl;
	std::cin >> d_price;
	ocr.set_price(d_price);
	
	// quantity
	double d_quantity;
	std::cout << "CANCEL REPLACE: Enter order quantity: " << std::endl;
	std::cin >> d_quantity;
	if (d_quantity > 0) {	
		ocr.set_order_qty(d_quantity);
	}
	
	pan::log_DEBUG("CANCEL REPLACE: Created message [", pan::integer(ocr.ByteSize()), "]\n",  ocr.DebugString(), "\n");

	return ocr;
}


void 
snd_ORDER_CANCEL_REPLACE(const capk::venue_id_t venueID, capkproto::order_cancel_replace& ocr) 
{
	bool rc;
	char msgbuf[MAX_MSGSIZE];

	// create an order id for this order
	order_id oid(true);
	char oidbuf[UUID_STRLEN + 1];
	pan::log_DEBUG("CANCEL REPLACE: Creating order id: ", oid.c_str(oidbuf));
	ocr.set_cl_order_id(oid.get_uuid(), UUID_LEN);	

	size_t msgsize = ocr.ByteSize();
	assert(msgsize < sizeof(msgbuf));
	ocr.SerializeToArray(msgbuf, msgsize);	

	zmq::message_t msg(msgsize);
	memcpy(msg.data(), msgbuf, msgsize);

	// send the interface id to the mux
	zmq::message_t iid(sizeof(venueID));
	memcpy(iid.data(), &venueID, sizeof(venueID));
	pan::log_DEBUG("CANCEL REPLACE: Sending venueID: ", pan::integer(venueID));
	rc = poe_interface->send(iid, ZMQ_SNDMORE);

	// send the message type 
	capk::msg_t order_cancel_replace = capk::ORDER_REPLACE;
	zmq::message_t msgtype(sizeof(order_cancel_replace));
	memcpy(msgtype.data(), &order_cancel_replace, sizeof(order_cancel_replace));
	pan::log_DEBUG("CANCEL REPLACE: Sending message type: ", pan::integer(order_cancel_replace));
	rc = poe_interface->send(msgtype, ZMQ_SNDMORE);
	assert(rc == true);

	// send the strategy ID
	zmq::message_t sidframe(sid.uuid(), UUID_LEN,  NULL, NULL);
	pan::log_DEBUG("CANCEL REPLACE: Sending strategyid: ", pan::blob(sidframe.data(), sidframe.size()));
	rc = poe_interface->send(sidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the order ID
	//zmq::message_t oidframe(oid.uuid(), UUID_LEN,  NULL, NULL);
	zmq::message_t oidframe(UUID_LEN);
	memcpy(oidframe.data(), oid.uuid(), UUID_LEN);
	pan::log_DEBUG("CANCEL REPLACE: Sending orderid: ", pan::blob(oidframe.data(), oidframe.size()));
	rc = poe_interface->send(oidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the data
	pan::log_DEBUG("CANCEL REPLACE: Sending cancel replace msg: ", pan::blob(msg.data(), msg.size()));
	rc = poe_interface->send(msg, 0);
	assert(rc == true);
	pan::log_DEBUG("CANCEL REPLACE: Msg sent");
}


void 
snd_ORDER_CANCEL(const capk::venue_id_t venueID, capkproto::order_cancel& oc) 
{
	bool rc;
	char msgbuf[MAX_MSGSIZE];

	// create an order id for this order
	order_id oid(true);
	char oidbuf[UUID_STRLEN + 1];
	pan::log_DEBUG("CANCEL: Creating order id: ", oid.c_str(oidbuf));
	oc.set_cl_order_id(oid.get_uuid(), UUID_LEN);	

	size_t msgsize = oc.ByteSize();
	assert(msgsize < sizeof(msgbuf));
	oc.SerializeToArray(msgbuf, msgsize);	

	zmq::message_t msg(msgsize);
	memcpy(msg.data(), msgbuf, msgsize);

	// send the interface id to the mux
	zmq::message_t iid(sizeof(venueID));
	memcpy(iid.data(), &venueID, sizeof(venueID));
	pan::log_DEBUG("CANCEL: Sending venueID: ", pan::integer(venueID));
	rc = poe_interface->send(iid, ZMQ_SNDMORE);

	// send the message type 
	capk::msg_t order_cancel = capk::ORDER_CANCEL;
	zmq::message_t msgtype(sizeof(order_cancel));
	memcpy(msgtype.data(), &order_cancel, sizeof(order_cancel));
	pan::log_DEBUG("CANCEL: Sending message type: ", pan::integer(order_cancel));
	rc = poe_interface->send(msgtype, ZMQ_SNDMORE);
	assert(rc == true);

	// send the strategy ID
	zmq::message_t sidframe(sid.uuid(), UUID_LEN,  NULL, NULL);
	pan::log_DEBUG("CANCEL: Sending strategyid: ", pan::blob(sidframe.data(), sidframe.size()));
	rc = poe_interface->send(sidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the order ID
	//zmq::message_t oidframe(oid.uuid(), UUID_LEN,  NULL, NULL);
	zmq::message_t oidframe(UUID_LEN);
	memcpy(oidframe.data(), oid.uuid(), UUID_LEN);
	pan::log_DEBUG("CANCEL: Sending orderid: ", pan::blob(oidframe.data(), oidframe.size()));
	rc = poe_interface->send(oidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the data
	pan::log_DEBUG("CANCEL: Sending new order msg: ", pan::blob(msg.data(), msg.size()));
	rc = poe_interface->send(msg, 0);
	assert(rc == true);
	pan::log_DEBUG("CANCEL: Msg sent");
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
snd_NEW_ORDER(const capk::venue_id_t venueID, capkproto::new_order_single& nos) 
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

	// send the interface id to the mux
	zmq::message_t iid(sizeof(venueID));
	memcpy(iid.data(), &venueID, sizeof(venueID));
	pan::log_DEBUG("Sending venueID: ", pan::integer(venueID));
	rc = poe_interface->send(iid, ZMQ_SNDMORE);

	// send the message type 
	capk::msg_t order_new = capk::ORDER_NEW;
	//zmq::message_t msgtype(&order_new_type, sizeof(order_new_type), NULL, NULL);
	zmq::message_t msgtype(sizeof(order_new));
	memcpy(msgtype.data(), &order_new, sizeof(order_new));
	pan::log_DEBUG("Sending message type: ", pan::integer(order_new));
	rc = poe_interface->send(msgtype, ZMQ_SNDMORE);
	assert(rc == true);

	// send the strategy ID
	zmq::message_t sidframe(sid.uuid(), UUID_LEN,  NULL, NULL);
	pan::log_DEBUG("Sending strategyid: ", pan::blob(sidframe.data(), sidframe.size()));
	rc = poe_interface->send(sidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the order ID
	//zmq::message_t oidframe(oid.uuid(), UUID_LEN,  NULL, NULL);
	zmq::message_t oidframe(UUID_LEN);
	memcpy(oidframe.data(), oid.uuid(), UUID_LEN);
	pan::log_DEBUG("Sending orderid: ", pan::blob(oidframe.data(), oidframe.size()));
	rc = poe_interface->send(oidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the data
	pan::log_DEBUG("Sending new order msg: ", pan::blob(msg.data(), msg.size()));
	rc = poe_interface->send(msg, 0);
	assert(rc == true);
	pan::log_DEBUG("Msg sent");
}


const char* ORDER_MUX = "inproc://order_mux";
const int FXCM_ID = 890778;
const int XCDE_ID = 908239;

int
main(int argc, char **argv)
{
	s_catch_signals();
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	assert(sid.set(STRATEGY_ID) == 0);

	logging_init("testcli.log");
/*
	int zero = 0;
	poe_interface = new zmq::socket_t(ctx, ZMQ_DEALER);
	poe_interface->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
	assert(poe_interface);
	poe_interface->connect("tcp://127.0.0.1:9999");
*/
	ClientOrderInterface if_FXCM(FXCM_ID, 
								&ctx, 
								"tcp://127.0.0.1:9999",
								ORDER_MUX);

	//ClientOrderInterface if_XCDE(FXCM_ID, 
								//&ctx, 
								//"tcp://127.0.0.1:9999",
								//ORDER_MUX);
	
	OrderMux mux(&ctx, 
				 ORDER_MUX);
	mux.addOrderInterface(&if_FXCM);
	
	boost::thread* t0 = new boost::thread(boost::bind(&OrderMux::run, &mux));
	int zero = 0;
	sleep(2);

	poe_interface = new zmq::socket_t(ctx, ZMQ_DEALER);
	poe_interface->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
	assert(poe_interface);
	poe_interface->connect(ORDER_MUX);
	snd_HELO(FXCM_ID, sid); 
	//snd_HELO(XCDE_ID, sid); 

/*	For market data when you hook that shit up...
	pmd_interface = new zmq::socket_t(ctx, ZMQ_SUB);
	assert(pmd_interface);
	// get all data - i.e. no filter
	const char* filter = ""; 
	pmd_interface->setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
	pmd_interface->connect("tcp://127.0.0.1:9000");
*/

	//t0->join();
	char action;
	while (s_interrupted != 1) {
		std::cout << "Action: " << std::endl;
		std::cin >> action;
		switch (action) {
			case 'n':
			{
				capkproto::new_order_single order =  query_order();
				snd_NEW_ORDER(FXCM_ID, order);
				break;
			}
			case 'c': 
			{
				capkproto::order_cancel cancel = query_cancel();
				snd_ORDER_CANCEL(FXCM_ID, cancel);
				break;
			}
			case 'r': 
			{
				capkproto::order_cancel_replace cancel_replace = query_cancel_replace();
				snd_ORDER_CANCEL_REPLACE(FXCM_ID, cancel_replace);
				break;
			}	
			case 'q': 
			{
				s_interrupted = 1;
				break;
			}
			default: 
				std::cerr << "Invalid action" << std::cerr;
		}
	}
	mux.stop();

	//poe_interface->close();
	//pmd_interface->close();

}




