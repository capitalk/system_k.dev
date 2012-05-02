#ifndef __ORDER_INTERFACE__
#define __ORDER_INTERFACE__

#include "utils/KTypes.h"
#include "KMsgCache.h"
#include "KMsgTypes.h"

class KMsgProcessor;

namespace capk
{

class OrderInterface
{
	public: 

		OrderInterface(venue_id_t venueID): _mp(NULL), _in(""), _out(""), _venueID(venueID) {};
		OrderInterface(KMsgProcessor* mp, const char* inaddr, const char* outaddr, int venueID):
			 _mp(mp), _in(inaddr), _out(outaddr), _venueID(venueID) {};


		virtual ~OrderInterface() {};

		virtual void dispatch(msg_t msgType,
							char* data, // DELETE [] WHEN DONE
							size_t len) = 0;
		virtual void snd(msg_t msgType, 
						char* data, 
						size_t len) = 0;
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
		const std::string& getInAddr() { return _in; };
		void setInAddr(const char* inaddr) { assert(inaddr != NULL); _in = inaddr;};

		const std::string& getOutAddr() { return _out; };	
		void setOutAddr(const char* outaddr) { assert(outaddr != NULL); _out = outaddr;};	

		KMsgProcessor* getMsgProcessor() { return _mp; };
		void setMsgProcessor(KMsgProcessor* mp) { _mp = mp; };

		inline const venue_id_t getVenueID() { return _venueID;};

	private:
		KMsgProcessor* _mp;
		std::string _in;
		std::string _out;
		venue_id_t _venueID;

};
};
#endif // __ORDER_INTERFACE__
