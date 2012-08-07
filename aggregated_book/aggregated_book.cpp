
#include <iostream>
#include <string>
#include <exception>

#include <stdio.h>

#include <boost/program_options.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>

#include <zmq.hpp>

#include "utils/zhelpers.hpp"
#include "utils/time_utils.h"
#include "utils/bbo_book_types.h"
#include "utils/venue_utils.h"
#include "utils/venue_globals.h"
#include "utils/constants.h"
#include "utils/types.h"

#include "proto/spot_fx_md_1.pb.h"

namespace po = boost::program_options;

#define DBG 1
#define MAX_SYMBOLS 500
#define MSGBUF_SIZ 2048
#define UPDATE_TIMEOUT_MILLIS 5000

#define INIT_BID INT_MIN
#define INIT_ASK INT_MAX
#define INIT_SIZE -1

/*
 * Unrolled loops if we like
 */
// for mic info 
#define STRCPY5(dst, src) do { \
    dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; \
    dst[3] = src[3]; dst[4] = src[4]; \
} while(0)

// for pair name info
#define STRCPY8(dst, src) do { \
    dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; \
    dst[3] = src[3]; dst[4] = src[4]; dst[5] = src[5]; \
    dst[6] = src[6]; dst[7] = src[7]; \
} while(0)


#define SYMBOL_LEN 8
const char symbols[][SYMBOL_LEN] = {
    "EUR/USD", 
    "GBP/USD", 
    "USD/JPY", 
    "USD/CHF", 
    "USD/CAD", 
    "USD/ZAR", 
    "USD/NOK", 
    "AUD/USD", 
    "AUD/JPY", 
    "AUD/NZD", 
    "AUD/CAD", 
    "AUD/CHF", 
    "CAD/CHF", 
    "CAD/JPY", 
    "CHF/JPY", 
    "EUR/AUD", 
    "EUR/CAD", 
    "EUR/CHF", 
    "EUR/GBP", 
    "EUR/JPY", 
    "EUR/NZD", 
    "GBP/JPY", 
    "GBP/AUD", 
    "GBP/CAD", 
    "GBP/CHF", 
    "GBP/NZD", 
    "NZD/CAD", 
    "NZD/CHF", 
    "NZD/USD", 
    "NZD/JPY",
    "USD/HKD",
    "USD/SEK",
    "EUR/SEK",
    "EUR/CZK",
    "SGD/JPY", 
    "EUR/NOK",
    "USD/MXN",
    "EUR/ZAR", 
    "USD/HUF",
    "USD/PLN",
    "EUR/PLN",
    "GBP/DKK",
    "USD/DKK",
    "DKK/NOK",
    "NOK/SEK",
    "GBP/SEK",
    "GBP/ZAR",
    "USD/CZK"
};

#define SYMBOL_COUNT (sizeof(symbols) / SYMBOL_LEN)

/*
char mics[][MIC_LEN] = {
    "XCDE",
    "FXCM"
    "HSFX",
    "GAIN", 
    "FXCM"
};
*/

capk::venue_id_t venues[] = {
    capk::kXCDE_VENUE_ID,
    capk::kFXCM_VENUE_ID
};

#define VENUE_COUNT (sizeof(venues) / sizeof(capk::venue_id_t))

zmq::socket_t* bcast_sock;

struct instrument_info
{
    char symbol[SYMBOL_LEN];
    zmq::socket_t* broadcast_socket;
    capk::SingleMarketBBO_t venues[MAX_VENUES];
};

void instrument_reset(capk::SingleMarketBBO_t& bbo)  {
    bbo.bid_price = INIT_BID;
    bbo.ask_price = INIT_ASK;
    bbo.bid_size = INIT_SIZE;
    bbo.ask_size = INIT_SIZE;
}

bool instrument_is_unset(capk::SingleMarketBBO_t& bbo) {
    return (bbo.bid_price == INIT_BID &&
        bbo.ask_price == INIT_ASK &&
        bbo.bid_size == INIT_SIZE &&
        bbo.ask_size == INIT_SIZE);
}

bool isZeroTimespec(const struct timespec& ts) {
	return (ts.tv_sec == 0 && ts.tv_nsec == 0);
}

instrument_info instruments[MAX_SYMBOLS];

class book_manager
{
    public: 
        book_manager(zmq::context_t* context, const std::string& connectAddr): 
                _context(context), 
                _connectAddr(connectAddr), 
                _stopRequested(false) {};
        void run();
        void stop(); 
    private: 
        zmq::context_t* _context;
        std::string _connectAddr;
        volatile bool _stopRequested;
};

void 
book_manager::stop() {
    _stopRequested = true;
}

void 
book_manager::run() {
    try {
        zmq::socket_t receiver(*_context, ZMQ_SUB); 
        const char* filter = "";
        receiver.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
        receiver.bind(_connectAddr.c_str());
        fprintf(stdout, "book_manager: connect address: %s\n", _connectAddr.c_str());
        fprintf(stdout, "book_manager: stop requested: %d\n", _stopRequested);

        capkproto::instrument_bbo bbo;
        while (1 && _stopRequested == false) {
            // Extract the message from protobufs
            zmq::message_t msg;
            receiver.recv(&msg);
            bbo.ParseFromArray(msg.data(), msg.size());
            //char mic[MIC_LEN];
            //STRCPY5(mic, bbo.mic());
            capk::venue_id_t bid_venue_id = bbo.bid_venue_id();
            capk::venue_id_t ask_venue_id = bbo.ask_venue_id();
#ifdef DEBUG
            fprintf(stderr, "BID VENUE_ID: %d ASK VENUE_ID: %d", bid_venue_id, ask_venue_id);
#endif
            char sym[SYMBOL_LEN];
            STRCPY8(sym, bbo.symbol());
            double bid_size = bbo.bid_size();
            double ask_size = bbo.ask_size();
            double bid_price = bbo.bid_price();
            double ask_price = bbo.ask_price();
#ifdef DEBUG
            boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time());
#endif

            bool found_symbol = false;
            bool found_venue = false;

            // Add the price to the correct orderbook
            // That is, receive an update for a SINGLE symbol on a SINGLE mic
            // so just blindly update it in the book 
            for (unsigned int i = 0; i < SYMBOL_COUNT; i++) {
                if (strncmp(sym, instruments[i].symbol, SYMBOL_LEN) == 0) {
                    found_symbol = true;
                    for (unsigned int j = 0; j < VENUE_COUNT; j++) {
                        // update the specific book that the price came from
                        if (instruments[i].venues[j].venue_id != capk::kNULL_VENUE_ID && 
                            instruments[i].venues[j].venue_id == bid_venue_id &&                                instruments[i].venues[j].venue_id == ask_venue_id) {
                                found_venue = true;
                                instruments[i].venues[j].bid_price = bid_price; 
                                instruments[i].venues[j].ask_price = ask_price; 
                                instruments[i].venues[j].bid_size = bid_size; 
                                instruments[i].venues[j].ask_size = ask_size; 
                                clock_gettime(CLOCK_MONOTONIC, &instruments[i].venues[j].last_update);
                                break;
                        }
                    }
                } 
            }

            if (!found_symbol) {
                std::cerr << "No matching symbol(book_manager): " << sym << "\n";
            }
            if (!found_venue) {
                std::cerr << "No matching venue(book_manager): BID(" 
                                                               << bid_venue_id 
                                                               << ") ASK(" 
                                                               << ask_venue_id 
                                                               << ")" << "\n";
            }
			
			// Now go through all the books and pick the bbo
            // check the time since last update and reset if too long
            double bb = INT_MIN;
            double ba = INT_MAX;
            //char* bbmic = 0;
            //char* bamic = 0;
            double bbvol = 0;
            double bavol = 0;
            capk::venue_id_t bb_venue_id = capk::kNULL_VENUE_ID;
            capk::venue_id_t ba_venue_id = capk::kNULL_VENUE_ID;
            timespec now;
            std::string msg_str;
			unsigned long last_update_millis;

			// find the symbol and mic in the book structure
            for (unsigned int i = 0; i < SYMBOL_COUNT; i++) {
                if (strncmp(sym, instruments[i].symbol, SYMBOL_LEN) == 0) {
                    found_symbol = true;

                    for (unsigned int j = 0; j < VENUE_COUNT; j++) {
                        if(instruments[i].venues[j].venue_id != capk::kNULL_VENUE_ID) {
                            // check the elapsed time since last update
                            clock_gettime(CLOCK_MONOTONIC, &now);
                            last_update_millis = 
                                capk::timespec_delta_millis(instruments[i].venues[j].last_update, now);
                            // was the item ever updated? 
                            /*
							if (isZeroTimespec(instruments[i].mics[j].last_update)) {
								    fprintf(stderr, "<%s:%s>: NEVER UPDATED\n", 
                                        instruments[i].mics[j].MIC_name,
                                        instruments[i].symbol);
							}
							*/
							fprintf(stderr, "<%s:%s>: %f@%f (last update: %lu ms)", 
									capk::venueIdToMICCString(instruments[i].venues[j].venue_id),
									instruments[i].symbol,
									instruments[i].venues[j].bid_price, 
									instruments[i].venues[j].ask_price, 
									last_update_millis);
							if (last_update_millis > UPDATE_TIMEOUT_MILLIS) { 
								fprintf(stderr, "*** TIMEOUT ****");
								instrument_reset(instruments[i].venues[j]);
							}
							fprintf(stderr, "\n");
#ifdef DEBUG
							std::cerr << "Last update: " << instruments[i].venues[j].last_update << std::endl;
							std::cerr << "Now        : " << now << std::endl;
                            timespec tdelta = 
                                capk::timespec_delta(instruments[i].venues[j].last_update, now);
							std::cerr << "Tdelta     : " << tdelta << std::endl;
                            fprintf(stderr, "<%s:%s>: ms since last update: %lu\n", 
                                    instruments[i].symbol, 
                                    capk::venueIdToMICCString(instruments[i].venues[j].venue_id), 
									last_update_millis);
							fprintf(stderr, "<%s:%s>: %f@%f\n", 
									capk::venueIdToMICCString(instruments[i].venues[j].venue_id),
									instruments[i].symbol,
									instruments[i].venues[j].bid_price, 
									instruments[i].venues[j].ask_price);
#endif 
						}
                        
						if (instruments[i].venues[j].bid_price > bb) {
							bb = instruments[i].venues[j].bid_price;
							bb_venue_id = instruments[i].venues[j].venue_id;	
							bbvol = instruments[i].venues[j].bid_size; 
						} 
						if (instruments[i].venues[j].ask_price < ba) {
							ba = instruments[i].venues[j].ask_price;
							ba_venue_id = instruments[i].venues[j].venue_id;
							bavol = instruments[i].venues[j].ask_size; 
						} 
					}
				}
			}
            
            // Setup BBO and re-broadcast
            fprintf(stderr, "\n***\n<%s> BB: %s:%f-%f BA: %s:%f-%f\n***\n", 
                    sym, 
                    capk::venueIdToMICCString(bb_venue_id), 
                    bb, 
                    bbvol, 
                    capk::venueIdToMICCString(ba_venue_id), 
                    ba, 
                    bavol);

            capkproto::instrument_bbo ins_bbo;

            ins_bbo.set_symbol(sym);

            ins_bbo.set_bid_venue_id(bb_venue_id);
            ins_bbo.set_bid_price(bb);
            ins_bbo.set_bid_size(bbvol);

            ins_bbo.set_ask_venue_id(ba_venue_id);
            ins_bbo.set_ask_price(ba);
            ins_bbo.set_ask_size(bavol); 

            int msgsize = bbo.ByteSize();
            if (msgsize > MSGBUF_SIZ) {
                fprintf(stderr, "**\nins_bbo msg too large for buffer - DROPPING MSG!\n***");
                //s_send(*bcast_sock, "ERR");
                continue;
            }
            else {
                ins_bbo.SerializeToString(&msg_str);
#ifdef DEBUG 
                fprintf(stderr, "Sending %d bytes\n", msgsize);
                fprintf(stderr, "Sending %s \n", ins_bbo.DebugString().c_str());
#endif
            }

			s_sendmore(*bcast_sock, sym);
			s_send(*bcast_sock, msg_str);
#ifdef DEBUG 
            boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time());
            boost::posix_time::time_duration duration(time_end - time_start);
            fprintf(stderr, "BookUpdate Time(us) to recv and parse: %s\n", to_simple_string(duration).c_str());
#endif
        }
    }
    catch (std::exception& e) {
        std::cerr << "EXCEPTION: " << __FILE__ << __LINE__ << e.what();
    }
}

class worker
{   
    public:
        worker(zmq::context_t* context, const std::string& connectAddr, const std::string& inprocAddr): _context(context), _connectAddr(connectAddr), _inprocAddr(inprocAddr),  _stopRequested(false) {};
        void run();
        void stop();
        ~worker();
 
    private:
        zmq::context_t* _context;
        std::string _connectAddr;
        std::string _inprocAddr;
        zmq::socket_t* _inproc;
        volatile bool _stopRequested;
        
};

worker::~worker() {
    if (_inproc) delete _inproc;
}

void 
worker::stop() {
    _stopRequested = true;
}

void
worker::run () {
    try {
        assert(_context != NULL);
        assert(_connectAddr.c_str() != NULL);
        fprintf(stdout, "worker: connect address: %s\n",  _connectAddr.c_str());
        fprintf(stdout, "worker: stop requested: %d\n", _stopRequested);
        zmq::socket_t subscriber(*_context, ZMQ_SUB);
        subscriber.connect(_connectAddr.c_str());
        const char* filter = "";
        subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
        //capkproto::instrument_bbo bbo;
        
        _inproc = new zmq::socket_t(*_context, ZMQ_PUB);
        assert(_inproc);
        _inproc->connect(_inprocAddr.c_str());
        
        zmq::message_t msg;
        while (1 && _stopRequested == false) {
            subscriber.recv(&msg);
#ifdef DEBUG
            boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time());
#endif

			int64_t more;
			size_t more_size;
			more_size = sizeof(more);
			subscriber.getsockopt(ZMQ_RCVMORE, &more, &more_size);
            // send to orderbook on inproc socket - no locks needed according to zmq
            _inproc->send(msg, more ? ZMQ_SNDMORE : 0);

#ifdef DEBUG
            boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time());
            boost::posix_time::time_duration duration(time_end - time_start);
            fprintf(stderr, "Inbound from md interface time to receive and parse: %s\n", to_simple_string(duration).c_str()); 
#endif
        }
    } 
    catch (std::exception& e) {
        std::cerr << "EXCEPTION: " << __FILE__ << __LINE__ << e.what();
    }
}


void 
initialize_instrument_info(zmq::context_t* context)
{
    assert(context);

#ifdef DEBUG
    std::cerr << "Initializing instruments: " << sizeof(instruments)  << " bytes \n";
    std::cerr << "SYMBOL count: " << SYMBOL_COUNT << "\n"; 
    std::cerr << "VENUE count: " << VENUE_COUNT << "\n"; 
#endif

    for (unsigned int i=0; i < SYMBOL_COUNT; i++) {
        std::cerr << "Initializing symbol: <" << i << "> to <" << symbols[i] << ">\n";
        STRCPY8(instruments[i].symbol, symbols[i]);
        instruments[i].broadcast_socket = new zmq::socket_t(*context, ZMQ_PUB);  
        assert(instruments[i].broadcast_socket);

        for (unsigned int j=0; j < VENUE_COUNT; j++) {
            std::cerr << "Initializing venue: <" << j << "> to <" << venues[j] << ">\n";
            //STRCPY5(instruments[i].venues[j].MIC_name, venues[j]); 
			instruments[i].venues[j].venue_id = venues[j]; 
			instruments[i].venues[j].last_update = {0};
            instruments[i].venues[j].bid_price = INT_MIN;
            instruments[i].venues[j].ask_price = INT_MAX;
            instruments[i].venues[j].bid_size = -1;
            instruments[i].venues[j].ask_size = -1;
        }
    }

    // sanity check
#ifdef DEBUG
    for (unsigned int i = 0; i < SYMBOL_COUNT; i++) {
        for (unsigned int j = 0; j < VENUE_COUNT; j++) {
                    fprintf(stderr, "%s %s <%d>:%f@%f\n", 
                            instruments[i].symbol, 
                            capk::venueIdToMICCString(instruments[i].venues[j].venue_id),
                            instruments[i].venues[j].venue_id,
                            instruments[i].venues[j].bid_price, 
                            instruments[i].venues[j].ask_price);
        }
    }
#endif
}

const std::string AGGREGATE_OB = "inproc://ob";
//const char* XCDE_addr = "tcp://127.0.0.1:5271";
//const char* GAIN_addr= "tcp://127.0.0.1:5272";
//const char* FXCM_addr= "tcp://127.0.0.1:5273";
//const char* BCAST_OUT = "tcp://*:9000";


int 
main(int argc, char** argv)
{

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // make sure counts are less that allocated space 
    assert(VENUE_COUNT < MAX_VENUES);
    assert(SYMBOL_COUNT < MAX_SYMBOLS);

	// check program options
	// KTK - TODO - use config file  and symbol file
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("c", po::value<std::string>(), "<config file>")
            ("s", po::value<std::string>(), "<symbol file>")
            ("d", "debug info")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 1;
        }
        if (vm.count("c")) {
            std::cout << "Config file: " << vm["c"].as<std::string>() << ".\n";
            std::string configFile = vm["c"].as<std::string>();
        } else {
            // set default
            std::cerr << "Config file was not set \n";
        }

    }
    catch (std::exception& e) {
        std::cerr << "EXCEPTION: " << __FILE__ << __LINE__ << e.what();
        return (-1);
    }

	// program options and init are OK - now start sockets and threads
    try { 
		// Global context for ZMQ
		zmq::context_t context(1);
		
		initialize_instrument_info(&context);
		bcast_sock = new zmq::socket_t(context, ZMQ_PUB);
        assert(bcast_sock);
        int zero = 0;
        bcast_sock->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
        std::cerr << "Attempting to bind to: " << capk::kCAPK_AGGREGATED_BOOK_MD_INTERFACE_ADDR << ". If this does not return then the socket is likely in use" << std::endl;
		bcast_sock->bind(capk::kCAPK_AGGREGATED_BOOK_MD_INTERFACE_ADDR);
        std::cerr << "Bind complete." << std::endl;

        // connect to inproc socket for orderbook
        book_manager ob(&context, AGGREGATE_OB);
        boost::thread* t0 = new boost::thread(boost::bind(&book_manager::run, &ob));

        // subscribers
        worker w1(&context, capk::kXCDE_BROADCAST_ADDR, AGGREGATE_OB);
        worker w2(&context, capk::kFXCM_BROADCAST_ADDR, AGGREGATE_OB);
        boost::thread* t1 = new boost::thread(boost::bind(&worker::run, &w1));
        boost::thread* t2 = new boost::thread(boost::bind(&worker::run, &w2));
        t0->join();
        t1->join();
        t2->join();
        google::protobuf::ShutdownProtobufLibrary();
    } 
    catch(std::exception& e) {
        std::cerr << "EXCEPTION: " << __FILE__ << __LINE__ << e.what();
        return (-1);
    }
}
