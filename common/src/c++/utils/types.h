#ifndef CAPK_GLOBAL_TYPEDEFS
#define CAPK_GLOBAL_TYPEDEFS

#include <stdint.h>
#include <memory.h>
#include <uuid/uuid.h>
#include <google/dense_hash_map>
#include <boost/shared_ptr.hpp>

#include "jenkins_hash.h"
#include "constants.h"
using std::tr1::hash; // for hash functors that are specialized to Jenkins Hash


namespace capk
{
typedef enum {
    BUY=0,
    SELL=1
} buy_sell_t;


enum TickType {
    QUOTE = 'Q',
    TRADE  = 'T'
};

enum Side {
    BID = 0,
    ASK = 1,
    NO_SIDE = 99
};
typedef Side Side_t;

//typedef uint32_t orderId_t;

typedef uint32_t msg_t;
typedef uint32_t venue_id_t;


typedef char uuidbuf_t[UUID_STRLEN+1];

typedef struct order_id order_id_t;
typedef struct node node_t;
typedef struct route route_t;


struct node 
{
	node():addr{0} {
	}

	node(const char a[ZMQ_ADDR_LEN]) {
		memcpy(this->addr, a, ZMQ_ADDR_LEN);
	}

	node(const char* a, size_t len) {
		memcpy(this->addr, a, len);
	}
	
	~node() {
		
	}

	node_t& operator=(node_t const & rhs) {
		memcpy(this->addr, rhs.addr, ZMQ_ADDR_LEN);
		return *this;
	}

	bool operator==(node_t const & rhs) const {
		return (memcmp(&(rhs.addr[0]), this->addr, ZMQ_ADDR_LEN) == 0);
	}

	inline size_t size() const {
		return len;
	}

	inline char* data() {
		return addr;
	}

	char addr[ZMQ_ADDR_LEN];
	static const size_t len = ZMQ_ADDR_LEN;

	
}; 


struct route
{
	route():num_nodes(0) {
		memset(nodes, 0, sizeof(nodes));
	}

	int addNode(const node_t& node) {
		//assert(num_nodes < ZMQ_MAX_MSG_HOPS);
		return addNode(node.addr, sizeof(node.addr));
	}
	
	int addNode(const char* addr, size_t len) {
		assert(addr);
		if (!addr || len <= 0) {
			return -1;
		}
		if (num_nodes >= ZMQ_MAX_MSG_HOPS) {
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
				printf("%u", nodes[i].addr[j]);
			}
			printf("\n");
		}
	}
#endif // DEBUG

	size_t num_nodes;
	node_t nodes[ZMQ_MAX_MSG_HOPS];
};




struct order_id
{
/*
	order_id() {
		set(0);
	}
*/
	
	order_id(bool gen = false):_oid{0} {
		if (gen) {
			generate();
		}
		//else {
			//_oid = {0};
		//}
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

    bool 
    is_empty() const
    {
       return uuid_is_null(_oid);
    }

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
	
	strategy_id(bool gen = false):_sid{0} {
		if (gen) {
			generate();
		}
		//else {
			//_sid = {};
		//}
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
			buf[UUID_STRLEN] = '\0';
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

	private:
		strategy_id_t _sid;
		order_id_t _oid;
		OrderStatus_t _status;
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

// Equality test for order_id_t
struct eq_strategy_id
{
	bool operator() (const strategy_id_t& s1, const strategy_id_t& s2) const {
		return (&s1 == &s2) || (uuid_compare(s1._sid, s2._sid) == 0);
	}
};

}; // namespace capk

// Specialized template hash function for order_id_t and strategy_id_t
namespace std {
    namespace tr1 {
		template<> class hash<capk::order_id> 
		{ 
			public:
				size_t operator() (const capk::order_id& x) const {
					size_t hval = hashlittle(x._oid, x.size(), 0);
					return hval;
				}
		};
	
		template<> class hash<capk::strategy_id>
		{
			public:
				size_t operator() (const capk::strategy_id& x) const {
					size_t hval = hashlittle(x._sid, x.size(), 0);
					return hval;
				}
		};
	};
}; // namepsace std



#endif // CAPK_OB_TYPEDEFS
