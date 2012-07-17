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

#include "msg_cache.h"
#include "msg_types.h"

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

#if 0
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
#endif 

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

class ClientOrderInterface
{
	public:
		ClientOrderInterface(const int venueID,
							zmq::context_t* context, 
							const std::string& interfaceAddr, 
							const std::string& inprocAddr):
		_interfaceID(venueID),
		_context(context), 
		_interfaceAddr(interfaceAddr),
		_inprocAddr(inprocAddr),
		_stopRequested(false),
		_initComplete(false)
		{}
		
		~ClientOrderInterface();

		void init();
		//int run();
		//void stop();
		
		const std::string& getInterfaceAddr() const { return _interfaceAddr;}
		const std::string& getInproAddr() const { return _inprocAddr;}
		const int getInterfaceID() const { return _interfaceID;}
		zmq::socket_t* getInterfaceSocket() { return _interface;}
		zmq::socket_t* getInprocSocket() { return _inproc;}

	private:
		int _interfaceID;
		zmq::context_t* _context;

		std::string _interfaceAddr;
		zmq::socket_t* _interface;

		std::string _inprocAddr;
		zmq::socket_t* _inproc;

		volatile bool _stopRequested;
		int64_t _msgCount;
		bool _initComplete;

};

ClientOrderInterface::~ClientOrderInterface()
{

}

/*
void
ClientOrderInterface::stop()
{
	_stopRequested = true;
}
*/

void
ClientOrderInterface::init()
{
	/* 
	 * N.B.  
	 * Only connect the socket to the interface. Don't connect
	 * the inproc socket here since there will then be more than one DEALER
	 * connected to the inproc socket here and messages _received_
	 * from the interface will then be round-robined to these and 
	 * consequently not returned to the application thread
	 */
	if (_initComplete == false) {
		assert(_context != NULL);
		assert(_interfaceAddr.c_str() != NULL);
		try {
		_interface = new zmq::socket_t(*_context, ZMQ_DEALER);
		assert(_interface);
		_interface->connect(_interfaceAddr.c_str());
		}

		catch (std::exception& e) {
			pan::log_CRITICAL("EXCEPTION: ", __FILE__, pan::integer(__LINE__), " ", e.what());
		}
	}

	_initComplete = true;
}

/* 
 * Use run() for running in a separate thread only!
 */
/*
int 
ClientOrderInterface::run()
{
	try {
		init();

		assert(_context != NULL);
		assert(_interfaceAddr.c_str() != NULL);
		zmq::socket_t oe(*_context, ZMQ_DEALER);
		oe.connect(_interfaceAddr.c_str());
		
		_inproc = new zmq::socket_t(*_context, ZMQ_PULL);
		assert(_inproc);
		_inproc->connect(_inprocAddr.c_str());


		zmq::pollitem_t poll_items[] = {
			{ *_interface, NULL, ZMQ_POLLIN, 0},
			{ *_inproc, NULL, ZMQ_POLLIN, 0}
		};

		bool rc;
		int ret;	
		int64_t more;
		size_t more_size = sizeof(more);
		while (1 && _stopRequested == false) {
			ret = zmq::poll(poll_items, 2, -1);
			if (ret == -1) {
				return -1;
			}		
			// interface connection 
			// incoming messages forwarded to inproc socket
			if (poll_items[0].revents & ZMQ_POLLIN) {
				_msgCount++;	
				do {
					zmq::message_t msg;
					rc = _interface->recv(&msg, 0);
					assert(rc);
					_interface->getsockopt(ZMQ_RCVMORE, &more, &more_size);
					rc = _inproc->send(msg, more ? ZMQ_SNDMORE : 0);	
				} while (more);
			}
			// inproc connection 
			// incoming messages forwarded to oe
			if (poll_items[1].revents & ZMQ_POLLIN) {
				_msgCount++;	
				do {
					zmq::message_t msg;
					rc = _inproc->recv(&msg, 0);
					assert(rc);
					_inproc->getsockopt(ZMQ_RCVMORE, &more, &more_size);
					rc = _interface->send(msg, more ? ZMQ_SNDMORE : 0);	
				} while (more);
			}
		}		
	}
	catch(std::exception& e) {
		pan::log_CRITICAL("EXCEPTION: ", __FILE__, pan::integer(__LINE__), " ", e.what());
	}
	return 0;
}
*/

class OrderMux
{
	public: 
		OrderMux(zmq::context_t* context, 
				const std::string& inprocAddr):
				_context(context),
				_inprocAddr(inprocAddr),
				_oiArraySize(0),
				_stopRequested(false),
				_msgCount(0)
		{
		};

		~OrderMux();
		void addOrderInterface(ClientOrderInterface* oi) {
			_oiArray[_oiArraySize] = oi;	
			_oiArraySize++;
		}

		int run();
		void stop();

	private:
		void rcv_RESPONSE(zmq::socket_t* sock);
		// initializer list 
		zmq::context_t* _context;
		std::string _inprocAddr;
		size_t _oiArraySize;
		volatile bool _stopRequested;
		int64_t _msgCount;

		ClientOrderInterface* _oiArray[10];
		zmq::socket_t* _inproc;	// from strategy->venue
		

};

OrderMux::~OrderMux()
{
	if (_inproc) {
		delete _inproc;
	}		
	if (_oiArray) {
		delete [] _oiArray;
	}
}

void 
OrderMux::stop()
{
	_stopRequested = true;
}

int	
OrderMux::run()
{
	try {
		assert(_context != NULL);

		_inproc = new zmq::socket_t(*_context, ZMQ_DEALER);
		assert(_inproc);
		pan::log_DEBUG("Binding inproc addr: ", _inprocAddr.c_str());
		_inproc->bind(_inprocAddr.c_str());

		for (size_t i = 0; i<_oiArraySize; i++) {
			_oiArray[i]->init();					
		}
		
		// 0th item in poll_items is always inproc socket
		zmq::pollitem_t* poll_items = new zmq::pollitem_t[_oiArraySize + 1];
		
		poll_items[0].socket = *_inproc;
		poll_items[0].fd = NULL;
		poll_items[0].events = ZMQ_POLLIN;
		poll_items[0].revents = 0;
		
		for (size_t i = 0; i < _oiArraySize; i++) {
			poll_items[i+1].socket = *(_oiArray[i]->getInterfaceSocket());
			poll_items[i+1].fd = NULL;
			poll_items[i+1].events = ZMQ_POLLIN;
			poll_items[i+1].revents = 0;
		}
		
		bool rc = false;
		int ret = -1;	
		int64_t more = 0;
		size_t more_size = sizeof(more);

		while (1 && _stopRequested == false) {
			ret = zmq::poll(poll_items, _oiArraySize + 1, -1);
			if (ret == -1) {
				return -1;
			}		
			// outbound orders routed to correct venue 
			if (poll_items[0].revents & ZMQ_POLLIN) {
				_msgCount++;	
					// get the venue id so we can route
					zmq::message_t venue_id_msg;
					rc = _inproc->recv(&venue_id_msg, 0);
					assert(rc);
					// lookup the socket for the venue
					zmq::socket_t* venue_sock;
					int venue_id = *(static_cast<int*>(venue_id_msg.data()));
					pan::log_DEBUG("MUX received msg for iterface id: ", pan::integer(venue_id));

					size_t sockIdx;
					for (sockIdx = 0; sockIdx < _oiArraySize; sockIdx++) {
						if (_oiArray[sockIdx]->getInterfaceID() == venue_id) {
							venue_sock = _oiArray[sockIdx]->getInterfaceSocket();
							assert(venue_sock);
							pan::log_DEBUG("MUX found interface socket for id: ", pan::integer(venue_id));
						}
					}
					if (sockIdx > _oiArraySize) {
						pan::log_CRITICAL("MUX cant find interface for id: ", pan::integer(venue_id));
					}

				do {
					// recv and forward remaining frames 
					zmq::message_t msg;
					rc = _inproc->recv(&msg, 0);
					pan::log_DEBUG("MUX forwarding frame from inproc: ", pan::blob(msg.data(), msg.size()));
					assert(rc);		
					_inproc->getsockopt(ZMQ_RCVMORE, &more, &more_size);
					rc = venue_sock->send(msg, more ? ZMQ_SNDMORE : 0);	
				} while (more);
				pan::log_DEBUG("MUX finished forwarding");
			}
			else {	
			// messages returning from venue
			// don't need to be routed
				for (size_t j = 0; j<_oiArraySize; j++) {
					pan::log_DEBUG("MUX checking incoming messages on oiArray: ", pan::integer(j));
					if (poll_items[j+1].revents && ZMQ_POLLIN) {
						zmq::socket_t* sock = _oiArray[j]->getInterfaceSocket();
						assert(sock);
						_msgCount++;	
						rcv_RESPONSE(sock);
/*
						do {
							zmq::message_t msg;
							rc = sock->recv(&msg, 0);
							assert(rc);
							pan::log_DEBUG("MUX received from interface: ", pan::blob(msg.data(), msg.size()));
							sock->getsockopt(ZMQ_RCVMORE, &more, &more_size);
							rc = _inproc->send(msg, more ? ZMQ_SNDMORE : 0);	
						} while (more);
*/
					}
				}
			}		
		}
	}
	catch(std::exception& e) {
		pan::log_CRITICAL("EXCEPTION: ", __FILE__, pan::integer(__LINE__), " ", e.what());
	}	
	return 0;
}


void 
OrderMux::rcv_RESPONSE(zmq::socket_t* sock)
{
	int64_t more = 0;
	size_t more_size = sizeof(more);
	pan::log_DEBUG("Entering recv loop");
	do {
		zmq::message_t msgtypeframe;
		sock->recv(&msgtypeframe, 0); 
		pan::log_DEBUG("Received msgtypeframe: size=", 
						pan::integer(msgtypeframe.size()), 
						" data=", 
						pan::blob(static_cast<const void*>(msgtypeframe.data()), msgtypeframe.size()));

		zmq::message_t msgframe;
		sock->recv(&msgframe, 0);
		pan::log_DEBUG("Received msgframe: size=", 
						pan::integer(msgframe.size()), 
						" data=", 
						pan::blob(static_cast<const void*>(msgframe.data()), msgframe.size()));

		if (*(static_cast<capk::msg_t*>(msgtypeframe.data())) == capk::STRATEGY_HELO_ACK) {
			pan::log_DEBUG("Received msg type: ", pan::integer(capk::STRATEGY_HELO_ACK),
							" - capk::STRATEGY_HELO_ACK from venue ID: ",
							pan::integer(*(static_cast<capk::venue_id_t*>(msgframe.data()))));
		}
		
		if (*(static_cast<capk::msg_t*>(msgtypeframe.data())) == capk::EXEC_RPT) {
			bool parseOK;
			pan::log_DEBUG("Received msg type: ", pan::integer(capk::EXEC_RPT), " - capk::EXEC_RPT");
			capkproto::execution_report er;
			parseOK = er.ParseFromArray(msgframe.data(), msgframe.size());
			assert(parseOK);
			pan::log_DEBUG(er.DebugString());
		}
		
		zmq_getsockopt(*sock, ZMQ_RCVMORE, &more, &more_size);
		assert(more == 0);
	} while (more);
	pan::log_DEBUG("Exiting recv loop");
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

/*		
	pmd_interface = new zmq::socket_t(ctx, ZMQ_SUB);
	assert(pmd_interface);
	// get all data - i.e. no filter
	const char* filter = ""; 
	pmd_interface->setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
	pmd_interface->connect("tcp://127.0.0.1:9000");


*/

	//t0->join();
	//capkproto::new_order_single order;
	//capkproto::order_cancel cancel;
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
	//main_event_loop();
	
	//snd_flood();

	poe_interface->close();
	pmd_interface->close();

}




