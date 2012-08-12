import zmq
import sys
import ConfigParser
import proto_objs.venue_configuration_pb2
from optparse import OptionParser

def parse(filename):
    config = ConfigParser.ConfigParser()
    config.read(filename)
    sections =  config.sections()
    for i in sections:
         #print config.get(i, 'foo')
         print config.items(i)


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
