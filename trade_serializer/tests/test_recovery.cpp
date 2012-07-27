#include <gtest/gtest.h>
#include <zmq.hpp>
#include "proto/execution_report.pb.h"
#include "logging.h"
#include "timing.h"
#include "order.h"
#include "venue_globals.h"
#include "msg_types.h"

const char* const STRATEGY_ID = "060d3dd2-6787-426b-9544-d4b7996651e4";

void
sendMessage(zmq::socket_t& socket, 
        strategy_id_t& strategy_id, 
        capk::msg_t msg_type)
{
        zmq::message_t strategy_id_msg(strategy_id.uuid(), strategy_id.size(), NULL, NULL);
        pan::log_DEBUG("Sending strategy id: ", pan::blob(strategy_id.get_uuid(), strategy_id.size()));
        socket.send(strategy_id_msg, ZMQ_SNDMORE);

        // send message type
        zmq::message_t msg_type_msg(sizeof(msg_type));
        *(static_cast<int*>(msg_type_msg.data())) = msg_type;
        socket.send(msg_type_msg, 0);

}


int 
main(int argc, char** argv)
{
    zmq::context_t ctx(1);
    zmq::socket_t recovery_socket(ctx, ZMQ_REQ);
    recovery_socket.connect(capk::kRECOVERY_LISTENER_ADDR);


    strategy_id_t strategy_id;
    strategy_id.parse(STRATEGY_ID);

    sendMessage(recovery_socket, strategy_id, capk::POSITION_REQ);
    zmq::message_t positionMsg;
    recovery_socket.recv(&positionMsg, 0);
    pan::log_DEBUG("Recovery replied with: ", pan::blob(positionMsg.data(), positionMsg.size()));
    
    sendMessage(recovery_socket, strategy_id, capk::OPEN_ORDER_REQ);
    zmq::message_t openOrderMsg;
    recovery_socket.recv(&openOrderMsg, 0);
    pan::log_DEBUG("Recovery replied with: ", pan::blob(openOrderMsg.data(), openOrderMsg.size()));

    recovery_socket.close();

}
