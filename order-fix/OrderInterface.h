#ifndef __ORDER_INTERFACE__
#define __ORDER_INTERFACE__

#include "utils/KTypes.h"
#include "KMsgCache.h"

namespace capk
{

class OrderInterface
{
	public: 
		OrderInterface() {};

		virtual ~OrderInterface() {};

		virtual void sndNewOrder(order_id_t& ClOrdID, 
							const char* Symbol,
							const side_t Side, 
							const double OrderQty,
							const short int OrdType,
							const double Price,
							const short int TimeInForce = 0,
							const char* Account = NULL) {};

		virtual void sndCancelOrder(order_id_t& ClOrdID, 
									order_id_t& OrigOrderID) {};

		virtual void sndOrderCancelReplace() {};

		virtual void sndOrderStatus() {};

		virtual void rcvExecutionReport() {};

		virtual void rcvListstatus() {};

};
};
#endif // __ORDER_INTERFACE__
