#ifndef __ORDER_INTERFACE__
#define __ORDER_INTERFACE__

#include "utils/KTypes.h"
#include "KMsgCache.h"

namespace capk
{

class OrderInterface
{
	public: 

		OrderInterface():_ctx(NULL), _in(""), _out("") {};
		OrderInterface(zmq::context_t* ctx, const char* inaddr, const char* outaddr):_ctx(ctx), _in(inaddr), _out(outaddr) {};

		virtual ~OrderInterface() {};

		virtual void dispatch(int msgType,
								char* data, // DELETE [] WHEN DONE
								size_t len) {};
/*
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
*/
		const std::string& getInAddr();
		void setInAddr(const char* inaddr) { _in = inaddr;};
		const std::string& getOutAddr();	
		void setOutAddr(const char* outaddr) { _out = outaddr;};	
		virtual bool rcv(zmq::message_t& in) {};
		virtual bool snd(zmq::message_t& out) {};

	private:
		zmq::context_t* _ctx;
		std::string _in;
		std::string _out;

};
};
#endif // __ORDER_INTERFACE__
