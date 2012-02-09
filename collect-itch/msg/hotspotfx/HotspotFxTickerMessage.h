#ifndef HOTSPOTFXTICKERMESSAGE_H
#define HOTSPOTFXTICKERMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class HotspotFxTickerMessage : public MessageBase
{
public:
    static const char TYPE = 'T';

    HotspotFxTickerMessage();

    bool Load(PacketBase& packet);

    void LogMessage(FILE *pLogFile) const;

    char GetBuyOrSell() const {
        return m_cBuyOrSell;
    }
    bool IsBuy() const {
        return m_cBuyOrSell == 'B';
    }
    bool IsSell() const {
        return m_cBuyOrSell == 'S';
    }
    const std::string& GetCurrencyPair() const {
        return m_strCurrencyPair;
    }
    const std::string& GetPrice() const {
        return m_strPrice;
    }
    const std::string& GetTransactionDate() const {
        return m_strTransactionDate;
    }
    const std::string& GetTransactionTime() const {
        return m_strTransactionTime;
    }

protected:
    char m_cBuyOrSell;
    std::string m_strCurrencyPair;
    std::string m_strPrice;
    std::string m_strTransactionDate;
    std::string m_strTransactionTime;
};

}

#endif // HOTSPOTFXTICKERMESSAGE_H
