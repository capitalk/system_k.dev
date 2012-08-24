
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/tokenizer.hpp>
#include <algorithm>
#include <iterator>



#include "order_book.h"
#include "order.h"
#include "limit.h"

#include "utils/types.h"
#include "utils/jenkins_hash.cpp"
#include "utils/time_utils.h"
#include "utils/fix_convertors.h"
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

using namespace std;

int 
main(int argc, char* argv[])
{
    std::string  x;
    cin >> x;

    uint32_t hash_val = hashlittle(x.c_str(), x.length(), 0);
   
    printf("%u", hash_val) ;
    
}

