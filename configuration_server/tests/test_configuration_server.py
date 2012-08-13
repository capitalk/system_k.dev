import sys
sys.path.append('..')
import zmq
import os.path
import proto_objs.venue_configuration_pb2


# Create context and connect
context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.connect("tcp://127.0.0.1:11111")

config_request_msg = 'C'
refresh_request_msg = 'R'

socket.send(config_request_msg)
(type, config_reply) = socket.recv_multipart()
configuration = proto_objs.venue_configuration_pb2.configuration()
configuration.ParseFromString(config_reply)

print configuration.trade_serialization_addr
print configuration.recovery_listener_addr

for venue_config in configuration.configs:
    print venue_config.mic_name
    print venue_config.venue_id
    print venue_config.order_interface_addr
    print venue_config.order_ping_addr
    print venue_config.market_data_broadcast_addr

#print configuration.__str__()
#print "Received reply:<", config_reply, ">"


socket.send(refresh_request_msg)
(type, refresh_reply) = socket.recv_multipart()
print "Received reply:<", refresh_reply, ">"


