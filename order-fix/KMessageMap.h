#ifndef _K_MESSAGE_MAP_
#define _K_MESSAGE_MAP_
#include <zmq.h>

// take a FIX message and map it into a protobuf structure for 
// serialization to wire, disk, etc.

class KMessageMap
{
	public:
		KMessageMap();
		~KMessageMap();


	private:

};

#endif // _K_MESSAGE_MAP_
