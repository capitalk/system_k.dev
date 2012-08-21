#ifndef HOTSPOTFXNEWORDERMESSAGE_H
#define HOTSPOTFXNEWORDERMESSAGE_H

#include <string>

#include "../MessageBase.h"

namespace itch
{

class HotspotFxNewOrderMessage : public MessageBase
{
public:
    static const char TYPE = 'N';

    HotspotFxNewOrderMessage();

    inline char GetBuyOrSell() const { return m_cBuyOrSell; }
    inline bool IsBuy() const { return m_cBuyOrSell == 'B';}
    inline bool IsSell() const { return m_cBuyOrSell == 'S';}
    inline const std::string& GetCurrencyPair() const { return m_strCurrencyPair; }
    inline const std::string& GetOrderID() const { return m_strOrderID; }
    inline const std::string& GetPrice() const { return m_strPrice; }
    inline double GetAmount() const { return m_lfAmount; }
    inline double GetMinQty() const { return m_lfMinQty; }
    inline double GetLotSize() const { return m_lfLotSize; }

    bool Load ( PacketBase& packet );

    void LogMessage(FILE *pLogFile) const;

protected:
    char m_cBuyOrSell;
    std::string m_strCurrencyPair;
    std::string m_strOrderID;
    std::string m_strPrice;
    double m_lfAmount;
    double m_lfMinQty;
    double m_lfLotSize;
};

}

#endif // HOTSPOTFXNEWORDERMESSAGE_H
