

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/tokenizer.hpp>
#include <algorithm>
#include <iterator>
#include "utils/hash.cpp"

using namespace std;

int 
main(int argc, char* argv[])
{
    std::string x;
    cin >> x;

    
    uint32_t hashValue = hashlittle(x.c_str(), x.length(), 0);
    std::cout << hashValue << std::endl;
    return 0;
}

