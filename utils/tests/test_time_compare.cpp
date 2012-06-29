#include <iostream>
#include <string>

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

#include "gtest/gtest.h"

#include "time_utils.h"

#define NANOS_PER_SECOND 1000000000
#define TIME_STR_LEN 26+1

TEST(TimeTest, TimespecToDouble) 
{
    timespec ts;
    clock_getres(CLOCK_REALTIME, &ts);
    std::cout << "Timespec resolution: " << ts << std::endl;
    assert(ts.tv_nsec == 1 && ts.tv_sec == 0);
    clock_gettime(CLOCK_REALTIME, &ts);

    std::cout << "Timespec original: " << ts << std::endl;
    int64_t out;
    EXPECT_EQ(0, capk::timespec2int64_t(&ts, &out));
    std::cout << "Timespec as int64_t: " << out << std::endl;
    
    EXPECT_EQ(-1, capk::timespec2int64_t(&ts, NULL));
}

TEST(TimeTest, Delta)
{
	timespec ts1;
	timespec ts2;

	clock_gettime(CLOCK_MONOTONIC, &ts1);
	sleep(1);
	clock_gettime(CLOCK_MONOTONIC, &ts2);

	unsigned long delta_millis = capk::timespec_delta_millis(ts1, ts2);
	std::cout << "Milliseconds delta: " << delta_millis << std::endl;

	timespec delta_timespec = capk::timespec_delta(ts1, ts2);
	std::cout << "Timespec delta: " << delta_timespec << std::endl;

	EXPECT_GE(1000, delta_millis);
}


int 
main(int argc, char** argv)
{
	
	testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();
/*
	std::string in(argv[1]);

	std::cout << "Read: " << in << std::endl;
	size_t pos = in.find(':');
	if (pos == std::string::npos) {
		std::cerr << "Time string must contain one \':\'" << std::endl;
		return (-1);
	}
	std::string secstr(in, 0, pos);
	std::string nanostr(in, pos+1, in.size());

    timespec ts;
	ts.tv_sec = atoi(secstr.c_str());
	ts.tv_nsec = atoi(nanostr.c_str());
    printf("Using Seconds: %ld, Nanos: %ld\n", ts.tv_sec, ts.tv_nsec);
	char buf[TIME_STR_LEN];
	//std::cout << "Buffer len: " << sizeof(buf) << std::endl;
	std::cout << timespec2str(ts, buf, sizeof(buf)) << std::endl;

*/
	return result;
}
