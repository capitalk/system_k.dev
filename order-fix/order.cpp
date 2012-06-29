#include "order.h"

namespace capk {

Order::Order() : _oid(false) 
{
}

Order::Order(order_id_t& oid): _oid(oid) 
{ 

};


Order::Order(const Order& o) 
{
    pan::log_DEBUG("COPY CTOR Order(const Order& rhs)");
    assign(o);
};

Order& 
Order::operator=(const Order& rhs) 
{
    if (&rhs == this) {
        return *this;
    }
    assign(rhs);
    pan::log_DEBUG("ASSGN OPR Order& operator=(const Order& rhs)");
    return *this;

};

/*
bool
Order::operator==(const Order& rhs) const 
{
    return (this == &rhs || this->_oid == rhs.getOid());
}
*/

Order::~Order() 
{ 

};	


void
Order::set(capkproto::execution_report& er) 
{
    _oid.set(er.cl_order_id().c_str(), er.cl_order_id().size());
    //_oid.parse(er.cl_order_id().c_str());
    _origClOid.set(er.orig_cl_order_id().c_str(), er.orig_cl_order_id().size());
    //_origClOid.parse(er.orig_cl_order_id().c_str());
    memcpy(_execId, er.exec_id().c_str(), er.exec_id().size());
    _execTransType = static_cast<capk::ExecTransType_t>(er.exec_trans_type());
    _ordStatus = static_cast<capk::OrdStatus_t>(er.order_status());
    _execType = static_cast<capk::ExecType_t>(er.exec_type());
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

void
Order::assign(const Order& o)
{
    this->_oid = o.getOid();
    this->_origClOid = o.getOrigClOid();
    memcpy(this->_execId, o.getExecId(), EXEC_ID_LEN);
    this->_execTransType = o.getExecTransType();
    this->_ordStatus = o.getOrdStatus();
    this->_execType = o.getExecType();
    memcpy(this->_symbol, o.getSymbol(), SYMBOL_LEN);
    memcpy(this->_secType, o.getSecType(), SEC_TYPE_LEN);
    this->_side = o.getSide();
    this->_orderQty = o.getOrdQty();
    this->_ordType = o.getOrdType();
    this->_price = o.getPrice();
    this->_lastShares = o.getLastShares();
    this->_lastPrice = o.getLastPrice();
    this->_leavesQty = o.getLeavesQty();
    this->_cumQty = o.getCumQty();
    this->_avgPrice = o.getAvgPrice();
    this->_timeInForce = o.getTimeInForce();
    this->_transactTime = o.getTransactTime();
    this->_transactTimeStr = o.getTransactTimeStr();
    this->_execInstStr = o.getExecInstStr();
    this->_handlInst = o.getHandlInst();
    this->_ordRejReason = o.getOrdRejectReason();
    this->_minQty = o.getMinQty();
}

} // namespace capk

