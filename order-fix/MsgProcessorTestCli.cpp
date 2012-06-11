#include "logging.h"
#include "timing.h"

#include <zmq.hpp>
#include <signal.h>


#include "google/dense_hash_map"

#include "proto/new_order_single.pb.h"
#include "proto/capk_globals.pb.h"
#include "proto/execution_report.pb.h"
#include "proto/order_cancel.pb.h"
#include "proto/order_cancel_reject.pb.h"
#include "proto/order_cancel_replace.pb.h"
#include "proto/spot_fx_md_1.pb.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>

#include <uuid/uuid.h>

#include "book_types.h"
#include "KMsgCache.h"
#include "KMsgTypes.h"
#include "ClientOrderInterface.h"
#include "ClientMarketDataInterface.h"
#include "OrderMux.h"
#include "MarketDataMux.h"
#include "utils/KTimeUtils.h"
#include "Order.h"
#include "KTypes.h"

// namespace stuff
using google::dense_hash_map;
namespace po = boost::program_options;

// Global vars
const char* STRATEGY_ID =  "7020f42e-b6c6-42d1-9b1e-65d968961a06";
strategy_id_t sid;

const char* ORDER_MUX = "inproc://order_mux";
const char* MD_MUX = "inproc://md_mux";
const int FXCM_ID = 890778;
const int XCDE_ID = 908239;
const int AGGREGATED_BOOK = 982132;

const char* FXCM_ORDER_INTERFACE_ADDR = "tcp://127.0.0.1:9999";
const char* XCDE_ORDER_INTERFACE_ADDR = "tcp://127.0.0.1:9998";

const char* AGGREGATED_BOOK_MD_INTERFACE_ADDR = "tcp://127.0.0.1:9000";


#define MAX_MSGSIZE 256

// Global zmq context
zmq::context_t ctx(1);

// Global sockets these are PAIRS and the only two endpoints into the strategy
// order entry socket
zmq::socket_t* pOEInterface;
// market data socket
zmq::socket_t* pMDInterface;

// Hash tables and typedefs for storing order states
typedef dense_hash_map<order_id_t, capk::Order, std::tr1::hash<order_id>, eq_order_id> order_map_t;
typedef order_map_t::iterator order_map_iter_t;
typedef std::pair<order_map_iter_t, bool> order_map_insert_t;
typedef std::pair<order_id_t, capk::Order> order_map_value_t;
order_map_t pendingOrders;
order_map_t workingOrders;
order_map_t completedOrders;	


// Signal handler setup for ZMQ
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


int 
snd_HELO(const int venueID, const strategy_id_t&) {
	// Send the HELO msg to set the route
/*
	int64_t more = 0;
	size_t more_size = sizeof(more);
*/
	bool rc;
	pan::log_DEBUG("Sending venueID");
	zmq::message_t iid(sizeof(venueID));
	memcpy(iid.data(), &venueID, sizeof(venueID));
	rc = pOEInterface->send(iid, ZMQ_SNDMORE);

	pan::log_DEBUG("sending HELO msg type");
	zmq::message_t msg_helo(sizeof(capk::STRATEGY_HELO));
	memcpy(msg_helo.data(), &capk::STRATEGY_HELO, sizeof(capk::STRATEGY_HELO));
	rc = pOEInterface->send(msg_helo, ZMQ_SNDMORE);

	pan::log_DEBUG("sending HELO msg body");
	zmq::message_t msg_sid(sid.size());
	memcpy(msg_sid.data(), sid.get_uuid(), sid.size());
	rc = pOEInterface->send(msg_sid, 0);
/*	
	pan::log_DEBUG("waiting for HELO ACK");
	zmq::message_t msg_helo_ack;
	rc = pOEInterface->recv(&msg_helo_ack, 0); 
	zmq_getsockopt(*pOEInterface, ZMQ_RCVMORE, &more, &more_size);
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
	ocr.set_ord_type(capkproto::LIM);

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
	rc = pOEInterface->send(iid, ZMQ_SNDMORE);

	// send the message type 
	capk::msg_t order_cancel_replace = capk::ORDER_REPLACE;
	zmq::message_t msgtype(sizeof(order_cancel_replace));
	memcpy(msgtype.data(), &order_cancel_replace, sizeof(order_cancel_replace));
	pan::log_DEBUG("CANCEL REPLACE: Sending message type: ", pan::integer(order_cancel_replace));
	rc = pOEInterface->send(msgtype, ZMQ_SNDMORE);
	assert(rc == true);

	// send the strategy ID
	zmq::message_t sidframe(sid.uuid(), UUID_LEN,  NULL, NULL);
	pan::log_DEBUG("CANCEL REPLACE: Sending strategyid: ", pan::blob(sidframe.data(), sidframe.size()));
	rc = pOEInterface->send(sidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the order ID
	//zmq::message_t oidframe(oid.uuid(), UUID_LEN,  NULL, NULL);
	zmq::message_t oidframe(UUID_LEN);
	memcpy(oidframe.data(), oid.uuid(), UUID_LEN);
	pan::log_DEBUG("CANCEL REPLACE: Sending orderid: ", pan::blob(oidframe.data(), oidframe.size()));
	rc = pOEInterface->send(oidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the data
	pan::log_DEBUG("CANCEL REPLACE: Sending cancel replace msg: ", pan::blob(msg.data(), msg.size()));
	rc = pOEInterface->send(msg, 0);
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
	rc = pOEInterface->send(iid, ZMQ_SNDMORE);

	// send the message type 
	capk::msg_t order_cancel = capk::ORDER_CANCEL;
	zmq::message_t msgtype(sizeof(order_cancel));
	memcpy(msgtype.data(), &order_cancel, sizeof(order_cancel));
	pan::log_DEBUG("CANCEL: Sending message type: ", pan::integer(order_cancel));
	rc = pOEInterface->send(msgtype, ZMQ_SNDMORE);
	assert(rc == true);

	// send the strategy ID
	zmq::message_t sidframe(sid.uuid(), UUID_LEN,  NULL, NULL);
	pan::log_DEBUG("CANCEL: Sending strategyid: ", pan::blob(sidframe.data(), sidframe.size()));
	rc = pOEInterface->send(sidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the order ID
	//zmq::message_t oidframe(oid.uuid(), UUID_LEN,  NULL, NULL);
	zmq::message_t oidframe(UUID_LEN);
	memcpy(oidframe.data(), oid.uuid(), UUID_LEN);
	pan::log_DEBUG("CANCEL: Sending orderid: ", pan::blob(oidframe.data(), oidframe.size()));
	rc = pOEInterface->send(oidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the data
	pan::log_DEBUG("CANCEL: Sending new order msg: ", pan::blob(msg.data(), msg.size()));
	rc = pOEInterface->send(msg, 0);
	assert(rc == true);
	pan::log_DEBUG("CANCEL: Msg sent");
}


capkproto::new_order_single
create_order(const char* symbol, 
				side_t side,
				double quantity,
				double price) 
{

	pan::log_DEBUG("create_order()");
	capkproto::new_order_single nos;
	nos.set_strategy_id(sid.get_uuid(), UUID_LEN);
	nos.set_symbol(symbol);
	if (side == BID) {
		nos.set_side(capkproto::BID);
	}
	else if (side == ASK) {
		nos.set_side(capkproto::ASK);
	}
	nos.set_order_qty(quantity);
	nos.set_price(price);

	nos.set_ord_type(capkproto::LIM);
	nos.set_time_in_force(capkproto::GFD);
	return nos;
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
	nos.set_ord_type(capkproto::LIM);

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
	rc = pOEInterface->send(iid, ZMQ_SNDMORE);

	// send the message type 
	capk::msg_t order_new = capk::ORDER_NEW;
	//zmq::message_t msgtype(&order_new_type, sizeof(order_new_type), NULL, NULL);
	zmq::message_t msgtype(sizeof(order_new));
	memcpy(msgtype.data(), &order_new, sizeof(order_new));
	pan::log_DEBUG("Sending message type: ", pan::integer(order_new));
	rc = pOEInterface->send(msgtype, ZMQ_SNDMORE);
	assert(rc == true);

	// send the strategy ID
	zmq::message_t sidframe(sid.uuid(), UUID_LEN,  NULL, NULL);
	pan::log_DEBUG("Sending strategyid: ", pan::blob(sidframe.data(), sidframe.size()));
	rc = pOEInterface->send(sidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the order ID
	//zmq::message_t oidframe(oid.uuid(), UUID_LEN,  NULL, NULL);
	zmq::message_t oidframe(UUID_LEN);
	memcpy(oidframe.data(), oid.uuid(), UUID_LEN);
	pan::log_DEBUG("Sending orderid: ", pan::blob(oidframe.data(), oidframe.size()));
	rc = pOEInterface->send(oidframe, ZMQ_SNDMORE);
	assert(rc == true);

	// send the data
	pan::log_DEBUG("Sending new order msg: ", pan::blob(msg.data(), msg.size()));
	rc = pOEInterface->send(msg, 0);
	assert(rc == true);
	pan::log_DEBUG("Msg sent");
}

void 
handleExecutionReport(capkproto::execution_report& er) 
{
    timespec ts;
    capk::Order order;
    order.set(er);
    bool isNewItem;
    capk::ord_status_t ordStatus = order.getOrdStatus();

    // There are three FIX tags that relay the status of an order
    // 1) ExecTransType (20)
    // 2) OrdStatus (39)
    // 3) ExecType (150)
    // Usually OrdStatus == ExecType but the devil lives where they are not
    // equal. For some order statuses they are always the same (e.g. NEW) 
    // so we don't check ExecType but others (e.g. PENDING_CANCEL) they may 
    // be different since the order may exists in more than one state (e.g
    // a fill while a cancel is pending). 
    // see fix-42-with_errata_2001050.pdf on http://fixprotocol.org for more info

    if (ordStatus == capk::ORD_STATUS_NEW) {
        order_id_t oid = order.getOid();
        assert(workingOrders.find(oid) == workingOrders.end());
        assert(pendingOrders.find(oid) != pendingOrders.end());

        order_map_insert_t insert = 
                workingOrders.insert(order_map_value_t(oid, order));
        isNewItem = insert.second;
        if (isNewItem) {
            pan::log_DEBUG("Added to working: ",
                        pan::blob(oid.get_uuid(), UUID_LEN));
        }
        size_t numPendingOrders = pendingOrders.erase(oid);
        pan::log_DEBUG("Remaining pending orders: ", pan::integer(numPendingOrders));

        clock_gettime(CLOCK_REALTIME, &ts); 
        pan::log_DEBUG("OID: ", 
                        pan::blob(oid.get_uuid(), UUID_LEN), 
                        " NEW ", 
                        pan::integer(ts.tv_sec), 
                        ":", 
                        pan::integer(ts.tv_nsec));
    }

    if (ordStatus == capk::ORD_STATUS_PARTIAL_FILL) {
        order_id_t oid = order.getOid();

        if (order.getExecType() == capk::EXEC_TYPE_REPLACE) {
            pan::log_NOTICE("OID: ", pan::blob(oid.get_uuid(), UUID_LEN), 
                    " replaced WHILE partial fill occurred");
        }
        order_map_iter_t orderIter = workingOrders.find(oid);
        assert(orderIter != workingOrders.end());
        (*orderIter).second = order;
        completedOrders.insert(order_map_value_t(oid, order));
        clock_gettime(CLOCK_REALTIME, &ts); 
        pan::log_DEBUG("OID: ", 
                        pan::blob(oid.get_uuid(), UUID_LEN), 
                        " PARTIAL FILL ", 
                        pan::integer(ts.tv_sec), 
                        ":", 
                        pan::integer(ts.tv_nsec));
    }

    if (ordStatus == capk::ORD_STATUS_FILL) {
       order_id_t oid = order.getOid();
       if (order.getExecType() == capk::EXEC_TYPE_REPLACE) {
           pan::log_NOTICE("OID: ", pan::blob(oid.get_uuid(), UUID_LEN),
                   " replaced AND fully filled");
       }
       pan::log_DEBUG("OID: ", 
                        pan::blob(oid.get_uuid(), UUID_LEN), 
                        " FILLED ", 
                        pan::integer(ts.tv_sec), 
                        ":", 
                        pan::integer(ts.tv_nsec));

       order_map_insert_t insert = 
       completedOrders.insert(order_map_value_t(oid, order)); 
       isNewItem = insert.second;
       if (isNewItem) {
           pan::log_DEBUG("Added to completed: ", 
                   pan::blob(oid.get_uuid(), UUID_LEN));
       }
    }

    if (ordStatus == capk::ORD_STATUS_CANCELLED) {
        order_id_t oid = order.getOrigClOid();
        clock_gettime(CLOCK_REALTIME, &ts); 
        pan::log_DEBUG("OID: ", 
                        pan::blob(oid.get_uuid(), UUID_LEN), 
                        " CANCELLED ", 
                        pan::integer(ts.tv_sec), 
                        ":", 
                        pan::integer(ts.tv_nsec));

        order_map_iter_t orderIter = workingOrders.find(oid);  
        if (orderIter == workingOrders.end()) {
            pan::log_WARNING("OID: ", 
                pan::blob(oid.get_uuid(), UUID_LEN), 
                " cancelled but not found in working orders");
        }

        order_map_iter_t pendingIter = pendingOrders.find(oid);
        if (pendingIter == pendingOrders.end()) {
            pan::log_WARNING("OID: ", 
                pan::blob(oid.get_uuid(), UUID_LEN), 
                " cancelled but not found in pending orders");
        }
         
    }

    if (ordStatus == capk::ORD_STATUS_REPLACE) {
        order_id_t oid = order.getOid();

        order_map_iter_t orderIter = workingOrders.find(oid);
        assert(orderIter != workingOrders.end());
        // order must be found in working orders
        (*orderIter).second = order;

        clock_gettime(CLOCK_REALTIME, &ts); 
        pan::log_DEBUG("OID: ", 
                        pan::blob(oid.get_uuid(), UUID_LEN), 
                        " REPLACE ", 
                        pan::integer(ts.tv_sec), 
                        ":", 
                        pan::integer(ts.tv_nsec));

 
    }

    if (ordStatus == capk::ORD_STATUS_PENDING_CANCEL) {
        order_id_t oid = order.getOrigClOid();
        // We had a partial fill while pending cancel - handle it
        if (order.getExecType() == capk::EXEC_TYPE_PARTIAL_FILL) {
            pan::log_NOTICE("OID: ", pan::blob(oid.get_uuid(), UUID_LEN), 
                    " partial fill while pending cancel");
            completedOrders.insert(order_map_value_t(oid, order));
        }
        order_map_insert_t insert = 
                pendingOrders.insert(order_map_value_t(oid, order));
        isNewItem = insert.second;
        if (isNewItem) {
            pan::log_DEBUG("Added to pending: ",
                        pan::blob(oid.get_uuid(), UUID_LEN));
        }
        clock_gettime(CLOCK_REALTIME, &ts); 
        pan::log_DEBUG("OID: ", 
                        pan::blob(oid.get_uuid(), UUID_LEN), 
                        " PENDING CANCEL (REALTIME) ", 
                        pan::integer(ts.tv_sec), 
                        ":", 
                        pan::integer(ts.tv_nsec));
            //(*((m.insert(value_type(k, data_type()))).first)).second
    }
    if (ordStatus == capk::ORD_STATUS_PENDING_REPLACE) {
        order_id_t oid = order.getOrigClOid();
        if (order.getExecType() == capk::EXEC_TYPE_PARTIAL_FILL) {
            pan::log_NOTICE("OID: ", pan::blob(oid.get_uuid(), UUID_LEN), 
                    " partial fill while pending replace");
            completedOrders.insert(order_map_value_t(oid, order));
        }
        order_map_insert_t insert = 
                pendingOrders.insert(order_map_value_t(oid, order));
        isNewItem = insert.second;
        if (isNewItem) {
            pan::log_DEBUG("Added to pending: ",
                        pan::blob(oid.get_uuid(), UUID_LEN));
        }

        clock_gettime(CLOCK_REALTIME, &ts); 
        pan::log_DEBUG("OID: ", 
                        pan::blob(oid.get_uuid(), UUID_LEN), 
                        " PENDING REPLACE (REALTIME) ", 
                        pan::integer(ts.tv_sec), 
                        ":", 
                        pan::integer(ts.tv_nsec));
 
    }

    if (ordStatus == capk::ORD_STATUS_REJECTED) {
        order_id_t oid = order.getOid();
        pan::log_WARNING("OID: ", pan::blob(oid.get_uuid(), UUID_LEN),
                " REJECTED - NOT IMPLEMENTED!!!!!!!!!!");
    }

    pan::log_DEBUG("Num pending orders: ", pan::integer(pendingOrders.size()));
    pan::log_DEBUG("Num working orders: ", pan::integer(workingOrders.size()));

}

void
handleOrderCancelReject(capkproto::order_cancel_reject& ocr) 
{
    pan::log_DEBUG("handleOrderCancelReject()"); 
    order_id_t oid;
    oid.set(ocr.orig_cl_order_id().c_str(), ocr.orig_cl_order_id().size());
    order_map_iter_t orderIter = pendingOrders.find(oid);
    if (orderIter == pendingOrders.end()) {
        pan::log_DEBUG("OID: ", 
                pan::blob(oid.get_uuid(), UUID_LEN), 
                " not found in pending orders");
    }
    else {
        pendingOrders.erase(orderIter);
    }
    pan::log_DEBUG("Num pending orders: ", pan::integer(pendingOrders.size()));
    pan::log_DEBUG("Num working orders: ", pan::integer(workingOrders.size()));
}

bool
receiveBBOMarketData(zmq::socket_t* sock)
{
    capkproto::instrument_bbo bbo;
    zmq::message_t tickmsg;
    assert(sock);
    bool rc;
    pan::log_DEBUG("receiveBBOMarketData()");
    rc = sock->recv(&tickmsg, ZMQ_NOBLOCK);
    assert(rc);
    bbo.ParseFromArray(tickmsg.data(), tickmsg.size());
    capk::BBO2_t bbo_book;
    //pan::log_DEBUG(bbo.DebugString());

    // TODO FIX THIS to be int id for mic rather than string	
    if (bbo.symbol() == "EUR/USD") {
        pan::log_DEBUG("Received market data - ", bbo.symbol());
        bbo_book.bid_venue = bbo.bb_mic();
        bbo_book.bid_price = bbo.bb_price();
        bbo_book.bid_size = bbo.bb_size();
        clock_gettime(CLOCK_MONOTONIC, &bbo_book.bid_last_update);

        // TODO FIX THIS to be int id for mic rather than string	
        bbo_book.ask_venue = bbo.ba_mic();
        bbo_book.ask_price = bbo.ba_price();
        bbo_book.ask_size = bbo.ba_size();
        clock_gettime(CLOCK_MONOTONIC, &bbo_book.ask_last_update);
        return true;
    }
    return false;
}

bool
receiveOrder(zmq::socket_t* sock) 
{
    zmq::message_t msgtypeframe;
    zmq::message_t msgframe;
    bool rc;
    rc = sock->recv(&msgtypeframe, 0);
    if (*(static_cast<capk::msg_t*>(msgtypeframe.data())) == capk::EXEC_RPT) {
        bool parseOK;
        pan::log_DEBUG("Received msg type: ", pan::integer(capk::EXEC_RPT), " - capk::EXEC_RPT");
        capkproto::execution_report er;
        parseOK = er.ParseFromArray(msgframe.data(), msgframe.size());
        assert(parseOK);
        pan::log_DEBUG(er.DebugString());
        handleExecutionReport(er); 
    }
    if (*(static_cast<capk::msg_t*>(msgtypeframe.data())) == capk::ORDER_CANCEL_REJ) {
        bool parseOK;
        pan::log_DEBUG("Received msg type: ", pan::integer(capk::ORDER_CANCEL_REJ), " - capk::ORDER_CANCEL_REJ");
        capkproto::order_cancel_reject ocr;
        parseOK = ocr.ParseFromArray(msgframe.data(), msgframe.size());
        assert(parseOK);
        pan::log_DEBUG(ocr.DebugString());
        handleOrderCancelReject(ocr);
    }
}

int
main(int argc, char **argv)
{
	s_catch_signals();
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	assert(sid.set(STRATEGY_ID) == 0);
	int zero = 0;

	logging_init("testcli.log");

    // program options
    bool runInteractive = false;
    po::options_description desc("Allowed options");
    desc.add_options() 
        ("help", "this msg")
        ("i", "interactive mode - no MARKET DATA")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("i")) {
        pan::log_NOTICE("Running interactive");
        runInteractive = true;
    }
    ///////////////////////////////////////////////////////////////////////////
    // ORDER INTERFACE SETUP
    ///////////////////////////////////////////////////////////////////////////
    // Set the empty keys for storing orders in dense_hash_map
	order_id oidEmpty("");
    pendingOrders.set_empty_key(oidEmpty);
    workingOrders.set_empty_key(oidEmpty);
    completedOrders.set_empty_key(oidEmpty);

    // create the market mux and add order interfaces
	OrderMux omux(&ctx, 
				 ORDER_MUX);

	ClientOrderInterface oif_FXCM(FXCM_ID, 
								&ctx, 
								FXCM_ORDER_INTERFACE_ADDR,	
								ORDER_MUX);

	//ClientOrderInterface if_XCDE(XCDE_ID, 
								//&ctx, 
								//XCDE_ORDER_INTERFACE_ADDR,
								//ORDER_MUX);
	
	// add interfaces
	oif_FXCM.init();
	omux.addOrderInterface(&oif_FXCM);
	// run the order mux
	boost::thread* t0 = new boost::thread(boost::bind(&OrderMux::run, &omux));
	sleep(2);
	// connect the thread local pair socket for order data 
	pOEInterface = new zmq::socket_t(ctx, ZMQ_PAIR);
	pOEInterface->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
	assert(pOEInterface);
	pOEInterface->connect(ORDER_MUX);
	// send helo msg to each exchange we're connecting to
    // KTK TODO - SHOULD WAIT FOR ACK!!!!
	snd_HELO(FXCM_ID, sid); 
	//snd_HELO(XCDE_ID, sid); 
  
 
    
    
    ///////////////////////////////////////////////////////////////////////////
    // MARKET DATA INTERFACE SETUP
    ///////////////////////////////////////////////////////////////////////////

    if (runInteractive == false) {
    // create the market data mux
	MarketDataMux mdmux(&ctx, 
						MD_MUX);
	// TODO differentiate between bbo stream and depth
	ClientMarketDataInterface mdif_AGG_BOOK(AGGREGATED_BOOK, 
								&ctx,
								AGGREGATED_BOOK_MD_INTERFACE_ADDR,
								MD_MUX);
	// add the interface				 
	mdif_AGG_BOOK.init();
	mdmux.addMarketDataInterface(&mdif_AGG_BOOK);
	// run the market data mux
	boost::thread* t1 = new boost::thread(boost::bind(&MarketDataMux::run, &mdmux));
    sleep(2);	
    // connect the thread local pair socket for market data
	pMDInterface = new zmq::socket_t(ctx, ZMQ_PAIR);
	pMDInterface->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
	assert(pMDInterface);
	pMDInterface->connect(MD_MUX);
    bool rc;
    int ret;
  
    // setup items to poll - only two endpoint pair sockets 
    zmq::pollitem_t pollItems[] = {
        /* { socket, fd, events, revents} */
        {*pMDInterface, NULL, ZMQ_POLLIN, 0},
        {*pOEInterface, NULL, ZMQ_POLLIN, 0}
    };


    // start the polling loop
	while (1 && s_interrupted != 1) {
        pan::log_DEBUG("Polling pair sockets in app thread");
        ret = zmq::poll(pollItems, 2, -1);
        // receive market data
        if (pollItems[0].revents && ZMQ_POLLIN) {
            pan::log_DEBUG("RECEIVING MARKET DATA");
            receiveBBOMarketData(pMDInterface);
	    }
        else if (pollItems[1].revents && ZMQ_POLLIN) {
            pan::log_DEBUG("RECEIVING ORDER DATA");
            receiveOrder(pOEInterface);
        }

    }
    }
    else {

/*	For market data when you hook that shit up... */

	//boost::thread* md_thread = new boost::thread(boost::bind(&MarketDataMux::run, &mdmux));
	//pMDInterface = new zmq::socket_t(ctx, ZMQ_PAIR);
	//assert(pMDInterface);
	//pMDInterface->connect(MD_MUX);
	// get all data - i.e. no filter
	//const char* filter = "EUR/USD"; 
	//pMDInterface->setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
	//pMDInterface->connect("tcp://127.0.0.1:9000");

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
		action = 0;
	}
    }
	//omux.stop();
	//mdmux.stop();

	//pOEInterface->close();
	//pMDInterface->close();

}




