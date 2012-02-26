#ifndef _K_MSG_CACHE
#define _K_MSG_CACHE

#include <google/dense_hash_map>
#include <uuid/uuid.h>

using google::dense_hash_map;
using std::tr1::hash; // for hash function - TODO change to Murmur3

struct eqstr
{
	bool operator()(const char* s1, const char* s2) const {
		return (s1 == s2) || (s1 && s2 && strcmp(s1, s2) == 0);
	}
}

#define MAX_ROUTES 4
#define MSG_ADDR_LEN 17

typedef struct route 
{
	void* r;
	size_t len;
} route_t;

typedef struct msg_route
{
	unsigned short int size;
	char[MSG_ADDR_LEN] headers[MAX_ROUTES];
} msg_route_t;

typedef order_id_t uuid_t;

class OrderInfo
{
	public:
		OrderInfo();
		~OrderInfO();
		pushRoute(void* addr, size_t len) {
			assert(addr);
			if (addr) {
				assert(path.size < MAX_ROUTES);
				memcpy(&path[path.size], addr, len);
				path.size += 1;
			}
		}
		msg_route* popRoute() {
				if (path.size <= 0) {
					return NULL;	
				}
		}
	private:
		order_id_t oid;
		msg_route_t path;
		uint32_t status;
};

struct cmp_oid
{
	bool operator() (const order_id_t& o1, const order_id_t& o2) const {
		return (&o1 == &o2) || (uuid_compare(o1, o2));
	}
}


class KMsgCache
{
	public:
		KMsgCache();
		~KMsgCache();

		void add(const order_id_t& key, ord_info_t* info);
		void del(const order_id_t& key);

	private:
		dense_hash_map<order_id_t&, msg_route_t* headers, cmp_oid> _hash;
}

#endif // _K_MSG_CACHE
