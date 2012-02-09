
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

int main() {
    boost::uuids::random_generator rg; 
    boost::uuids::uuid ui = rg();
    //boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::cout << ui << std::endl;
}
