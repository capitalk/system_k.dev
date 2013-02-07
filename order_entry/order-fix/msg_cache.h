#ifndef _K_MSG_CACHE
#define _K_MSG_CACHE


#include <google/dense_hash_map>

#include <memory.h>

#include <boost/make_shared.hpp>

#include <iostream>

#include "utils/logging.h"
#include "utils/jenkins_hash.h"
#include "utils/types.h"

using google::dense_hash_map;
using std::tr1::hash; // for hash function - specialized to Jenkns hash 


//static const char* const STRATEGY_CACHE_FILENAME = "StrategyRouteMap.cache";
//static const char* const ORDER_CACHE_FILENAME = "OrderStatusMap.cache";


namespace std {
	template <>
	inline bool operator==(const std::pair<const capk::strategy_id_t, capk::route_t>& p1, 
				const std::pair<const capk::strategy_id_t, capk::route_t>& p2) 
	{
		std::cerr << "bool operator==(const std::pair<const strategy_id_t, route_t>)" << std::endl;
		return (p1.first == p2.first && p1.second == p2.second);
	};
	
	template <>
	inline bool operator==(const std::pair<const capk::order_id_t, capk::OrderInfo_ptr>& p1, 
				const std::pair<const capk::order_id_t, capk::OrderInfo_ptr>& p2) 
	{
		std::cout << "bool operator==(const std::pair<order_id_t, OrderInfo_ptr>)" << std::endl;
		return (p1.first == p2.first);// && p1.second == p2.second);
	};
};

struct StrategyCacheSerializer {
	bool operator()(FILE* fp, const std::pair<const capk::strategy_id_t, capk::route_t>& value) const {
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

	bool operator()(FILE* fp, std::pair<const capk::strategy_id_t, capk::route_t>* value) const {
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
	bool operator()(FILE* fp, const std::pair<const capk::order_id_t, capk::OrderInfo_ptr>& value) const {

		// Write the key
		if (fwrite(&value.first._oid, sizeof(value.first._oid), 1, fp) != 1) {
			return false;
		}

		if (value.second == capk::OrderInfo_ptr()) {
			std::cerr << "Error writing hash table in OrderCacheSerialiser - value.second is empty!" << std::endl;
		}
		// Write the order id
		const capk::order_id_t& oid = value.second->getOrderID();
		if (fwrite(&oid._oid, sizeof(oid._oid), 1, fp) != 1) {
			return false;
		}

		// Write the strategy id
		const capk::strategy_id_t& sid = value.second->getStrategyID();
		if (fwrite(&sid._sid, sizeof(sid._sid), 1, fp) != 1) {
			return false;
		}

		// Write the status
		const capk::OrderStatus_t& status = value.second->getStatus();
		if (fwrite(&status, sizeof(status), 1, fp) != 1) {
			return false;
		}
	
		return true;
	};

	bool operator()(FILE* fp, std::pair<const capk::order_id_t, capk::OrderInfo_ptr>* value) const {
		if (fread(const_cast<uuid_t*>(&(value->first._oid)), sizeof(value->first._oid), 1, fp) != 1) {
			return false;	
		}

		value->second = boost::make_shared<capk::OrderInfo>();
        capk::order_id_t oid;
		if (fread(&oid._oid, sizeof(oid._oid), 1, fp) != 1) {
			return false;
		}
		value->second->setOrderID(oid);

        capk::strategy_id_t sid;
		if (fread(&sid._sid, sizeof(sid._sid), 1, fp) != 1) {
			return false;
		}
		value->second->setStrategyID(sid);
        
        capk::OrderStatus_t status;
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
typedef dense_hash_map<capk::strategy_id_t, capk::route_t, std::tr1::hash<capk::strategy_id>, capk::eq_strategy_id> StrategyRoute_map;
class KStrategyCache
{
	public:
		KStrategyCache(size_t init_size = STRATEGY_CACHE_INIT_SIZE): _cache(init_size) {
            capk::strategy_id empty("");
			_cache.set_empty_key(empty);
            capk::strategy_id deleted("2");
			_cache.set_deleted_key(deleted);
		}	
		
		~KStrategyCache() {};
		
		void add(capk::strategy_id_t& key, capk::route_t& val) {
			if (_cache.find(key) != _cache.end()) {
				pan::log_DEBUG("KStrategyCache::add() - Key exists - replacing key with new route");
			}
			_cache[key] = val;
		}
	
		size_t del(const capk::strategy_id_t& key) {
			return _cache.erase(key);
		}

		// TODO change return by value - sloppy
        capk::route_t get(capk::strategy_id_t& key) {
			if (_cache.find(key) != _cache.end()) {
				return _cache[key];
			}
			return capk::route_t();	
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
				pan::log_CRITICAL("Can't open strategy cache file for read: ", filename, " error: ", strerror(errno));
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
				pan::log_CRITICAL("Can't open strategy cache file for write: ", filename, " error: ", strerror(errno));
				return false;
			}
			bOK = _cache.serialize(StrategyCacheSerializer(), fp);
			fclose(fp);
			return bOK;

		}
	
		size_t size() { return _cache.size(); };

		void print_stats() {
            pan::log_DEBUG("<Strategy cache statistics>");
			pan::log_DEBUG("Cache size: ", pan::integer(_cache.size()));
			pan::log_DEBUG("Cache bucket_count: ", pan::integer(_cache.bucket_count()));
            capk::uuidbuf_t uuidbuf;
            StrategyRoute_map::iterator cacheiter;
            cacheiter = _cache.begin();
            while (cacheiter != _cache.end()) {
                pan::log_DEBUG("UUID: ", cacheiter->first.c_str(uuidbuf));
                pan::log_DEBUG("NumNodes: ", pantheios::integer(cacheiter->second.num_nodes));
                pan::log_DEBUG("Routing table: ");
                capk::node_t node;
                capk::route_t route = cacheiter->second;
                for (size_t i = 0; i<cacheiter->second.num_nodes; i++) {
                    route.getNode(i, &node);
                    pan::log_DEBUG("\t", pantheios::integer(*((int*)(node.data()))));
                }
                cacheiter++;
            }

            pan::log_DEBUG("</Strategy cache statistics>");
		}

	private:
		StrategyRoute_map _cache;	
		
};


#define MSG_CACHE_INIT_SIZE 512
typedef dense_hash_map<capk::order_id_t, capk::OrderInfo_ptr, std::tr1::hash<capk::order_id>, capk::eq_order_id> OrderInfo_map;
class KOrderCache
{
	public:
		KOrderCache(size_t init_size = MSG_CACHE_INIT_SIZE): _cache(init_size) {
            capk::order_id empty("");
			_cache.set_empty_key(empty);
            capk::order_id deleted("1");
			_cache.set_deleted_key(deleted);
		}
		~KOrderCache() {};

		void add(capk::order_id_t& key, capk::OrderInfo_ptr& val) {
			_cache[key] = val;
		};

		size_t del(const capk::order_id_t& key) {
			return _cache.erase(key);
		};

        capk::OrderInfo_ptr get(capk::order_id_t& key) {
			if (_cache.find(key) != _cache.end()) {
				return _cache[key];
			}
			return capk::OrderInfo_ptr();
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
				pan::log_CRITICAL("Can't open order cache file: ", filename, " error: ", strerror(errno));
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
				pan::log_CRITICAL("Can't open order cache file: ", filename, " - ABORTING", " error: ", strerror(errno));
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

