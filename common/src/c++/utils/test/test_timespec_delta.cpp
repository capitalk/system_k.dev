#include <utils/timing.h>
#include <utils/time_utils.h>

#include <iostream>

/*
 * Build string 
 * g++ x.cpp /home/timir/capitalk/common/src/c++/utils/timing.cpp /home/timir/capitalk/common/src/c++/utils/time_utils.cpp -I /home/timir/capitalk/common/src/c++/ -lrt
*/
int
main()
{
    timespec t1, t2;
    timespec delta;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    delta = capk::timespec_delta(t1, t2);
    
    std::cout << "t1: " << t1 << std::endl;
    sleep(1);
    std::cout << "t2: " << t2 << std::endl;
    std::cout << "delta: " << delta << std::endl;
}
