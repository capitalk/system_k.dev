#include <gtest/gtest.h>
#include <zmq.hpp>
#include "proto/execution_report.pb.h"
#include "logging.h"
#include "timing.h"
#include "order.h"
#include "venue_globals.h"
#include "msg_types.h"

//const char* const TRADE_LISTENER_ADDRESS = "tcp://127.0.0.1:9898";
//const char* const TRADE_LISTENER_ADDRESS = "ipc:///tmp/trade_serializer";
const char* const STRATEGY_ID = "060d3dd2-6787-426b-9544-d4b7996651e4";
const char* const STR_CL_ORDER_ID_1 = "b666d23c-75a3-4f39-adfc-55e59b8e7602";
const char* const STR_ORIG_CL_ORDER_ID_1 = "b666d23c-75a3-4f39-adfc-55e59b8e7602";

const char* const SYMBOL = "EUR/USD";

const double ORDER_QTY = 1111111;
const capk::OrdType_t ORDER_TYPE = capkproto::LIM;
const capk::venue_id_t VENUE_ID = capk::kCAPK_VENUE_ID;
const double PRICE = 1.2123;
const char* ACCOUNT = "ACCOUNT";
const char* EXEC_ID = "EXEC_ID";
const capkproto::side_t SIDE = capkproto::BID;
const capk::TimeInForce_t TIF = capkproto::GFD;
const double LAST_SHARES=111112; // leaves 999999
const double CUM_QTY = LAST_SHARES; // if cum_qty == last_shares then we have only one execution
const char* FUT_SETT_DATE = "20120801";

capkproto::execution_report
createExecutionReport(order_id_t& cl_order_id, 
        order_id_t& orig_cl_order_id, 
        capk::OrdStatus_t order_status, 
        capk::ExecType_t exec_type)
{
    capkproto::execution_report er;

    er.set_cl_order_id(cl_order_id.get_uuid(), cl_order_id.size());

    er.set_orig_cl_order_id(orig_cl_order_id.get_uuid(), orig_cl_order_id.size());

    er.set_exec_id(EXEC_ID);

    er.set_exec_trans_type(capk::EXEC_TRAN_NEW);

    er.set_order_status(order_status);

    er.set_exec_type(exec_type);

    er.set_symbol(SYMBOL);

    er.set_security_type("FOR");

    er.set_side(SIDE);

    er.set_order_qty(ORDER_QTY);

    er.set_ord_type(ORDER_TYPE);

    er.set_price(PRICE);

    er.set_last_shares(LAST_SHARES);

    er.set_last_price(PRICE - 0.0001);

    er.set_leaves_qty(ORDER_QTY-CUM_QTY);
    
    er.set_cum_qty(CUM_QTY);

    er.set_avg_price(1.0000);

    er.set_time_in_force(TIF);
  
    FIX::UtcTimeStamp fixTimeNow; 
    er.set_transact_time(FIX::UtcTimeStampConvertor::convert(fixTimeNow, true));
    
    er.set_exec_inst("HANDL_INST");

    er.set_handl_inst(capk::HANDL_INST_AUTOMATED_NO_INTERVENTION);

    er.set_order_reject_reason(capk::REJECT_UNKNOWN_SYMBOL);

    er.set_min_qty(90000);

    er.set_venue_id(VENUE_ID);

    er.set_account(ACCOUNT);
   
    return er;
}

capkproto::new_order_single
createNewOrder(order_id_t& order_id)
{
    capkproto::new_order_single nos;
    nos.set_order_id(order_id.get_uuid(), order_id.size());

    strategy_id_t strategy_id(false);
    strategy_id.parse(STRATEGY_ID);
    nos.set_strategy_id(strategy_id.get_uuid(), strategy_id.size());

    nos.set_symbol(SYMBOL);
    nos.set_side(SIDE);
    nos.set_order_qty(ORDER_QTY); 
    nos.set_ord_type(ORDER_TYPE); 
    nos.set_price(PRICE);
    nos.set_time_in_force(TIF);
    nos.set_account(ACCOUNT);
    nos.set_venue_id(VENUE_ID);
    return nos;
}

capkproto::order_cancel
createOrderCancel(order_id_t& cl_order_id, 
        order_id_t& orig_cl_order_id)
{
    capkproto::order_cancel oc;
    strategy_id_t strategy_id(false);
    strategy_id.parse(STRATEGY_ID);
    oc.set_strategy_id(strategy_id.get_uuid(), strategy_id.size());
    oc.set_orig_order_id(orig_cl_order_id.get_uuid(), orig_cl_order_id.size());
    oc.set_cl_order_id(cl_order_id.get_uuid(), cl_order_id.size());
    oc.set_symbol(SYMBOL);
    oc.set_side(SIDE);
    oc.set_order_qty(ORDER_QTY);
    oc.set_fut_sett_date(FUT_SETT_DATE);
    return oc;
}

void
sendMessage(zmq::socket_t& socket, 
        strategy_id_t& strategy_id, 
        capk::msg_t msg_type,
        zmq::message_t& msg_body) 
{
        zmq::message_t strategy_id_msg(strategy_id.uuid(), strategy_id.size(), NULL, NULL);
        pan::log_DEBUG("Sending strategy id: ", pan::blob(strategy_id.get_uuid(), strategy_id.size()));
        socket.send(strategy_id_msg, ZMQ_SNDMORE);

        // send message type
        zmq::message_t msg_type_msg(sizeof(msg_type));
        *(static_cast<int*>(msg_type_msg.data())) = msg_type;
        socket.send(msg_type_msg, ZMQ_SNDMORE);

        socket.send(msg_body, 0);
}


int 
main(int argc, char** argv)
{
    zmq::context_t ctx(1);
    zmq::socket_t pub_socket(ctx, ZMQ_DEALER);
    pub_socket.connect(capk::kTRADE_SERIALIZATION_ADDR);

    int insertCount = argc > 1 ? atoi(argv[1]) : 1;

    strategy_id_t strategy_id;
    strategy_id.parse(STRATEGY_ID);

    order_id_t cl_ord_id;
    cl_ord_id.parse(STR_CL_ORDER_ID_1);

    order_id_t orig_cl_ord_id;
    orig_cl_ord_id.parse(STR_ORIG_CL_ORDER_ID_1);
    
    capkproto::execution_report ertest = createExecutionReport(cl_ord_id, orig_cl_ord_id, capk::ORD_STATUS_FILL, capk::EXEC_TYPE_FILL);
    T0(a) 
    capk::Order o;
    o.set(ertest);
    TN(b);
    TDIFF(c, a, b);
    std::cerr << "Conversion time from execution report to order: " << c << std::endl; 
    
    T0(start)


        //////////////////////////////////////////////////////
        // NEW ORDER MSG
        // send strategy id
        capkproto::new_order_single nos = createNewOrder(cl_ord_id);

        // create message body
        zmq::message_t new_order_msg(nos.ByteSize());
        pan::log_DEBUG("Sending new order:\n ", nos.DebugString().c_str(), pan::integer(nos.ByteSize()), " bytes");
        nos.SerializeToArray(new_order_msg.data(), new_order_msg.size());
 
        sendMessage(pub_socket, strategy_id, capk::ORDER_NEW, new_order_msg);

        //////////////////////////////////////////////////////
        // EXEC RPT MSG - new execution
        capkproto::execution_report er = createExecutionReport(cl_ord_id, orig_cl_ord_id, capk::ORD_STATUS_PARTIAL_FILL, capk::EXEC_TYPE_PARTIAL_FILL);

        // create message body
        zmq::message_t execution_report_msg(er.ByteSize());
        pan::log_DEBUG("Sending execution report: ", pan::integer(er.ByteSize()), " bytes");
        er.SerializeToArray(execution_report_msg.data(), execution_report_msg.size());

        sendMessage(pub_socket, strategy_id, capk::EXEC_RPT, execution_report_msg);

        //////////////////////////////////////////////////////

        //////////////////////////////////////////////////////
        // ORDER CANCEL MSG - THIS SHOULD BE IGNORED BY SERIALIZER
        capkproto::order_cancel oc = createOrderCancel(cl_ord_id, orig_cl_ord_id);

        // create message body
        zmq::message_t order_cancel_msg(oc.ByteSize());
        pan::log_DEBUG("Sending execution report: ", pan::integer(oc.ByteSize()), " bytes");
        er.SerializeToArray(order_cancel_msg.data(), order_cancel_msg.size());
 
        sendMessage(pub_socket, strategy_id, capk::ORDER_CANCEL, order_cancel_msg);

        char x;
        std::cin >> x;
        //////////////////////////////////////////////////////
        // EXEC RPT MSG - cancellation should change order_state in table
        er = createExecutionReport(cl_ord_id, orig_cl_ord_id, capk::ORD_STATUS_CANCELLED, capk::EXEC_TYPE_CANCELLED);

        // create message body
        execution_report_msg.rebuild(er.ByteSize());
        pan::log_DEBUG("Sending execution report: ", pan::integer(er.ByteSize()), " bytes");
        er.SerializeToArray(execution_report_msg.data(), execution_report_msg.size());

        sendMessage(pub_socket, strategy_id, capk::EXEC_RPT, execution_report_msg);



    TN(stop)
    TDIFF(diff, start, stop)
    std::cout << "Elapsed time: " << diff << std::endl;
    pub_socket.close();

}
