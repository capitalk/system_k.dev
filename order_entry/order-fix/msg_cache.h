#ifndef _K_MSG_CACHE
#define _K_MSG_CACHE


#include <google/dense_hash_map>

#include <uuid/uuid.h>

#include <memory.h>

#include <boost/make_shared.hpp>

#include <iostream>

#include "logging.h"
#include "utils/jenkins_hash.h"

using google::dense_hash_map;
using std::tr1::hash; // for hash function - specialized to Jenkns hash 


#define MAX_HOPS 4
#define MSG_ADDR_LEN 17
#define UUID_LEN 16
#define UUID_STRLEN 36
typedef char uuidbuf_t[UUID_STRLEN+1];

static const char* const STRATEGY_CACHE_FILENAME = "StrategyRouteMap.cache";
static const char* const ORDER_CACHE_FILENAME = "OrderStatusMap.cache";


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

	inline size_t size() const {
		return len;
	}

	inline char* data() {
		return addr;
	}

	char addr[MSG_ADDR_LEN];
	static const size_t len = MSG_ADDR_LEN;

	
}; 


struct route
{
	route():num_nodes(0) {
		memset(nodes, 0, sizeof(nodes));
	}

	int addNode(const node_t& node) {
		//assert(num_nodes < MAX_HOPS);
		return addNode(node.addr, sizeof(node.addr));
	}
	
	int addNode(const char* addr, size_t len) {
		assert(addr);
		if (!addr || len <= 0) {
			return -1;
		}
		if (num_nodes >= MAX_HOPS) {
			return -2;	
		}
		memcpy(&nodes[num_nodes], addr, len);
		num_nodes += 1;
		return 0;
	}

	int getNode(size_t i, node_t* node) {
		if (num_nodes < 0 || i > num_nodes-1) {
			return -1;
		}	
		assert(node);
		memcpy(node->addr, &nodes[i], nodes[i].len);	
		return 0;
	}

	int delNode() {
		if (num_nodes <= 0) {
			return -1;	
		}
		// Just decrement to delete since array is static
		num_nodes -= 1;	
        return num_nodes;
	}

	size_t size() {
		return num_nodes;
	}

	void clear() {
		num_nodes = 0;
	}

	bool operator==(route_t const & rhs) const {
		if (rhs.num_nodes != num_nodes) {
			return false;
		}
		for (size_t i=0; i<num_nodes; i++) {
			if (!(rhs.nodes[i] == nodes[i])) {
				return false;
			}
		}
		return true;
	}
	
#ifdef DEBUG
	void
	dbg_print() {
		for (unsigned i = 0; i< num_nodes; i++) {
			printf("Node %u: ", i);
			for (unsigned int j = 0; j < nodes[i].size(); j++) {
				printf("%c", nodes[i].addr[j]);
			}
			printf("\n");
		}
	}
#endif // DEBUG

	size_t num_nodes;
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

	bool set(const char* id, size_t len) {
	   	size_t expected = sizeof(uuid_t);
        if (len != expected) { 
          #ifdef DEBUG 
            printf("Expected buffer of length %zu bytes, got %zu\n", expected, len); 
          #endif 
          return false;
        } else { 
		  memcpy(this->_oid, id, len);
          return true; 
        }
	}

	int set(const char* id) {
		return uuid_parse(id, _oid);
	}

	int parse(const char* id) {
		return uuid_parse(id, _oid);
	}

	bool operator==(order_id const & rhs) const {
		//std::cerr << "ORDER_ID OPERATOR==" << std::endl;
		return (uuid_compare(rhs._oid, this->_oid) == 0);
	};

	// need a non-const version for ZMQ zero copy constructor
	unsigned char*
	uuid() {
		return _oid;
	}

	const unsigned char* 
	get_uuid() const {
		return _oid;	
	}

	size_t size() const {
		return UUID_LEN;
	}

	// buf must contain enough space - at least UUID_STRLEN+1 for uuid to be written
	char*
	c_str(char *buf) const 
	{
		if (buf) {
			uuid_unparse(_oid, buf);
			buf[UUID_STRLEN] = '\0';
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

	bool set(const char* id, size_t len) {
		size_t expected = sizeof(uuid_t);
        if (len != expected) { 
          #ifdef DEBUG
           printf("Expected strategy id of %zu bytes, got %zu\n", expected, len);
          #endif 
          return false; 
        } else { 
		  memcpy(this->_sid, id, len);
	      return true; 
        }
    }


	bool operator==(strategy_id const & rhs) const {
		return (uuid_compare(rhs._sid, this->_sid) == 0);
	};

	void set(const char c) {
		memset(this->_sid, c, UUID_LEN);
	}

	int parse(const char* id) {
		return uuid_parse(id, _sid);
	}

	void generate() {
		uuid_generate(_sid);
	}

	// need a non-const version for ZMQ zero copy constructor
	unsigned char*
	uuid() {
		return _sid;
	}

	const unsigned char* 
	get_uuid() const {
		return _sid;	
	}

	size_t size() const {
		return UUID_LEN;
	}

	// buf must contain enough space - at least UUID_STRLEN for uuid to be written
	char*
	c_str(char *buf) const
	{
		if (buf) {
			uuid_unparse(_sid, buf);
			buf[UUID_STRLEN-1] = '\0';
			return buf;
		}
		else {
			return NULL;
		}
	}

	uuid_t _sid;
};
typedef struct strategy_id strategy_id_t;

typedef uint32_t OrderStatus_t;
class OrderInfo
{
	public:
		OrderInfo() {
		};

		OrderInfo(const order_id_t& oid, 
					const strategy_id_t& sid) {
			memcpy(_oid._oid, oid._oid, sizeof(order_id_t));
			memcpy(_sid._sid, sid._sid, sizeof(strategy_id_t));
		}

		~OrderInfo() {
		};

		// by const ref - should never be changed externally
/*
		const route_t& getRoute() const {
			return _route;
		}

		void setRoute(route_t& r) {
			_route.clear();
			_route = r;
		}	
*/

		const order_id_t& getOrderID() const {
			return this->_oid;
		}
		
		void setOrderID(order_id_t& oid) {
			this->_oid = oid;
		}

		const strategy_id_t& getStrategyID() const {
			return this->_sid;
		}

		void setStrategyID(strategy_id_t& sid) {
			this->_sid = sid;
		}

		const OrderStatus_t& getStatus() const {
			return this->_status;
		}

		void setStatus(OrderStatus_t st) {
			_status = st;
		}

		void shit() {
			std::cerr << "STACKS OF SHIT" << std::endl;
		}

	private:
		strategy_id_t _sid;
		order_id_t _oid;
		OrderStatus_t _status;
};

typedef boost::shared_ptr<order_id_t> order_id_ptr;
typedef boost::shared_ptr<route_t> route_ptr;
typedef boost::shared_ptr<OrderInfo> OrderInfo_ptr;
// typedef struct OrderInfo OrderInfo_t
// typedef OrderInfo_t OrderInfo_ptr

// Equality test for order_id_t
struct eq_order_id
{
	bool operator() (const order_id_t& o1, const order_id_t& o2) const {
		return (&o1 == &o2) || (uuid_compare(o1._oid, o2._oid) == 0);
	}
};

// Equality test for order_id_t
struct eq_strategy_id
{
	bool operator() (const strategy_id_t& s1, const strategy_id_t& s2) const {
		return (&s1 == &s2) || (uuid_compare(s1._sid, s2._sid) == 0);
	}
};

// Specialized template hash function for order_id_t and strategy_id_t
namespace std {
	namespace tr1 {
		template<> class hash< order_id> 
		{ 
			public:
				size_t operator() (const order_id& x) const {
					size_t hval = hashlittle(x._oid, x.size(), 0);
					return hval;
				}
		};
	
		template<> class hash<strategy_id>
		{
			public:
				size_t operator() (const strategy_id& x) const {
					size_t hval = hashlittle(x._sid, x.size(), 0);
					return hval;
				}
		};
	};
};

namespace std {
	template <>
	inline bool operator==(const std::pair<const strategy_id_t, route_t>& p1, 
				const std::pair<const strategy_id_t, route_t>& p2) 
	{
		std::cerr << "bool operator==(const std::pair<const strategy_id_t, route_t>)" << std::endl;
		return (p1.first == p2.first && p1.second == p2.second);
	};
	
	template <>
	inline bool operator==(const std::pair<const order_id_t, OrderInfo_ptr>& p1, 
				const std::pair<const order_id_t, OrderInfo_ptr>& p2) 
	{
		std::cout << "bool operator==(const std::pair<order_id_t, OrderInfo_ptr>)" << std::endl;
		return (p1.first == p2.first);// && p1.second == p2.second);
	};
};

struct StrategyCacheSerializer {
	bool operator()(FILE* fp, const std::pair<const strategy_id_t, route_t>& value) const {
		// Write the key
		if (fwrite(&value.first._sid, sizeof(value.first._sid), 1, fp) != 1) {
			return false;
		}
	
		// Write the size of the route table - number of entries
		if (fwrite(&value.second.num_nodes, sizeof(value.second.num_nodes), 1, fp) != 1) {
			return false;
		}
	
		// Write the route table
		if (fwrite(&value.second.nodes, sizeof(value.second.nodes), 1, fp) != 1) {
			return false;
		}
	
		return true;
	};

	bool operator()(FILE* fp, std::pair<const strategy_id_t, route_t>* value) const {
	// Read the key.  Note the need for const_cast to get around
	// the fact hash_map keys are always const.
	// Read the sid
		if (fread(const_cast<uuid_t*>(&value->first._sid), sizeof(value->first._sid), 1, fp) != 1) {
			return false;
		}

		// Read the number of nodes in route.
		if (fread(&value->second.num_nodes, sizeof(value->second.num_nodes), 1, fp) != 1) {
			return false;
		}

		// Read the route
		if (fread(&value->second.nodes, sizeof(value->second.nodes), 1, fp) != 1) {
			return false;
		}

		return true;
	};
};

struct OrderCacheSerializer {
	bool operator()(FILE* fp, const std::pair<const order_id_t, OrderInfo_ptr>& value) const {

		// Write the key
		if (fwrite(&value.first._oid, sizeof(value.first._oid), 1, fp) != 1) {
			return false;
		}

		if (value.second == OrderInfo_ptr()) {
			std::cerr << "SOMETHING IS ROTTEN HERE" << std::endl;
		}
		// Write the order id
		const order_id_t& oid = value.second->getOrderID();
		if (fwrite(&oid._oid, sizeof(oid._oid), 1, fp) != 1) {
			return false;
		}

		// Write the strategy id
		const strategy_id_t& sid = value.second->getStrategyID();
		if (fwrite(&sid._sid, sizeof(sid._sid), 1, fp) != 1) {
			return false;
		}

		// Write the status
		const OrderStatus_t& status = value.second->getStatus();
		if (fwrite(&status, sizeof(status), 1, fp) != 1) {
			return false;
		}
	
		return true;
	};

	bool operator()(FILE* fp, std::pair<const order_id_t, OrderInfo_ptr>* value) const {
		if (fread(const_cast<uuid_t*>(&(value->first._oid)), sizeof(value->first._oid), 1, fp) != 1) {
			return false;	
		}

		value->second = boost::make_shared<OrderInfo>();
		order_id_t oid;
		if (fread(&oid._oid, sizeof(oid._oid), 1, fp) != 1) {
			return false;
		}
		value->second->setOrderID(oid);

		strategy_id_t sid;
		if (fread(&sid._sid, sizeof(sid._sid), 1, fp) != 1) {
			return false;
		}
		value->second->setStrategyID(sid);

		OrderStatus_t status;
		if (fread(&status, sizeof(status), 1, fp) != 1) {
			return false;
		}
		value->second->setStatus(status);

		return true;
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


// TODO - either templatize these two classes or create a common base class

#define STRATEGY_CACHE_INIT_SIZE 64
typedef dense_hash_map<strategy_id_t, route_t, std::tr1::hash<strategy_id>, eq_strategy_id> StrategyRoute_map;
class KStrategyCache
{
	public:
		KStrategyCache(size_t init_size = STRATEGY_CACHE_INIT_SIZE): _cache(init_size) {
			strategy_id empty("");
			_cache.set_empty_key(empty);
			strategy_id deleted("2");
			_cache.set_deleted_key(deleted);
		}	
		
		~KStrategyCache() {};
		
		void add(strategy_id_t& key, route_t& val) {
			if (_cache.find(key) != _cache.end()) {
				pan::log_DEBUG("KStrategyCache::add() - Key exists - replacing key with new route");
			}
			_cache[key] = val;
		}
	
		size_t del(const strategy_id_t& key) {
			return _cache.erase(key);
		}

		// TODO change return by value - sloppy
		route_t get(strategy_id_t& key) {
			if (_cache.find(key) != _cache.end()) {
				return _cache[key];
			}
			return route_t();	
		}

		// TODO should just return a const iterator
		StrategyRoute_map* getCache() {
			return &_cache;
		}	

		bool read(const char* filename) {
			if (!filename && *filename) {
				return false;
			}
			bool bOK;
			FILE* fp = fopen(filename, "r");
			if (!fp) {
				pan::log_CRITICAL("Can't open strategy cache file: ", filename);
				return false;
			}	
			bOK = _cache.unserialize(StrategyCacheSerializer(), fp);	
			fclose(fp);	
			return bOK;
		}
		
		bool write(const char* filename) {
			if (!filename && *filename) {
				return false;
			}
			bool bOK;
			FILE* fp = fopen(filename, "w");
			if (!fp) {
				pan::log_CRITICAL("Can't open strategy cache file: ", filename);
				return false;
			}
			bOK = _cache.serialize(StrategyCacheSerializer(), fp);
			fclose(fp);
			return bOK;

		}
	
		size_t size() { return _cache.size(); };

		void print_stats() {
			pan::log_DEBUG("Cache size: ", pan::integer(_cache.size()));
			pan::log_DEBUG("Cache bucket_count: ", pan::integer(_cache.bucket_count()));
		}

	private:
		StrategyRoute_map _cache;	
		
};


#define MSG_CACHE_INIT_SIZE 512
typedef dense_hash_map<order_id_t, OrderInfo_ptr, std::tr1::hash<order_id>, eq_order_id> OrderInfo_map;
class KOrderCache
{
	public:
		KOrderCache(size_t init_size = MSG_CACHE_INIT_SIZE): _cache(init_size) {
			order_id empty("");
			_cache.set_empty_key(empty);
			order_id deleted("1");
			_cache.set_deleted_key(deleted);
		}
		~KOrderCache() {};

		void add(order_id_t& key, OrderInfo_ptr& val) {
			_cache[key] = val;
		};

		size_t del(const order_id_t& key) {
			return _cache.erase(key);
		};

		OrderInfo_ptr get(order_id_t& key) {
			if (_cache.find(key) != _cache.end()) {
				return _cache[key];
			}
			return OrderInfo_ptr();
		}

		// TODO should just return a const iterator
		OrderInfo_map* getCache() {
			return &_cache;
		}	

		bool read(const char* filename) {
			if (!filename && *filename) {
				return false;
			}
			bool bOK;
			FILE* fp = fopen(filename, "r");
			if (!fp) {
				pan::log_CRITICAL("Can't open order cache file: ", filename);
				return false;
			}	
			bOK = _cache.unserialize(OrderCacheSerializer(), fp);	
			fclose(fp);	
			return bOK;
		}
		
		bool write(const char* filename) {
			if (!filename && *filename) {
				return false;
			}
			bool bOK;
			FILE* fp = fopen(filename, "w");
			if (!fp) {
				pan::log_CRITICAL("Can't open order cache file: ", filename, " - ABORTING");
				return false;
			}
			bOK = _cache.serialize(OrderCacheSerializer(), fp);
			fclose(fp);
			return bOK;

		}

		void print_stats() {
			pan::log_DEBUG("Cache size: ", pan::integer(_cache.size()));
			pan::log_DEBUG("Cache bucket_count: ", pan::integer(_cache.bucket_count()));
		}

	private:
		// KTK - use below if using struct hash
		//dense_hash_map<order_id_ptr, OrderInfo_ptr, hash_oid_ptr, cmp_oid_ptr> _cache;
		// KTK - use below for std::tr1::hash template specialization
		//dense_hash_map<order_id_t, OrderInfo_ptr, std::tr1::hash<order_id>, eq_order_id> OrderInfo_map;
		OrderInfo_map _cache;
};


struct MsgBytes 
{
	MsgBytes():_data(0), _len(0) {
	}

	MsgBytes(char* s, size_t len) { 
		set(s, len);
	};


	~MsgBytes() {
		destroy();
	}

	void
	destroy() {
		if (_data) {
			delete[] _data;
			_data = NULL;
			_len = 0;
		}
	}

	void
	set(char* s, size_t len) {
		destroy();
		assert(_data == NULL);
		assert(_len == 0);
		if (s && len > 0) {
			_data = new char[len];
			_len = len;
		}
	}
	
	size_t size() {
		return _len;
	}

	char* data() {
		return _data;
	}

	char* _data;	
	size_t _len;

};
#endif // _K_MSG_CACHE

