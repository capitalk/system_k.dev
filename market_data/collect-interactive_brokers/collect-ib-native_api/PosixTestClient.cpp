#include "PosixTestClient.h"

#include "EPosixClientSocket.h"
#include "EPosixClientSocketPlatform.h"

#include "Contract.h"
#include "Order.h"

#include "utils/constants.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <zmq.hpp>

const int PING_DEADLINE = 2; // seconds
const int SLEEP_BETWEEN_PINGS = 30; // seconds

///////////////////////////////////////////////////////////
// member funcs
PosixTestClient::PosixTestClient()
	: m_pClient(new EPosixClientSocket(this))
	, m_state(ST_CONNECT)
	, m_sleepDeadline(0)
	, m_orderId(0)
    , m_zmqContext(1)
    , m_pubSock(m_zmqContext, ZMQ_PUB)
{
}

PosixTestClient::~PosixTestClient()
{
}

bool PosixTestClient::connect(const char *host, unsigned int port, int clientId)
{
	// trying to connect
    int zero = 0;
	fprintf(stderr,  "Connecting to %s:%d clientId:%d\n", !( host && *host) ? "127.0.0.1" : host, port, clientId);

	bool bRes = m_pClient->eConnect( host, port, clientId);

    //m_pubSock = zmq::socket_t(&m_zmqContext);
    m_pubSock.setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
    m_pubSock.bind(capk::IBRK_BROADCAST_ADDR); 

	if (bRes) {
		fprintf(stderr,  "Connected to %s:%d clientId:%d\n", !( host && *host) ? "127.0.0.1" : host, port, clientId);
	}
	else {
		fprintf(stderr,  "Cannot connect to %s:%d clientId:%d\n", !( host && *host) ? "127.0.0.1" : host, port, clientId);
    }

	return bRes;
}

void PosixTestClient::disconnect() const
{
	m_pClient->eDisconnect();

	printf ( "Disconnected\n");
}

bool PosixTestClient::isConnected() const
{
	return m_pClient->isConnected();
}

void PosixTestClient::subscribe(const TickerId id, 
        const Contract& contract, 
        const IBString& ticklist, 
        const bool snapshot)
{
    capk::IBSingleMarketBBO_t bbo;
    memcpy(&bbo.symbol, contract.symbol.c_str(), SYMBOL_LEN);
    memcpy(&bbo.currency, contract.currency.c_str(), CURRENCY_SYMBOL_LEN);
    clock_gettime(CLOCK_REALTIME, &bbo.last_update);
    contractPriceMap[id] = bbo;
    m_pClient->reqMktData(id, contract, ticklist, false);
}



int PosixTestClient::handleTickSize(const TickerId tickerId, 
        TickType field, 
        double size)
{
    std::map<TickerId, capk::IBSingleMarketBBO_t>::iterator it = contractPriceMap.find(tickerId);
    if (it == contractPriceMap.end()) {
        fprintf(stderr, "TickerId %d NOT FOUND in contract map (size)\n", static_cast<int>(tickerId));
        return (-1);
    }
    else {
        if (field == BID_SIZE) { 
            //fprintf(stderr, "Setting bid size\n");
            clock_gettime(CLOCK_REALTIME, &(it->second.last_update));
            it->second.bid_size = size; 
        }
        else if (field == ASK_SIZE) {
            //fprintf(stderr, "Setting ask size\n");
            clock_gettime(CLOCK_REALTIME, &(it->second.last_update));
            it->second.ask_size = size;
        }
        else if (field == LAST_SIZE) {
            //fprintf(stderr, "Setting last size\n");
            clock_gettime(CLOCK_REALTIME, &(it->second.last_update));
            it->second.last_size = size;
        }
    }
    fprintf(stderr, "%s %f %f - %f %f (last %f %f)\n", it->second.symbol, it->second.bid_size, it->second.bid_price, it->second.ask_price, it->second.ask_size, it->second.last_size, it->second.last_price);
    zmqPublishTick(it->second);
    return (-1);
}

void PosixTestClient::zmqPublishTick(const capk::IBSingleMarketBBO_t& ib)
{
    capkproto::interactive_brokers_bbo ib_bbo;
    bool sndOK;
    ib_bbo.set_symbol(ib.symbol);
    ib_bbo.set_bid_price(ib.bid_price);
    ib_bbo.set_ask_price(ib.ask_price);
    ib_bbo.set_bid_size(ib.bid_size);
    ib_bbo.set_ask_size(ib.ask_size);
    //ib_bbo.set_last_price(ib.last_price);
    //ib_bbo.set_last_size(ib.last_size);
    ib_bbo.set_venue_id(capk::IBRK_VENUE_ID);
    char msgbuf[256];
    size_t msgsize = ib_bbo.ByteSize();
    assert(msgsize < sizeof(msgbuf));
    ib_bbo.SerializeToArray(msgbuf, msgsize);
    zmq::message_t msg(msgsize);
    memcpy(msg.data(), msgbuf, msgsize);
    fprintf(stderr, "Sending tick message:\n %s" PRIuPTR "\n", ib_bbo.DebugString().c_str(), msgsize);
    sndOK = m_pubSock.send(msg, 0);
    assert(sndOK);
}

int PosixTestClient::handleTickPrice(const TickerId tickerId, 
        TickType field, 
        double price, 
        int canAutoExecute)
{
    std::map<TickerId, capk::IBSingleMarketBBO_t>::iterator it = contractPriceMap.find(tickerId);
    if (it == contractPriceMap.end()) {
        fprintf(stderr, "TickerId %d NOT FOUND in contract map (price)\n", static_cast<int>(tickerId));
        return (-1);
    }
    else {
        if (field == BID) { 
            //fprintf(stderr, "Setting bid price\n");
            clock_gettime(CLOCK_REALTIME, &(it->second.last_update));
            it->second.bid_price = price; 
        }
        else if (field == ASK) {
            //fprintf(stderr, "Setting ask price\n");
            clock_gettime(CLOCK_REALTIME, &(it->second.last_update));
            it->second.ask_price = price;
        }
        else if (field == LAST) {
            //fprintf(stderr, "Setting last price\n");
            clock_gettime(CLOCK_REALTIME, &(it->second.last_update));
            it->second.last_price = price;
        }
    }
    fprintf(stderr, "%s %f %f - %f %f (last %f %f)\n", it->second.symbol, it->second.bid_size, it->second.bid_price, it->second.ask_price, it->second.ask_size, it->second.last_size, it->second.last_price);
    zmqPublishTick(it->second);
    return (-1);
}

void PosixTestClient::processMessages()
{
    m_pClient->checkMessages();
    /*
    if (m_pClient->checkMessages()) {
        fprintf(stderr, "checkMessages returned TRUE\n");
    }
    else {
        fprintf(stderr, "checkMessages returned FALSE\n");
    }
    */
}
/*
void PosixTestClient::processMessages()
{
	fd_set readSet, writeSet, errorSet;
    
	struct timeval tval;
	tval.tv_usec = 0;
	tval.tv_sec = 0;

	time_t now = time(NULL);
    fprintf(stderr, "STATE: %d", m_state);
	switch (m_state) {
		case ST_PLACEORDER:
			//placeOrder();
			break;
		case ST_PLACEORDER_ACK:
			break;
		case ST_CANCELORDER:
			//cancelOrder();
			break;
		case ST_CANCELORDER_ACK:
			break;
		case ST_PING:
			reqCurrentTime();
			break;
		case ST_PING_ACK:
			if( m_sleepDeadline < now) {
				disconnect();
				return;
			}
			break;
		case ST_IDLE:
			if( m_sleepDeadline < now) {
				m_state = ST_PING;
				return;
			}
			break;
	}

	if( m_sleepDeadline > 0) {
		// initialize timeout with m_sleepDeadline - now
		tval.tv_sec = m_sleepDeadline - now;
	}

	if( m_pClient->fd() >= 0 ) {

		FD_ZERO( &readSet);
		errorSet = writeSet = readSet;

		FD_SET( m_pClient->fd(), &readSet);

		if( !m_pClient->isOutBufferEmpty())
			FD_SET( m_pClient->fd(), &writeSet);

		FD_CLR( m_pClient->fd(), &errorSet);

		int ret = select( m_pClient->fd() + 1, &readSet, &writeSet, &errorSet, &tval);

		if( ret == 0) { // timeout
			return;
		}

		if( ret < 0) {	// error
			disconnect();
			return;
		}

		if( m_pClient->fd() < 0)
			return;

		if( FD_ISSET( m_pClient->fd(), &errorSet)) {
			// error on socket
			m_pClient->onError();
		}

		if( m_pClient->fd() < 0)
			return;

		if( FD_ISSET( m_pClient->fd(), &writeSet)) {
			// socket is ready for writing
			m_pClient->onSend();
		}

		if( m_pClient->fd() < 0)
			return;

		if( FD_ISSET( m_pClient->fd(), &readSet)) {
			// socket is ready for reading
			m_pClient->onReceive();
		}
	}
}
*/
//////////////////////////////////////////////////////////////////
// methods
void PosixTestClient::reqCurrentTime()
{
	fprintf(stderr,  "Requesting Current Time\n");

	// set ping deadline to "now + n seconds"
	m_sleepDeadline = time( NULL) + PING_DEADLINE;

	m_state = ST_PING_ACK;

	m_pClient->reqCurrentTime();
}

/*
void PosixTestClient::placeOrder()
{
	Contract contract;
	Order order;

	contract.symbol = "MSFT";
	contract.secType = "STK";
	contract.exchange = "SMART";
	contract.currency = "USD";

	order.action = "BUY";
	order.totalQuantity = 1000;
	order.orderType = "LMT";
	order.lmtPrice = 0.01;

	fprintf(stderr,  "Placing Order %ld: %s %ld %s at %f\n", m_orderId, order.action.c_str(), order.totalQuantity, contract.symbol.c_str(), order.lmtPrice);

	m_state = ST_PLACEORDER_ACK;

	m_pClient->placeOrder( m_orderId, contract, order);
}
*/
/*
void PosixTestClient::cancelOrder()
{
	fprintf(stderr,  "Cancelling Order %ld\n", m_orderId);

	m_state = ST_CANCELORDER_ACK;

	m_pClient->cancelOrder( m_orderId);
}
*/


///////////////////////////////////////////////////////////////////
// events
void PosixTestClient::orderStatus( OrderId orderId, const IBString &status, int filled,
	   int remaining, double avgFillPrice, int permId, int parentId,
	   double lastFillPrice, int clientId, const IBString& whyHeld)

{
	if( orderId == m_orderId) {
		if( m_state == ST_PLACEORDER_ACK && (status == "PreSubmitted" || status == "Submitted"))
			m_state = ST_CANCELORDER;

		if( m_state == ST_CANCELORDER_ACK && status == "Cancelled")
			m_state = ST_PING;

		fprintf(stderr,  "Order: id=%ld, status=%s\n", orderId, status.c_str());
	}
}

void PosixTestClient::nextValidId( OrderId orderId)
{
	m_orderId = orderId;

	//m_state = ST_PLACEORDER;
}

void PosixTestClient::currentTime( long time)
{
	if ( m_state == ST_PING_ACK) {
		time_t t = ( time_t)time;
		struct tm * timeinfo = localtime ( &t);
		fprintf(stderr,  "The current date/time is: %s", asctime( timeinfo));

		time_t now = ::time(NULL);
		m_sleepDeadline = now + SLEEP_BETWEEN_PINGS;

		m_state = ST_IDLE;
	}
}

void PosixTestClient::error(const int id, const int errorCode, const IBString errorString)
{
	fprintf(stderr,  "Error id=%d, errorCode=%d, msg=%s\n", id, errorCode, errorString.c_str());

	if( id == -1 && errorCode == 1100) // if "Connectivity between IB and TWS has been lost"
		disconnect();
}

void PosixTestClient::tickPrice( TickerId tickerId, TickType field, double price, int canAutoExecute) 
{
    fprintf(stderr, "tickPrice: %d, %d, %f, %d\n", static_cast<int>(tickerId), field, price, canAutoExecute);
    handleTickPrice(tickerId, field, price, canAutoExecute);
}
void PosixTestClient::tickSize( TickerId tickerId, TickType field, int size) 
{
    fprintf(stderr, "tickSize: %d, %d, %d\n", static_cast<int>(tickerId), field, size);
    handleTickSize(tickerId, field, size);
}
void PosixTestClient::tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
											 double optPrice, double pvDividend,
											 double gamma, double vega, double theta, double undPrice) {}
void PosixTestClient::tickGeneric(TickerId tickerId, TickType tickType, double value) 
{
    fprintf(stderr, "C");
}
void PosixTestClient::tickString(TickerId tickerId, TickType tickType, const IBString& value) 
{
    fprintf(stderr, "D");
}
void PosixTestClient::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const IBString& formattedBasisPoints,
							   double totalDividends, int holdDays, const IBString& futureExpiry, double dividendImpact, double dividendsToExpiry) {}
void PosixTestClient::openOrder( OrderId orderId, const Contract&, const Order&, const OrderState& ostate) {}
void PosixTestClient::openOrderEnd() {}
void PosixTestClient::winError( const IBString &str, int lastError) {}
void PosixTestClient::connectionClosed() 
{
    fprintf(stderr, "E");
}
void PosixTestClient::updateAccountValue(const IBString& key, const IBString& val,
										  const IBString& currency, const IBString& accountName) {}
void PosixTestClient::updatePortfolio(const Contract& contract, int position,
		double marketPrice, double marketValue, double averageCost,
		double unrealizedPNL, double realizedPNL, const IBString& accountName){}
void PosixTestClient::updateAccountTime(const IBString& timeStamp) {}
void PosixTestClient::accountDownloadEnd(const IBString& accountName) {}
void PosixTestClient::contractDetails( int reqId, const ContractDetails& contractDetails) {}
void PosixTestClient::bondContractDetails( int reqId, const ContractDetails& contractDetails) {}
void PosixTestClient::contractDetailsEnd( int reqId) {}
void PosixTestClient::execDetails( int reqId, const Contract& contract, const Execution& execution) {}
void PosixTestClient::execDetailsEnd( int reqId) {}

void PosixTestClient::updateMktDepth(TickerId id, int position, int operation, int side,
									  double price, int size) 
{
    fprintf(stderr, "F");
}
void PosixTestClient::updateMktDepthL2(TickerId id, int position, IBString marketMaker, int operation,
										int side, double price, int size) 
{
    fprintf(stderr, "G");
}
void PosixTestClient::updateNewsBulletin(int msgId, int msgType, const IBString& newsMessage, const IBString& originExch) {}
void PosixTestClient::managedAccounts( const IBString& accountsList) {}
void PosixTestClient::receiveFA(faDataType pFaDataType, const IBString& cxml) {}
void PosixTestClient::historicalData(TickerId reqId, const IBString& date, double open, double high,
									  double low, double close, int volume, int barCount, double WAP, int hasGaps) {}
void PosixTestClient::scannerParameters(const IBString &xml) {}
void PosixTestClient::scannerData(int reqId, int rank, const ContractDetails &contractDetails,
	   const IBString &distance, const IBString &benchmark, const IBString &projection,
	   const IBString &legsStr) {}
void PosixTestClient::scannerDataEnd(int reqId) {}
void PosixTestClient::realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
								   long volume, double wap, int count) {}
void PosixTestClient::fundamentalData(TickerId reqId, const IBString& data) {}
void PosixTestClient::deltaNeutralValidation(int reqId, const UnderComp& underComp) {}
void PosixTestClient::tickSnapshotEnd(int reqId) 
{
    fprintf(stderr, "H");
}

void PosixTestClient::marketDataType(TickerId reqId, int marketDataType) 
{
    fprintf(stderr, "I");
}

/*
bool checkMessages()
{
    fprintf(stderr, "K");
    return true;
}
*/
