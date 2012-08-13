import zmq
import sys
import ConfigParser
import os.path
import proto_objs.venue_configuration_pb2
from optparse import OptionParser

full_config = proto_objs.venue_configuration_pb2.configuration()


def parse(filename):
    config = ConfigParser.ConfigParser()
    config.read(filename)
    sections =  config.sections()
    full_config.Clear()
    for s in sections:
         if s == 'global':
             full_config.trade_serialization_addr  = config.get(s, 'trade_serialization_addr')
             full_config.recovery_listener_addr  = config.get(s, 'recovery_listener_addr')
         else:
            single_venue_config = full_config.configs.add()
            make_protobuf(s, config, single_venue_config)
         
    #print full_config.__str__() 
    return True

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
    if os.path.exists(config_filename) == False:
        print "Config file: ", config_filename, " does not exist"
        raise Exception("Config file: ", config_filename, " does not exist")

    parse(config_filename)

    # Create context and connect
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    
    socket.bind("tcp://127.0.0.1:11111")
    
    while True:
        contents = socket.recv()
        print "Received msg:<", contents, ">"
        if contents == 'R':
            refresh_ret = parse(config_filename)
            if (refresh_ret == True): 
                refresh_status = "OK"
            else:
                refresh_status = "ERROR"
            socket.send_multipart(["REFRESH", refresh_status])
        elif contents == 'C':
            socket.send_multipart(["CONFIG", full_config.SerializeToString()])
        else:
            socket.send_multipart(["ERROR", "unknown message"])
    

if __name__ == "__main__":
    main()
