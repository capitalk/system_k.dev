import zmq

# Create context and connect
context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.connect("tcp://127.0.0.1:11111")

config_request_msg = 'C'
refresh_request_msg = 'R'

socket.send(config_request_msg)
config_reply = socket.recv()
print "Received reply:<", config_reply, ">"

socket.send(refresh_request_msg)
refresh_reply = socket.recv()
print "Received reply:<", refresh_reply, ">"


