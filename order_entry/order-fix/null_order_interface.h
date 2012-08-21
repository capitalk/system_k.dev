#ifndef __NULLORDER_INTERFACE__
#define __NULLORDER_INTERFACE__

#include "proto/capk_globals.pb.h"
#include "proto/new_order_single.pb.h"
#include "proto/order_cancel.pb.h"
#include "proto/execution_report.pb.h"

#include "msg_types.h"
#include "order_interface.h"
#include "logging.h"

#define NULL_INTERFACE_ID 123

namespace capk
{

class NullOrderInterface : public OrderInterface
{
	public: 
	NullOrderInterface(capk::venue_id_t ID, zmq::context_t* ctx): OrderInterface(ID), _ctx(ctx) {
#ifdef LOG
		pan::log_DEBUG("NullOrderInterface()");
#endif
	}

	virtual ~NullOrderInterface() {
#ifdef LOG
		pan::log_DEBUG("~NullOrderInterface()");
#endif
	}

	void 
	run() {
			// Wait for server to bind...
			pan::log_DEBUG("NullOrderInterface sleeping for 3 seconds before starting");
			sleep(3);
			std::string outaddr = getMsgProcessor()->getOutboundAddr();
			int zero = 0;
			//zmq::context_t* ctx = getMsgProcessor()->getZMQContext();
			assert(_ctx);
			//zmq::socket_t inproc(*_ctx, ZMQ_DEALER);
			_inproc = new zmq::socket_t(*_ctx, ZMQ_DEALER);
			assert(_inproc);
			//zmq::socket_t* inproc = new zmq::socket_t(*ctx, ZMQ_DEALER);
			pan::log_DEBUG("NullOrderInterface (outbound) connecting to: ", outaddr.c_str());
			_inproc->connect(outaddr.c_str());
			_inproc->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
			//char sidbuf[UUID_LEN];
            uuidbuf_t sidbuf;
			bool rc;
		while(1) {
				//sleep(1);
				StrategyRoute_map* m = getMsgProcessor()->getStrategyCache()->getCache();
				for (StrategyRoute_map::iterator it = m->begin(); it != m->end(); it++) {
					strategy_id_t sid = it->first;
					pan::log_DEBUG("NullOrderInterface sending SYNTHETIC reply to SID: ", sid.c_str(sidbuf));
					// for routing info lookup - this is removed by receiver and NOT forwarded to client
					// Zero copy
					//zmq::message_t m1(oid.get_uuid(), UUID_LEN, NULL, NULL);
					// Non-ZC
					zmq::message_t m1(sid.size());
					memcpy(m1.data(), sid.get_uuid(), sid.size());
					pan::log_DEBUG("Replying to  SID: ", pan::blob(static_cast<const void*>(sid.get_uuid()), sid.size()));
					rc = _inproc->send(m1, ZMQ_SNDMORE);
					assert(rc);

					// message contents
					// Zero copy - uncomment line below
					//zmq::message_t m2(oid.get_uuid(), UUID_LEN);//, NULL, NULL);
					// Send message with all zero uuid
					order_id_t nullid(false);
					zmq::message_t m2(nullid.size());
					memcpy(m2.data(), nullid.get_uuid(),  nullid.size());
					rc = _inproc->send(m2, 0);
					assert(rc);
				}	
			}
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
};
*/
	virtual void
	snd(msg_t msgType,
		char* data, 
		size_t len)
	{};

	virtual void dispatch(msg_t msgType,
							char* data, // DELETE [] WHEN DONE
							size_t len) {
		assert(data);
		assert(len > 0);
		pan::log_DEBUG("Dispatch received msgType: ", pan::integer(msgType), " along with ", pan::integer(len), " bytes of data");

		order_id_t oid;

		if (msgType == capk::ORDER_NEW) {
			capkproto::new_order_single nos;
			nos.ParseFromArray(data, len);
#ifdef LOG
	        pan::log_DEBUG(nos.DebugString());
#endif
			oid.set(nos.order_id().c_str(), nos.order_id().size());	
			
		}
		else if (msgType == capk::ORDER_CANCEL) {
			capkproto::order_cancel oc;
			oc.ParseFromArray(data, len);
#ifdef LOG
	        pan::log_DEBUG(oc.DebugString());
#endif
				
		}

		
		if (data) {
			delete[] data;
		}
	};


	private: 
	zmq::context_t* _ctx;
	zmq::socket_t* _inproc;
};

}; // namespace capk
#endif // __NULLORDER_INTERFACE__
