import zmq
import sys
import ConfigParser
import os.path
import proto_objs.venue_configuration_pb2
import daemon
import signal
import lockfile
from optparse import OptionParser
import datetime

full_config = proto_objs.venue_configuration_pb2.configuration()

bind_addr="tcp://127.0.0.1:11111"

def parse(filename):
    config = ConfigParser.ConfigParser()
    config.read(filename)
    sections =  config.sections()
    full_config.Clear()
    i = 0
    for s in sections:
         if s == 'global':
             full_config.trade_serialization_addr  = config.get(s, 'trade_serialization_addr')
             full_config.recovery_listener_addr  = config.get(s, 'recovery_listener_addr')
             full_config.aggregated_bbo_book_addr  = config.get(s, 'aggregated_bbo_book_addr')
             full_config.aggregated_bbo_book_id  = config.getint(s, 'aggregated_bbo_book_id')
         else:
            i+=1
            print ("Adding venue: %d " % i)
            single_venue_config = full_config.configs.add()
            make_protobuf(s, config, single_venue_config)
         
    print full_config.__str__() 
    return True

def make_protobuf(section, config, single_venue_config):
    single_venue_config.venue_id = config.getint(section, 'venue_id')
    single_venue_config.mic_name = config.get(section, 'mic_name')
    single_venue_config.order_interface_addr = config.get(section, 'order_interface_addr')
    single_venue_config.order_ping_addr = config.get(section, 'order_ping_addr')
    single_venue_config.market_data_broadcast_addr = config.get(section, 'market_data_broadcast_addr')
    if config.has_option(section, 'use_synthetic_cancel_replace'):
        single_venue_config.use_synthetic_cancel_replace = config.getboolean(section, 'use_synthetic_cancel_replace')

def run(config_filename):

    # Create context and connect
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    
    socket.setsockopt(zmq.LINGER, 0)
    print "Binding to: ", bind_addr
    socket.bind(bind_addr)
    
    while True:
        contents = socket.recv()
        print datetime.datetime.now(), "Received msg:<", contents, ">"
        if contents == 'R':
            print "Refresh request"
            refresh_ret = parse(config_filename)
            if (refresh_ret == True): 
                refresh_status = "OK"
            else:
                refresh_status = "ERROR"
            socket.send_multipart(["REFRESH", refresh_status])
        elif contents == 'C':
            print "Config request"
            socket.send_multipart(["CONFIG", full_config.SerializeToString()])
        else:
            print "Unknown request - ERROR"
            socket.send_multipart(["ERROR", "unknown message"])
    
def terminate():
    print "Terminate"
    socket.close()
    context.close()

def main():
    parser = OptionParser(usage="usage: %prog [options] <config_filename>")
    parser.add_option("-D", "--daemon", 
            dest="runAsDaemon", 
            help="Run configuration server as daemon", 
            action="store_true", 
            default=False)
    (options, args) = parser.parse_args();

    if len(args) < 1:
        parser.error("Missing arguments")

    config_filename = args[0]
    log_filename = "configuration_server.log"
    log = open(log_filename, 'w+')
 
    print "Using config file: ", config_filename
    if os.path.exists(config_filename) == False:
        print "Config file: ", config_filename, " does not exist"
        raise Exception("Config file: ", config_filename, " does not exist")

    if options.runAsDaemon == True:
        context = daemon.DaemonContext(
                working_directory='.',
                umask=0o002,
                #pidfile=lockfile.FileLock('./configuration_server.pid'),
                stdout=log,
                stderr=log)
        #context.signal_map = {
            #signal.SIGTERM: 'terminate',
            #signal.SIGHUP: 'terminate',
            #signal.SIGUSR1: 'terminate',
                #}
        #with daemon.DaemonContext():
        with context:
            parse(config_filename)
            run(config_filename)
    else:
        parse(config_filename)
        run(config_filename)
    
if __name__ == "__main__":
        main()
