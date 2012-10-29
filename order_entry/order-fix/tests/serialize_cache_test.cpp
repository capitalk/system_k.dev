#include <iostream>
#include <google/dense_hash_map>
#include <gtest/gtest.h>
#include <string>
#include "msg_cache.h"
#include "utils/jenkins_hash.h"
#include "utils/logging.h"
#include "utils/types.h"

using google::dense_hash_map;      // namespace where class lives by default
using std::tr1::hash;  // or __gnu_cxx::hash, or maybe tr1::hash, depending on your OS
using namespace capk;

//typedef dense_hash_map<const int, std::string, std::tr1::hash<const int>,  eq> TESTMAP;
//

struct StringToIntSerializer {
  bool operator()(FILE* fp, const std::pair<const int, std::string>& value) const {
    // Write the key.  We ignore endianness for this example.
    if (fwrite(&value.first, sizeof(value.first), 1, fp) != 1)
      return false;
    // Write the value.
    assert(value.second.length() <= 255);   // we only support writing small strings
    const unsigned char size = value.second.length();
    if (fwrite(&size, 1, 1, fp) != 1)
      return false;
    if (fwrite(value.second.data(), size, 1, fp) != 1)
      return false;
    return true;
  }

  bool operator()(FILE* fp, std::pair<const int, std::string>* value) const {
    // Read the key.  Note the need for const_cast to get around
    // the fact hash_map keys are always const.
    if (fread(const_cast<int*>(&value->first), sizeof(value->first), 1, fp) != 1)
      return false;
    // Read the value.
    unsigned char size;    // all strings are <= 255 chars long
    if (fread(&size, 1, 1, fp) != 1)
      return false;
    char* buf = new char[size];
    if (fread(buf, size, 1, fp) != 1) {
      delete[] buf;
      return false;
    }
    value->second.assign(buf, size);
    delete[] buf;
    return true;
  }
};

// Specialized template hash function for order_id_t and strategy_id_t
namespace std {
    namespace tr1 {
        template<> class hash< const int>
        {
            public:
                size_t operator() (const int& x) const {
                    //size_t hval = hashlittle(x._oid, x.size(), 0);
                    return x;
                    //return hval;
                }
        };

    };
};


TEST(Serialization, SerializeSIDCache) 
{

// TEST STRATEGY CACHE SERIALIZATION
	KStrategyCache sc1;
	strategy_id_t sid1(true);
	route_t r1;
	r1.addNode("11111111111111111", 17);
	r1.addNode("22222222222222222", 17);
	r1.addNode("33333333333333333", 17);
	std::cerr << "Adding route r1:" << std::endl;
	r1.dbg_print();
	sc1.add(sid1, r1);

	strategy_id_t sid3(true);
	route_t r3(r1);
	std::cerr << "Adding route r3:" << std::endl;
	r3.dbg_print();
	sc1.add(sid3, r3);

	KStrategyCache sc2;
/*
	strategy_id_t sid2(true);
	route_t r2;
	r2.addNode("44444444444444444", 17);
	r2.addNode("55555555555555555", 17);
	r2.addNode("66666666666666666", 17);
	sc2.add(sid2, r2);
*/	
	FILE* fp = fopen("strategyCache.data", "w");
	assert(fp);

	StrategyRoute_map* pscm1 = sc1.getCache();	
	pscm1->serialize(StrategyCacheSerializer(), fp);
	fclose(fp);

// WRITING COMPLETE - NOW TRY READING

	FILE* fp_in = fopen("strategyCache.data", "r");
	assert(fp_in);
	StrategyRoute_map* pscm2 = sc2.getCache();
	pscm2->unserialize(StrategyCacheSerializer(), fp_in);
	fclose(fp_in);


// SANITY PRINT
	StrategyRoute_map::iterator it1 = pscm1->begin();
	StrategyRoute_map::iterator it2 = pscm2->begin();
	char uuidbuf[UUID_STRLEN];
	for (it1 = pscm1->begin(), it2 = pscm2->begin(); it1 != pscm1->end(), it2 != pscm2->end(); it1++, it2++) {
		std::cout << "Comparing1: " << it1->first.c_str(uuidbuf) << std::endl;
		std::cout << "Comparing2: " << it2->first.c_str(uuidbuf) << std::endl;
		std::cout << "Comparing1: " << it1->second.num_nodes << std::endl;
		std::cout << "Comparing2: " << it2->second.num_nodes << std::endl;
		std::cout << "Routing table 1: " << std::endl;
		it1->second.dbg_print();
		std::cout << "Routing table 2: " << std::endl;
		it2->second.dbg_print();
	}

	// Check that maps are equal
	EXPECT_TRUE((*pscm1) == (*pscm2));
}

TEST(Serialization, SerializeOIDCache) 
{
	KOrderCache oc1;
	order_id_t o1(true);	
	order_id_t o2(true);	
	order_id_t o3(true);	
	strategy_id_t sid(true);	
	OrderInfo_ptr oi1= boost::make_shared<OrderInfo>(o1, sid);
	OrderInfo_ptr oi2= boost::make_shared<OrderInfo>(o2, sid);
	OrderInfo_ptr oi3= boost::make_shared<OrderInfo>(o3, sid);
	oc1.add(o1, oi1);
	oc1.add(o2, oi2);
	oc1.add(o3, oi3);

	bool ret;

	FILE* fp = fopen("orderCache.data", "w");	
	assert(fp);
	OrderInfo_map* poim1 = oc1.getCache();
	ret = poim1->serialize(OrderCacheSerializer(), fp);
	EXPECT_TRUE(ret);
	fclose(fp);
// WRITING COMPLETE - NOW TRY READING

	KOrderCache oc2;
	FILE* fp_in = fopen("orderCache.data", "r");
	assert(fp_in);
	OrderInfo_map* poim2 = oc2.getCache();
	ret = poim2->unserialize(OrderCacheSerializer(), fp_in);
	EXPECT_TRUE(ret);
	fclose(fp_in);

// SANTIY PRINT
	OrderInfo_map::iterator it1 = poim1->begin();
	OrderInfo_map::iterator it2 = poim2->begin();
	char uuidbuf[UUID_STRLEN];

	std::cout << "Cache1 size(): " << poim1->size() << std::endl;
	std::cout << "Cache2 size(): " << poim2->size() << std::endl;

	for (it1 = poim1->begin(), it2 = poim2->begin(); it1 != poim1->end(), it2 != poim2->end(); it1++, it2++) {
		std::cout << "Comparing1: " << it1->first.c_str(uuidbuf) << std::endl;
		std::cout << "Comparing2: " << it2->first.c_str(uuidbuf) << std::endl;
	}

	// Check that maps are equal
	EXPECT_TRUE((*poim1) == (*poim2));

}

int
main(int argc, char** argv)
{

	testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();
	
	return result;
}

