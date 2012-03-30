#ifndef __NULLORDER_INTERFACE__
#define __NULLORDER_INTERFACE__

#include "OrderInterface.h"
#include "logging.h"

namespace capk
{

class NullOrderInterface : public virtual OrderInterface
{
	public: 
	NullOrderInterface() {
#ifdef LOG
		pan::log_DEBUG("NullOrderInterface()");
#endif
	}

	virtual ~NullOrderInterface() {
#ifdef LOG
		pan::log_DEBUG("~NullOrderInterface()");
#endif
	}
/*
		virtual void sndNewOrder(order_id_t& ClOrdID, 
							const char* Symbol,
							const side_t Side, 
							const double OrderQty,
							const short int OrdType,
							const double Price,
							const short int TimeInForce = 0,
							const char* Account = NULL) {
#ifdef LOG
			pan::log_DEBUG("sndNewOrder()");
			pan::log_DEBUG(pan::integer(Side), " ", Symbol, " ", pan::real(Price));
#endif
		}

		virtual void sndCancelOrder(order_id_t& ClOrdID, 
									order_id_t& OrigOrderID) {
#ifdef LOG
			pan::log_DEBUG("sndCancelOrder()");
#endif
		}

		virtual void sndOrderCancelReplace() {
#ifdef LOG
			pan::log_DEBUG("sndOrderCancelReplace()");
#endif
		}

		virtual void sndOrderStatus() {
#ifdef LOG
			pan::log_DEBUG("sndOrderStatus()");
#endif
		}

		virtual void rcvExecutionReport() {
#ifdef LOG
			pan::log_DEBUG("rcvExecutionReport()");
#endif
		}
	
		virtual void rcvListStatus() {
#ifdef LOG
			pan::log_DEBUG("rcvListStatus()");
#endif
		}
*/

	virtual bool snd(zmq::message_t& rep) {
		return false;
	}

	virtual bool rcv(zmq::message_t& req) {
		return false;
	}
};

}; // namespace capk
#endif // __NULLORDER_INTERFACE__
