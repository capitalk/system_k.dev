#include <gtest/gtest.h>
#include <zmq.hpp>
#include "proto/execution_report.pb.h"
#include "logging.h"
#include "order.h"

//const char* const TRADE_LISTENER_ADDRESS = "tcp://127.0.0.1:9898";
const char* const TRADE_LISTENER_ADDRESS = "ipc:///tmp/trade_serializer";
const char* const STRATEGY_ID = "060d3dd2-6787-426b-9544-d4b7996651e4";

int 
main(int argc, char** argv)
{
    zmq::context_t ctx(1);
    zmq::socket_t pub_socket(ctx, ZMQ_DEALER);
    pub_socket.connect(TRADE_LISTENER_ADDRESS);

    strategy_id_t strategy_id;
    strategy_id.parse(STRATEGY_ID);
    

    for (int i = 0; i<1; i++) {

        capkproto::execution_report er;

        order_id_t cloid(true);
        er.set_cl_order_id(cloid.get_uuid(), cloid.size());

        order_id_t origcloid(true);
        er.set_orig_cl_order_id(origcloid.get_uuid(), origcloid.size());

        er.set_exec_id("EXEC ID HERE");

        er.set_exec_trans_type(capk::EXEC_TYPE_NEW);

        er.set_order_status(capk::ORD_STATUS_PARTIAL_FILL);

        er.set_exec_type(capk::EXEC_TYPE_PARTIAL_FILL);

        er.set_symbol("FOO/BAR");

        er.set_security_type("FOR");

        er.set_side(capkproto::BID);

        er.set_order_qty(100000);

        er.set_ord_type(capk::ORD_TYPE_MARKET);

        er.set_price(1.2345);

        er.set_last_shares(20000);

        er.set_last_price(1.234);

        er.set_leaves_qty(80000);
        
        er.set_cum_qty(100000);

        er.set_avg_price(1.2333);

        er.set_time_in_force(capk::TIF_DAY);
      
        FIX::UtcTimeStamp fixTimeNow; 
        er.set_transact_time(FIX::UtcTimeStampConvertor::convert(fixTimeNow, true));
        
        er.set_exec_inst("12A");

        er.set_handl_inst(capk::EXEC_INST_AON);

        er.set_order_reject_reason(capk::REJECT_UNKNOWN_SYMBOL);

        er.set_min_qty(90000);

        er.set_mic("CAPK");

        zmq::message_t strategy_id_msg(strategy_id.uuid(), strategy_id.size(), NULL, NULL);
        pan::log_DEBUG("Sending strategy id: ", pan::blob(strategy_id.get_uuid(), strategy_id.size()));
        pub_socket.send(strategy_id_msg, ZMQ_SNDMORE);
        zmq::message_t execution_report_msg(er.ByteSize());
        pan::log_DEBUG("Sending execution report: ", pan::integer(er.ByteSize()), " bytes");
        er.SerializeToArray(execution_report_msg.data(), execution_report_msg.size());
        pan::log_DEBUG("Execution report: \n", er.DebugString());
        pub_socket.send(execution_report_msg, 0);
    }

    pub_socket.close();

}
