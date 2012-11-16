#include "utils/logging.h"
#include "utils/timing.h"
#include "utils/config_server.h"
#include "gtest/gtest.h"
#include <zmq.h>

#include "proto/venue_configuration.pb.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#define ZC 1

void* ctx;

TEST(CacheTest, GetConfig) 
{
    capkproto::configuration all_configs;
    int x = capk::get_config_params(ctx, "tcp://127.0.0.1:11111", &all_configs);
    EXPECT_EQ(x, 0);
    pan::log_INFORMATIONAL(all_configs.DebugString().c_str());
    const char* test_mic = "FXCM.dev";
    pan::log_DEBUG("Requesting venue info for: ", test_mic);
    capkproto::venue_configuration my_config = 
        capk::get_venue_config(&all_configs, test_mic);

    pan::log_DEBUG("Got venue info:\n", my_config.DebugString());
    EXPECT_GT(my_config.DebugString().size(), 0);
}

int
main(int argc, char **argv)
{
	logging_init("test_config_server.log");
   
    ::testing::InitGoogleTest(&argc, argv);
    GOOGLE_PROTOBUF_VERIFY_VERSION; 

	ctx = zmq_init(1);
	boost::posix_time::ptime start_ptime(boost::posix_time::microsec_clock::local_time()); 
    
    int result = RUN_ALL_TESTS();
   	boost::posix_time::ptime stop_ptime(boost::posix_time::microsec_clock::local_time()); 
	boost::posix_time::time_duration ptime_duration(stop_ptime - start_ptime); 
	std::cerr << ptime_duration << "\n";
	zmq_term(ctx);

    return result;
}




