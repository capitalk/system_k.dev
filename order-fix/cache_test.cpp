#include <uuid/uuid.h>

#include "KMsgCache.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "utils/JenkinsHash.h"

int main()
{

	#define TEST_SIZE 2048
	KMsgCache mc(TEST_SIZE);
	// Sizing questions
	std::cerr << "sizeof(order_id_t): "		<< sizeof(order_id_t) << std::endl;
	std::cerr << "sizeof(strategy_id_t): "	<< sizeof(strategy_id_t) << std::endl;
	std::cerr << "sizeof(mc): "				<< sizeof(mc) << std::endl;
	std::cerr << "sizeof(OrderInfo): "		<< sizeof(OrderInfo) << std::endl;
	std::cerr << "sizeof(OrderInfo_ptr): "	<< sizeof(OrderInfo_ptr) << std::endl;

	// oid equality test
	order_id_t oid1;
	order_id_t oid2;

	// both are empty
	assert(oid1 == oid2);

	// create a list of oids
	order_id_t oid_list[TEST_SIZE];
	strategy_id_t sid_list[TEST_SIZE];

	boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time());
	
	char uuid_str[UUID_STRLEN+1];
	std::cerr << "Initializing oids" << std::endl;
	for (int i=0; i<TEST_SIZE; i++) {
		oid_list[i] = order_id(true);
		sid_list[i] = strategy_id();
		
	}
#if 0
	for (int i=0; i<TEST_SIZE; i++) {
		std::cerr << "oid " << i << ": " << oid_list[i].c_str(uuid_str) << std::endl;
		std::cerr << "sid " << i << ": " << sid_list[i].c_str(uuid_str) << std::endl;
	}
#endif 
	// Test node and route push/pop
	char r1[MSG_ADDR_LEN]; memset(r1, 'a', sizeof(r1));
		for (int i = 0; i<MSG_ADDR_LEN; i++) { std::cerr << r1[i];}; std::cerr << std::endl;
	char r2[MSG_ADDR_LEN]; memset(r2, 'b', sizeof(r2));
		for (int i = 0; i<MSG_ADDR_LEN; i++) { std::cerr << r2[i];}; std::cerr << std::endl;
	char r3[MSG_ADDR_LEN];  memset(r3, 'c', sizeof(r3));
		for (int i = 0; i<MSG_ADDR_LEN; i++) { std::cerr << r3[i];}; std::cerr << std::endl;
	char r4[MSG_ADDR_LEN];  memset(r4, 'd', sizeof(r4));
		for (int i = 0; i<MSG_ADDR_LEN; i++) { std::cerr << r4[i];}; std::cerr << std::endl;
	char r5[MSG_ADDR_LEN];  memset(r5, 'e', sizeof(r5));
		for (int i = 0; i<MSG_ADDR_LEN; i++) { std::cerr << r5[i];}; std::cerr << std::endl;

		

	OrderInfo_ptr testid = boost::make_shared<OrderInfo>();
	assert(testid->pushRoute(r1, MSG_ADDR_LEN) == 0);
	assert(testid->pushRoute(r2, MSG_ADDR_LEN) == 0);
	assert(testid->pushRoute(r3, MSG_ADDR_LEN) == 0);
	node_t n4(r4);
	assert(testid->pushRoute(n4) == 0);
		std::cerr << "n4: " ;
		for (int i = 0; i<MSG_ADDR_LEN; i++) { std::cerr << n4.addr[i];}; std::cerr << std::endl;
	
	node_t n3(r3);
		std::cerr << "n3: " ;
		for (int i = 0; i<MSG_ADDR_LEN; i++) { std::cerr << n3.addr[i];}; std::cerr << std::endl;
	node_t cur_node;
	assert(testid->popRoute(&cur_node) == 0);
		std::cerr << "cur_node (popped): " ;
		for (int i = 0; i<MSG_ADDR_LEN; i++) { std::cerr << cur_node.addr[i];}; std::cerr << std::endl;
	// Should fail
	//assert(cur_node == n3);
	
// Causes assertion failure in stack - too many routes
//	assert(testid->pushRoute(r5, MSG_ADDR_LEN) == 0);

	char ch;
	std::cin >> ch;		
		

	std::cerr << "Inserting oids and order info" << std::endl;
	// insert all oids and then delete them - assert count == 0
	for (int i=0; i<TEST_SIZE; i++) {
		OrderInfo_ptr info_ptr(new OrderInfo(oid_list[i], sid_list[i]));
		//info_ptr->shit();
		mc.add(oid_list[i], info_ptr);
	}
	std::cerr << "size()        : " << (mc.getCache())->size() << std::endl;

		
	size_t new_size;
	for (int j=0; j<TEST_SIZE; j++) {

		new_size = mc.del(oid_list[j]);
#if 0
		std::cerr << "size()        : " << (mc.getCache())->size() << std::endl;
		std::cerr << "bucket_count(): " << (mc.getCache())->bucket_count() << std::endl;
#endif 

	}
	std::cerr << "size()        : " << (mc.getCache())->size() << std::endl;
	boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time());
	boost::posix_time::time_duration duration(time_end - time_start);	
	std::cout << "Duration: " << duration << std::endl;
}

