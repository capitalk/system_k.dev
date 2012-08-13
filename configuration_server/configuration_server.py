import zmq
import sys
import ConfigParser
import proto_objs.venue_configuration_pb2
from optparse import OptionParser

full_config = proto_objs.venue_configuration_pb2.configuration()

def parse(filename):
    config = ConfigParser.ConfigParser()
    config.read(filename)
    sections =  config.sections()
    for s in sections:
         single_venue_config = full_config.configs.add()
         make_protobuf(s, config, single_venue_config)
         
    print full_config.__str__() 

def make_protobuf(section, config, single_venue_config):
    single_venue_config.venue_id = config.get(section, 'venue_id')
    single_venue_config.mic_name = config.get(section, 'mic_name')
    single_venue_config.order_interface_addr = config.get(section, 'order_interface_addr')
    single_venue_config.order_ping_addr = config.get(section, 'order_ping_addr')
    single_venue_config.market_data_broadcast_addr = config.get(section, 'market_data_broadcast_addr')

def main():
    parser = OptionParser(usage="usage: %prog [options] <config_filename>")
    (options, args) = parser.parse_args();

    if len(args) < 1:
        parser.error("Missing arguments")

    config_filename = args[0]
    print "Using config file: ", config_filename
    parse(config_filename)

    # Create context and connect
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    
    socket.bind("tcp://127.0.0.1:11111")
    
    while True:
        contents = socket.recv()
        print "Received msg:<", contents, ">"
        if contents == 'R':
            socket.send("REFRESH")
        elif contents == 'C':
            socket.send("CONFIG")
        else:
            socket.send("ERROR")
    

if __name__ == "__main__":
    main()
