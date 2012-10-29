#include <iostream>
#include <google/dense_hash_map>
#include <gtest/gtest.h>
#include <string>
#include "msg_cache.h"
#include "utils/jenkins_hash.h"
#include "utils/logging.h"
#include "utils/types.h"

using namespace capk;



StrategyRoute_map* 
read_cache(const char* filename) {
	FILE* fp_in = fopen(filename, "r");
	assert(fp_in);
	KStrategyCache cache;
	StrategyRoute_map* pcache = cache.getCache();
	pcache->unserialize(StrategyCacheSerializer(), fp_in);
	fclose(fp_in);
    return pcache;
}

void
print_cache(StrategyRoute_map* pcache) {
	char uuidbuf[UUID_STRLEN];
	StrategyRoute_map::iterator cacheiter;
	cacheiter = pcache->begin(); 
    while (cacheiter != pcache->end()) {
        std::cout << "UUID: " << cacheiter->first.c_str(uuidbuf) << std::endl;
        std::cout << "NumNodes: " << cacheiter->second.num_nodes << std::endl;
        std::cout << "Routing table: " << std::endl;
        node_t node;
        route_t route = cacheiter->second;
        for (size_t i = 0; i<cacheiter->second.num_nodes; i++) {
            route.getNode(i, &node);
            fprintf(stderr, "\t%d\n", (*((int*)(node.data()))));
        }
        cacheiter++;
    }
}


int
main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <cache_file>", argv[0]);
        return -1;
    }

    KStrategyCache cache;
    cache.read(argv[1]);
    cache.print_stats();     
    print_cache(cache.getCache());

    //StrategyRoute_map* pmap = read_cache(argv[1]);
    //assert(pmap);
    //print_cache(pmap);

}
