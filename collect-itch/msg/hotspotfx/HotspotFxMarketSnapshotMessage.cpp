#include "HotspotFxMarketSnapshotMessage.h"

#include <stdio.h>

#include "../MessageFactory.h"
#include "../PacketBase.h"

namespace itch
{
namespace hotspotfx
{

Order::Order()
        : m_lfAmount(0)
        , m_lfMinQty(0)
        , m_lfLotSize(0)
{
}

Order::Order(const Order& other)
        : m_lfAmount(other.m_lfAmount)
        , m_lfMinQty(other.m_lfMinQty)
        , m_lfLotSize(other.m_lfLotSize)
        , m_strOrderID(other.m_strOrderID)
{
}

bool Order::Load(PacketBase& packet)
{
    if (!packet.ReadDbl(m_lfAmount, 16))
        return false;
    if (!packet.ReadDbl(m_lfMinQty, 16))
        return false;
    if (!packet.ReadDbl(m_lfLotSize, 16))
        return false;
    if (!packet.ReadString(m_strOrderID, 15))
        return false;
    return true;
}

void Order::LogMessage(FILE* pLogFile) const
{
    fprintf(pLogFile, SESSION_LOG_FIELD_DELIMITER "%lf"
            SESSION_LOG_FIELD_DELIMITER "%lf"
            SESSION_LOG_FIELD_DELIMITER "%lf"
            SESSION_LOG_FIELD_DELIMITER "%s",
            m_lfAmount,
            m_lfMinQty,
            m_lfLotSize,
            m_strOrderID.c_str());
}


BidOrOffer::BidOrOffer(const BidOrOffer& other)
        : m_strBidPrice(other.m_strBidPrice)
        , m_lstOrders(other.m_lstOrders)
{
}

bool BidOrOffer::Load(PacketBase& packet)
{
    m_lstOrders.clear();

    if (!packet.ReadString(m_strBidPrice, 10))
        return false;

    int nOrderCount;
    if (!packet.ReadInt(nOrderCount, 4))
        return false;

    while (--nOrderCount >= 0)
    {
        Order order;
        if (!order.Load(packet))
            return false;
        m_lstOrders.push_back(order);
    }

    return true;
}

void BidOrOffer::LogMessage(FILE* pLogFile) const
{
    fprintf(pLogFile, SESSION_LOG_FIELD_DELIMITER "%s"
            SESSION_LOG_FIELD_DELIMITER "%ld",
            m_strBidPrice.c_str(),
            m_lstOrders.size());

    for (std::list<Order>::const_iterator it = m_lstOrders.begin(); it != m_lstOrders.end(); it++)
    {
        (*it).LogMessage(pLogFile);
    }
}



CurrencyPair::CurrencyPair(const CurrencyPair& other)
        : m_strCurrencyPair(other.m_strCurrencyPair)
        , m_lstBids(other.m_lstBids)
        , m_lstOffers(other.m_lstOffers)
{
}

bool CurrencyPair::Load(PacketBase& packet)
{
    m_lstBids.clear();
    m_lstOffers.clear();

    if (!packet.ReadString(m_strCurrencyPair, 7))
        return false;

    int nBidsCount;
    if (!packet.ReadInt(nBidsCount, 4))
        return false;

    while (--nBidsCount >= 0)
    {
        BidOrOffer bid;
        if (!bid.Load(packet))
            return false;
        m_lstBids.push_back(bid);
    }

    int nOffersCount;
    if (!packet.ReadInt(nOffersCount, 4))
        return false;

    while (--nOffersCount >= 0)
    {
        BidOrOffer offer;
        if (!offer.Load(packet))
            return false;
        m_lstOffers.push_back(offer);
    }

    return true;
}

void CurrencyPair::LogMessage(FILE* pLogFile) const
{
    fprintf(pLogFile, SESSION_LOG_FIELD_DELIMITER "%s",
            m_strCurrencyPair.c_str());

    fprintf(pLogFile, SESSION_LOG_FIELD_DELIMITER "%ld",
            m_lstBids.size());
    for (std::list<BidOrOffer>::const_iterator it = m_lstBids.begin(); it != m_lstBids.end(); it++)
        (*it).LogMessage(pLogFile);

    fprintf(pLogFile, SESSION_LOG_FIELD_DELIMITER "%ld",
            m_lstOffers.size());
    for (std::list<BidOrOffer>::const_iterator it = m_lstOffers.begin(); it != m_lstOffers.end(); it++)
        (*it).LogMessage(pLogFile);
}

}

HotspotFxMarketSnapshotMessage::HotspotFxMarketSnapshotMessage()
        : MessageBase ( TYPE )
        , m_nMessageLength ( 0 )
{
}

bool HotspotFxMarketSnapshotMessage::Load(PacketBase& packet)
{
    m_lstCurrencyPairs.clear();

    if (!packet.ReadInt(m_nMessageLength, 6))
        return false;

    int nCurrencyPairCount;
    if (!packet.ReadInt(nCurrencyPairCount, 4))
        return false;

    while (--nCurrencyPairCount >= 0)
    {
        hotspotfx::CurrencyPair currencyPair;
        if (!currencyPair.Load(packet))
            return false;
        m_lstCurrencyPairs.push_back(currencyPair);
    }

    return MessageBase::Load(packet);
}

void HotspotFxMarketSnapshotMessage::LogMessage(FILE* pLogFile) const
{
    MessageBase::LogMessage(pLogFile);
    fprintf(pLogFile, SESSION_LOG_FIELD_DELIMITER "%d"
            SESSION_LOG_FIELD_DELIMITER "%ld",
            m_nMessageLength,
            m_lstCurrencyPairs.size());
    for (std::list<hotspotfx::CurrencyPair>::const_iterator it = m_lstCurrencyPairs.begin();
            it != m_lstCurrencyPairs.end(); it++)
    {
        (*it).LogMessage(pLogFile);
    }
}


static MessageBase *CREATE() {
    return new HotspotFxMarketSnapshotMessage();
}
static MessageFactoryRegistry __registry(MESSAGE_HOTSPOTFX_NAMESPACE, HotspotFxMarketSnapshotMessage::TYPE, CREATE);
}
