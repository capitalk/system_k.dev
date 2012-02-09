#ifndef HOTSPOTFXMARKETSNAPSHOTMESSAGE_H
#define HOTSPOTFXMARKETSNAPSHOTMESSAGE_H

#include <list>

#include "../MessageBase.h"

namespace itch
{

namespace hotspotfx
{

class Order
{
public:
    Order();
    Order(const Order& other);

    bool Load(PacketBase& packet);

    void LogMessage(FILE *pLogFile) const;

    double GetAmount() const {
        return m_lfAmount;
    }
    double GetMinQty() const {
        return m_lfMinQty;
    }
    double GetLotSize() const {
        return m_lfLotSize;
    }
    const std::string& GetOrderID() const {
        return m_strOrderID;
    }

private:
    double m_lfAmount;
    double m_lfMinQty;
    double m_lfLotSize;
    std::string m_strOrderID;
};

class BidOrOffer
{
public:
    BidOrOffer() {}
    BidOrOffer(const BidOrOffer& other);

    bool Load(PacketBase& packet);

    void LogMessage(FILE *pLogFile) const;

    const std::string& GetBidPrice() const {
        return m_strBidPrice;
    }
    const std::list<Order>& GetOrders() const {
        return m_lstOrders;
    }

private:
    std::string m_strBidPrice;
    std::list<Order> m_lstOrders;
};

class CurrencyPair
{
public:
    CurrencyPair() {}
    CurrencyPair(const CurrencyPair& other);

    bool Load(PacketBase &packet);

    void LogMessage(FILE *pLogFile) const;

    const std::string& GetCurrencyPair() const {
        return m_strCurrencyPair;
    }
    const std::list<BidOrOffer>& GetBids() const {
        return m_lstBids;
    }
    const std::list<BidOrOffer>& GetOffers() const {
        return m_lstOffers;
    }

private:
    std::string m_strCurrencyPair;
    std::list<BidOrOffer> m_lstBids;
    std::list<BidOrOffer> m_lstOffers;
};

}

class HotspotFxMarketSnapshotMessage : public MessageBase
{
public:
    static const char TYPE = 'S';

    HotspotFxMarketSnapshotMessage();

    bool Load(PacketBase& packet);

    void LogMessage(FILE *pLogFile) const;

    int GetMessageLength() const {
        return m_nMessageLength;
    }
    const std::list<hotspotfx::CurrencyPair>& GetCurrencyPairs() const {
        return m_lstCurrencyPairs;
    }
protected:
    int m_nMessageLength;
    std::list<hotspotfx::CurrencyPair> m_lstCurrencyPairs;
};

}

#endif // HOTSPOTFXMARKETSNAPSHOTMESSAGE_H
