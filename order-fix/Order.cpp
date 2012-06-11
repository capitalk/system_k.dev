#include "Order.h"

namespace capk {

Order::Order() : _oid(false) 
{
}

Order::Order(order_id_t& oid): _oid(oid) 
{ 

};


Order::Order(const Order& o) 
{
    std::cerr << "COPY CTOR Order(const Order& rhs)" << std::endl;
};

Order& 
Order::operator=(const Order& rhs) 
{
    if (&rhs == this) {
        return *this;
    }
    std::cerr << "ASSGN OPR Order& operator=(const Order& rhs)" << std::endl;
    return *this;

};

Order::~Order() 
{ 

};	


void
Order::set(capkproto::execution_report& er) 
{
    _oid.set(er.cl_order_id().c_str(), er.cl_order_id().size());
    _origClOid.set(er.orig_cl_order_id().c_str(), er.orig_cl_order_id().size());
    memcpy(_execId, er.exec_id().c_str(), er.exec_id().size());
    _execTransType = static_cast<capk::exec_trans_type_t>(er.exec_trans_type());
    _ordStatus = static_cast<capk::ord_status_t>(er.order_status());
    _execType = static_cast<capk::exec_type_t>(er.exec_type());
    memcpy(_symbol, er.symbol().c_str(), er.symbol().size()); 
    std::string security_type = er.security_type();
    // assert(security_type == capk::SEC_TYPE_FOR);
    capkproto::side_t side = er.side();
    if (side == capkproto::BID) {
        _side = BID;
    }
    else if (side == capkproto::ASK) {
        _side = ASK;
    }
    _orderQty = er.order_qty();
    _ordType = er.ord_type();
    _price = er.price();
    _lastShares = er.last_shares();
    _lastPrice = er.last_price();
    _leavesQty = er.leaves_qty();
    _cumQty = er.cum_qty();
    _avgPrice = er.avg_price();
    _timeInForce = er.time_in_force();
    _transactTimeStr = er.transact_time();
    //FIXConverters::UTCTimestampStringToTimespec(_strTransactTime, &_transactTime);
    _execInstStr = er.exec_inst();
    _handlInst = er.handl_inst();
    _ordRejReason = er.order_reject_reason();
    _minQty = er.min_qty();

};


} // namespace capk

