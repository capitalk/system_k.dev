#include <uuid/uuid.h>

#include "KMsgCache.h"
#include <google/dense_hash_map>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "utils/JenkinsHash.h"


#define UUID_STRLEN 36
int main()
{
// General setup for hashtable - empty and deleted keys
	char empty[UUID_LEN];
	memset(empty, 0, UUID_LEN);
	
	char deleted[UUID_LEN];
	memset(deleted, 1, UUID_LEN);

	char strbuf[UUID_STRLEN + 1]; 
	
	//order_id_t oid_empty(empty);
	order_id_t oid_empty(false);
	std::cout << "Empty oid: ";
	std::cerr << oid_empty.c_str(strbuf) << std::endl;

	//order_id_t oid_deleted(deleted);
	order_id_t oid_deleted("1");
	std::cout << "Deleted oid: ";
	std::cerr << oid_deleted.c_str(strbuf) << std::endl;


// Using custom key type and pod value
	dense_hash_map<order_id_t, int, std::tr1::hash<order_id>, eq_order_id> _cache;
	_cache.set_deleted_key(oid_deleted);
	_cache.set_empty_key(oid_empty);
	std::cerr << "size()        : " << _cache.size() << std::endl;
	std::cerr << "bucket_count(): " << _cache.bucket_count() << std::endl;

	order_id_t oid1(true);
	std::cout << "oid1: ";
	std::cerr << oid1.c_str(strbuf) << std::endl;
	order_id_t oid2(true);
	std::cout << "oid2: ";
	std::cerr << oid2.c_str(strbuf) << std::endl;

	
	std::cout << "oid1: ";
	std::cerr << oid1.c_str(strbuf) << std::endl;
	_cache[oid1] = 99;
	//_cache[oid2] = 10101101;

// Using shared_pointers for value
	dense_hash_map<order_id_t, OrderInfo_ptr, std::tr1::hash<order_id>, eq_order_id> _cache2;
	std::cerr << "size()        : " << _cache2.size() << std::endl;
	std::cerr << "bucket_count(): " << _cache2.bucket_count() << std::endl;
	
	_cache2.set_empty_key(oid_empty);
	_cache2.set_deleted_key(oid_deleted);

	OrderInfo* oi = new OrderInfo();
	_cache2[oid1] = OrderInfo_ptr(oi);
	OrderInfo_ptr op = _cache2[oid1];
	std::cerr << "op use count: " << op.use_count() << std::endl;
	op->shit();

	_cache2[oid2] = boost::make_shared<OrderInfo>();
	OrderInfo_ptr op2 = _cache2[oid2];
	std::cerr << "op use count: " << op2.use_count() << std::endl;
	op2->shit();

	std::cerr << "size()        : " << _cache2.size() << std::endl;
	std::cerr << "bucket_count(): " << _cache2.bucket_count() << std::endl;
	
/* Can't let two DIFFERENT POINTERS point to same shared_ptr
	//_cache2[oid2] = boost::make_shared<OrderInfo>();
	_cache2[oid2] = OrderInfo_ptr(oi);
	OrderInfo_ptr op2 = _cache2[oid2];
	std::cerr << "op use count: " << op2.use_count() << std::endl;
	op2->shit();
	
*/

	size_t newSize = _cache2.erase(oid2);
	std::cerr << "Delete 1: " << newSize << std::endl;
	newSize = _cache2.erase(oid1);
	std::cerr << "Delete 2: " << newSize << std::endl;

	std::cerr << "size()        : " << _cache2.size() << std::endl;
	std::cerr << "bucket_count(): " << _cache2.bucket_count() << std::endl;
}

