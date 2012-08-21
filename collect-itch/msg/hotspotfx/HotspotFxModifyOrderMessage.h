#ifndef HOTSPOTFXMODIFYORDERMESSAGE_H
#define HOTSPOTFXMODIFYORDERMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class HotspotFxModifyOrderMessage : public MessageBase
{
public:
    static const char TYPE = 'M';

    HotspotFxModifyOrderMessage();

    bool Load ( PacketBase& packet );

    void LogMessage ( FILE *pLogFile ) const;

    const std::string& GetCurrencyPair() const {
        return m_strCurrencyPair;
    }
    const std::string& GetOrderID() const {
        return m_strOrderID;
    }
    double GetAmount() const {
        return m_lfAmount;
    }
    double GetMinQty() const {
        return m_lfMinQty;
    }
    double GetLotSize() const {
        return m_lfLotSize;
    }

protected:
    std::string m_strCurrencyPair;
    std::string m_strOrderID;
    double m_lfAmount;
    double m_lfMinQty;
    double m_lfLotSize;
};

}

#endif // HOTSPOTFXMODIFYORDERMESSAGE_H
