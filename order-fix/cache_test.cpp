#include <uuid/uuid.h>

#include "KMsgCache.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "utils/JenkinsHash.h"

#include "gtest/gtest.h"

// Number of test items default
//#define TEST_SIZE 2048
static const int TEST_SIZE = 2048;

KOrderCache mc(TEST_SIZE);

// create a list of oids
order_id_t oid_list[TEST_SIZE];
strategy_id_t sid_list[TEST_SIZE];

TEST(CacheTest, InitCache) {
	std::cerr << "Creating strategy and order ids [" << TEST_SIZE << "]" << std::endl;
	for (int i=0; i<TEST_SIZE; i++) {
		oid_list[i] = order_id(true);
		sid_list[i] = strategy_id(true);
		
	}
	std::cerr << "Inserting oids and order info" << std::endl;
	for (int i=0; i<TEST_SIZE; i++) {
		OrderInfo_ptr info_ptr(new OrderInfo(oid_list[i], sid_list[i]));
		mc.add(oid_list[i], info_ptr);
	}
	std::cerr << "Cache size(): " << (mc.getCache())->size() << std::endl;
	ASSERT_EQ(TEST_SIZE, mc.getCache()->size());
}


TEST(CacheTest, GetSet) {
	order_id_t o1(true);
	order_id_t o2(true);
	order_id_t o3(true);
	OrderInfo_ptr oi1 = boost::make_shared<OrderInfo>();
	mc.add(o1,oi1);
	mc.add(o2,oi1);
	mc.add(o3,oi1);

	// check get(...) method
	std::cerr << "Testing cache.get(oid)" << std::endl;
	char sidbuf[UUID_STRLEN + 1];
	char oidbuf[UUID_STRLEN + 1];
	order_id_t o4(true);
	strategy_id_t s1(true);
	std::cerr << "Created oid: " << o4.c_str(oidbuf) << std::endl;
	std::cerr << "Created sid: " << s1.c_str(sidbuf) << std::endl;
	// clear the buffer!
	memset(sidbuf, 0, sizeof(sidbuf));
	OrderInfo_ptr oi2 = boost::make_shared<OrderInfo>(o4, s1);
	mc.add(o4, oi2);
	OrderInfo_ptr foundoi = mc.get(o4);
	EXPECT_NE(foundoi, OrderInfo_ptr());
	std::cerr << "Found strategy id: " << foundoi->getStrategyID().c_str(sidbuf) << std::endl;
	std::cerr << "Cache size(): " << (mc.getCache())->size() << std::endl;

	// EXPECT FAILURE
	order_id_t o5(true);
	OrderInfo_ptr oi3 = mc.get(o5);	
	EXPECT_EQ(oi3, OrderInfo_ptr());
	//assert(oi3 != OrderInfo_ptr());	
}

void
print_node(const char* prefix, node_t& node) {
		std::cerr << prefix << ": ";
		for (int i = 0; i<MSG_ADDR_LEN; i++) { std::cerr << node.addr[i];}; std::cerr << std::endl;
}

TEST(CacheTest, RouteModification) {
	// Test node and route add/get
	//
	route_t ret_route;	
	char node1[MSG_ADDR_LEN]; memset(node1, 'a', sizeof(node1));
	//print_node("node1", node1);

	char node2[MSG_ADDR_LEN]; memset(node2, 'b', sizeof(node2));
	//print_node("node2", node1);

	char node3[MSG_ADDR_LEN];  memset(node3, 'c', sizeof(node3));
	//print_node("node3", node1);

	char node4[MSG_ADDR_LEN];  memset(node4, 'd', sizeof(node4));
	//print_node("node4", node1);

	char node5[MSG_ADDR_LEN];  memset(node5, 'e', sizeof(node5));
	//print_node("node5", node1);

	int ret;
	ret = ret_route.addNode(node1);
	EXPECT_TRUE(ret == 0);
	ret = ret_route.addNode(node2);
	EXPECT_TRUE(ret == 0);
	ret = ret_route.addNode(node3);
	EXPECT_TRUE(ret == 0);

	node_t copynode4(node4);
	ret = ret_route.addNode(copynode4);
	ASSERT_TRUE(ret == 0);
	//print_node("copy4", copynode4);
	
	node_t copynode3(node3);
	ASSERT_TRUE(copynode3 == node3);
	//print_node("copy3", copynode3);

	node_t cur_node;
	ret = ret_route.getNode(ret_route.size()-1, &cur_node);
	EXPECT_EQ(ret_route.size(), 4);
	EXPECT_EQ(0, ret);
/*
	std::cerr << "cur_node (list): " ;
	for (int i = 0; i<MSG_ADDR_LEN; i++) { std::cerr << cur_node.addr[i];}; std::cerr << std::endl;
*/

	// current node is NOT s3 - it should be node4
	ASSERT_FALSE(cur_node == node3);
	
	for (size_t i = 0; i< ret_route.size(); i++) {
		//std::cerr << "Original node list size(" << i << "):";
		node_t* pnode = new node_t();
		ret = ret_route.getNode(i, pnode);
		EXPECT_EQ(0, ret);
		//for (int i = 0; i<MSG_ADDR_LEN; i++) { std::cerr << pnode->addr[i];}; std::cerr << std::endl;
	}
	
	// copy the return route 
	route_t copyroute = ret_route;

	for (size_t i = 0; i< copyroute.size(); i++) {
		//std::cerr << "Copied node list size(" << i << "):";
		node_t* pnode = new node_t();
		ret = copyroute.getNode(i, pnode);
		EXPECT_EQ(0, ret);
		//for (int i = 0; i<MSG_ADDR_LEN; i++) { std::cerr << pnode->addr[i];}; std::cerr << std::endl;
	}

	// Causes assertion failure in stack - too many routes
	ASSERT_EQ(-2, ret_route.addNode(node5));

}

TEST(CacheTest, Operators)
{
	// oid equality test
	order_id_t oid1;
	order_id_t oid2;

	// both are empty
	EXPECT_TRUE(oid1 == oid2);

	// set from const char*
	oid1.set("d024cc8f-c32b-4d54-b766-3fea5635efb1");
	// assignment operator
	oid2 = oid1;
	// test equality;
	EXPECT_TRUE(oid1 == oid2);
	
	// copy ctor
	order_id_t oid3 = oid1;
	EXPECT_TRUE(oid3 == oid1);

}


TEST(CacheTest, ReturnRouteByValue) 
{

	KStrategyCache _scache;
	strategy_id_t sid(true);
	route_t r1;
	r1.addNode("11111111111111111", 17);
	r1.addNode("22222222222222222", 17);
	r1.addNode("33333333333333333", 17);

	//std::cerr << "Route r1" << std::endl;
	//r1.dbg_print();
	
	// add the route for the given sid	
	_scache.add(sid, r1); 

	route_t r2;
	// get by value
	r2 = _scache.get(sid);
	ASSERT_TRUE(r2.num_nodes == r1.num_nodes);

	for (unsigned int i = 0; i < r2.num_nodes; i++) {
		ASSERT_TRUE(r1.nodes[i] == r2.nodes[i]);
	}

	// delete the sid associated with the route
	_scache.del(sid);
	// now r1 should be empty route 
	r1 = _scache.get(sid);

	//std::cerr << "Route r1 - after delete" << std::endl;
	//r1.dbg_print();
	EXPECT_NE(r1.size(), r2.size());
	
}

TEST(CacheTest, DeleteAll) 
{
	size_t deleted_count;
	for (int j=0; j<TEST_SIZE; j++) {
		deleted_count = mc.del(oid_list[j]);
	}
	std::cerr << "Cache size(): " << (mc.getCache())->size() << std::endl;
	ASSERT_EQ(4, mc.getCache()->size());
}

void
print_all(KOrderCache& mc) {
	std::cerr << "*********************************Begin all items " << std::endl;
	OrderInfo_map* map = mc.getCache();			
	OrderInfo_map::iterator it = map->begin();
	char idbuf[UUID_STRLEN + 1] ;
	while (it != map->end()) {
		order_id_t id = it->first;
		id.c_str(idbuf);	
		std::cerr << idbuf << std::endl;
		it++;
	}
	std::cerr << "*********************************End all items " << std::endl;
}

int main(int argc, char** argv)
{


	// Sizing questions
	std::cerr << "sizeof(order_id_t): "		<< sizeof(order_id_t) << std::endl;
	std::cerr << "sizeof(strategy_id_t): "	<< sizeof(strategy_id_t) << std::endl;
	std::cerr << "sizeof(mc): "				<< sizeof(mc) << std::endl;
	std::cerr << "sizeof(OrderInfo): "		<< sizeof(OrderInfo) << std::endl;
	std::cerr << "sizeof(OrderInfo_ptr): "	<< sizeof(OrderInfo_ptr) << std::endl;

	boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time());
	testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();

	boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time());
	boost::posix_time::time_duration duration(time_end - time_start);	
	std::cout << "Duration: " << duration << std::endl;
	return result;
}

