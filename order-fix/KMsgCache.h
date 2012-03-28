#ifndef _K_MSG_CACHE
#define _K_MSG_CACHE

#include <google/dense_hash_map>
#include <uuid/uuid.h>

#include <memory.h>

#include "utils/JenkinsHash.h"

#include <boost/make_shared.hpp>

#include <iostream>

using google::dense_hash_map;
using std::tr1::hash; // for hash function - TODO change to Murmur3

#define MAX_HOPS 4
#define MSG_ADDR_LEN 17
#define UUID_LEN 16
#define UUID_STRLEN 36+1

typedef struct order_id order_id_t;
typedef struct node node_t;
typedef struct route route_t;

struct node 
{
	node() {
		addr = {0};
	}

	node(const char a[MSG_ADDR_LEN]) {
		memcpy(this->addr, a, MSG_ADDR_LEN);
	}

	node(const char* a, size_t len) {
		memcpy(this->addr, a, len);
	}
	
	~node() {
		
	}

	node_t& operator=(node_t const & rhs) {
		memcpy(this->addr, rhs.addr, MSG_ADDR_LEN);
		return *this;
	}

	bool operator==(node_t const & rhs) const {
		return (memcmp(&(rhs.addr[0]), this->addr, MSG_ADDR_LEN) == 0);
	}

	const inline size_t size() {
		return MSG_ADDR_LEN;
	}

	inline char* data() {
		return addr;
	}

	char addr[MSG_ADDR_LEN];
	
}; 

// ZMQ free function for zero copy 
/*
void
freenode(void* data, void* hint) {
	if (data) {
		delete(static_cast<node_t*>(data));
	}
}
*/

struct route
{
	route():num_nodes(0) {
		memset(nodes, 0, sizeof(nodes));
	}

	unsigned short int num_nodes;
	node_t nodes[MAX_HOPS];
};


struct order_id
{
/*
	order_id() {
		set(0);
	}
*/
	
	order_id(bool gen = false) {
		if (gen) {
			generate();
		}
		else {
			_oid = {0};
		}
	}
/*
	// for initializing special values
	order_id(const char init[UUID_LEN]) {
		memcpy(oid, init, UUID_LEN);
	}
*/

	order_id(const char* c) {
		if (c) {
			set(*c);
		}
	}


	order_id(const struct order_id& id) {
		//std::cerr << "ORDER_ID COPY CTOR" << std::endl;
		memcpy(this->_oid, id._oid, sizeof(uuid_t));	
	}

	void generate() {
		uuid_generate(_oid);
	}
/* don't need this if array is just inside a struct
 * @see http://stackoverflow.com/questions/3437110/why-does-c-support-memberwise-assignment-of-arrays-within-structs-but-not-gen
 */	
/*
	order_id& operator=(order_id const & rhs) {
		if (this == &rhs) return *this;
		std::cerr << "ORDER_ID ASSIGNMENT" << std::endl;
		memcpy(this->oid, rhs.oid, sizeof(uuid_t));	
		return *this;
	}
*/
	void set(const char c) {
		memset(this->_oid, c, UUID_LEN);
	}

	void set(const char* id, size_t len) {
		assert(len == sizeof(uuid_t));
		memcpy(this->_oid, id, len);
	};

	bool operator==(order_id const & rhs) const {
		//std::cerr << "ORDER_ID OPERATOR==" << std::endl;
		return (uuid_compare(rhs._oid, this->_oid) == 0);
	};

	unsigned char* 
	get_uuid()
	{
		return _oid;	
	}

	size_t size() {
		return UUID_LEN;
	}

	// buf must contain enough space - at least UUID_STRLEN for uuid to be written
	char*
	c_str(char *buf)
	{
		if (buf) {
			uuid_unparse(_oid, buf);
			buf[UUID_STRLEN] = 0;
			return buf;
		}
		else {
			return NULL;
		}
	}

	

	void fuck() {
		std::cerr << "HERE IS SOME FUCK" << std::endl;
	};

	uuid_t _oid;
};

struct strategy_id
{
/*
	strategy_id() {
		_sid = {0};
	};

*/
	~strategy_id() {}

/*
	order_id() {
		set(0);
	}
*/
	
	strategy_id(bool gen = false) {
		if (gen) {
			generate();
		}
		else {
			_sid = {0};
		}
	}
/*
	// for initializing special values
	order_id(const char init[UUID_LEN]) {
		memcpy(oid, init, UUID_LEN);
	}
*/

	strategy_id(const char* c) {
		if (c) {
			set(*c);
		}
	}


	strategy_id(const struct strategy_id& id) {
		memcpy(this->_sid, id._sid, sizeof(uuid_t));	
	}

	void set(const char* id, size_t len) {
		assert(len == sizeof(uuid_t));
		memcpy(this->_sid, id, len);
	};

	bool operator==(strategy_id const & rhs) const {
		return (uuid_compare(rhs._sid, this->_sid) == 0);
	};

	void set(const char c) {
		memset(this->_sid, c, UUID_LEN);
	}

	void generate() {
		uuid_generate(_sid);
	}

	unsigned char* 
	get_uuid()
	{
		return _sid;	
	}


	// buf must contain enough space - at least UUID_STRLEN for uuid to be written
	char*
	c_str(char *buf)
	{
		if (buf) {
			uuid_unparse(_sid, buf);
			buf[UUID_STRLEN] = 0;
			return buf;
		}
		else {
			return NULL;
		}
	}

	uuid_t _sid;
};
typedef struct strategy_id strategy_id_t;

class OrderInfo
{
	public:
		OrderInfo() {
			//std::cerr << "OrderInfo()" << std::endl;
		};

		OrderInfo(const order_id_t& oid, const strategy_id_t& sid) {
			memcpy(_oid._oid, oid._oid, sizeof(order_id_t));
			memcpy(_sid._sid, sid._sid, sizeof(strategy_id_t));
		}

		~OrderInfo() {
			//std::cerr << "~OrderInfo()" << std::endl;
		};

		uint32_t pushRoute(const node_t& node) {
			assert(_path.num_nodes < MAX_HOPS);
			return pushRoute(node.addr, sizeof(node.addr));
		}

		uint32_t pushRoute(const char* addr, size_t len) {
			assert(addr);
			if (!addr || len <= 0) {
				return -1;
			}
			//std::cerr << "**************NODES IN STACK: " << _path.num_nodes << std::endl;
			assert(_path.num_nodes < MAX_HOPS);
			//pan::log_DEBUG("NODES IN STACK: ", pan::integer(_path.num_nodes));
			memcpy(&_path.nodes[_path.num_nodes], addr, MSG_ADDR_LEN);
			_path.num_nodes += 1;
			return 0;
		}

		uint32_t popRoute(node_t* node) {
			if (_path.num_nodes <= 0) {
				return -1;	
			}
			assert(node);
			memcpy(node->addr, &_path.nodes[_path.num_nodes-1], MSG_ADDR_LEN);
			_path.num_nodes -= 1;
			return 0;
		}

		uint32_t routeSize() {
			return _path.num_nodes;
		}

		// By value!
		order_id_t getOrderID() {
			return this->_oid;
		}

		uint32_t getStatus() {
			return this->_status;
		}

		void shit() {
			std::cerr << "STACKS OF SHIT" << std::endl;
		}

	private:
		strategy_id_t _sid;
		order_id_t _oid;
		route_t _path;
		uint32_t _status;
};

typedef boost::shared_ptr<order_id_t> order_id_ptr;
typedef boost::shared_ptr<route_t> route_ptr;
typedef boost::shared_ptr<OrderInfo> OrderInfo_ptr;

// Equality test for order_id_t
struct eq_order_id
{
	bool operator() (const order_id_t& o1, const order_id_t& o2) const {
		return (&o1 == &o2) || (uuid_compare(o1._oid, o2._oid) == 0);
	}
};

// Specialized template hash function for order_id_t
namespace std {
	namespace tr1 {
		template<> class hash< order_id> 
		{ 
			public:
				size_t operator() (const order_id& x) const {
					size_t hval = hashlittle(x._oid, UUID_LEN, 0);
					return hval;
				}
		};
	};
};


/*
// KTK  - use this to use the struct and above to specialize the
// hash function in std::tr1 namespace
// @see http://marknelson.us/2011/09/03/hash-functions-for-c-unordered-container
//
struct hash_oid_ptr
{
	size_t operator() (const order_id_ptr& x) const {
		size_t hval =  hashlittle(x->oid, UUID_LEN, 0);
		return hval;
	}
};

struct eq_order_id_ptr
{
	bool operator() (const order_id_ptr& o1, const order_id_ptr& o2) const {
		return (uuid_compare(o1->oid, o2->oid) == 0);
	}
};
*/

//static const size_t CACHE_INIT_SIZE 2048;
typedef dense_hash_map<order_id_t, OrderInfo_ptr, std::tr1::hash<order_id>, eq_order_id> OrderInfo_map;
#define CACHE_INIT_SIZE 512
class KMsgCache
{
	public:
		KMsgCache(size_t init_size = CACHE_INIT_SIZE): _cache(init_size) {
			order_id empty("");
			_cache.set_empty_key(empty);
			order_id deleted("1");
			_cache.set_deleted_key(deleted);
		}
		~KMsgCache() {};

		void add(order_id_t& key, OrderInfo_ptr& val) {
			_cache[key] = val;
		};

		size_t del(const order_id_t& key) {
			return _cache.erase(key);
		};

		OrderInfo_map* 
		getCache() {
			return &_cache;
		}	

	private:
		// KTK - use below if using struct hash
		//dense_hash_map<order_id_ptr, OrderInfo_ptr, hash_oid_ptr, cmp_oid_ptr> _cache;
		// KTK - use below for std::tr1::hash template specialization
		//dense_hash_map<order_id_t, OrderInfo_ptr, std::tr1::hash<order_id>, eq_order_id> OrderInfo_map;
		OrderInfo_map _cache;
};

#endif // _K_MSG_CACHE

