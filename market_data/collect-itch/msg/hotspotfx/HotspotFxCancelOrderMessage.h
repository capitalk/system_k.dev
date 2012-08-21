#ifndef HOTSPOTFXCANCELORDERMESSAGE_H
#define HOTSPOTFXCANCELORDERMESSAGE_H

#include "../MessageBase.h"

namespace itch
{

class HotspotFxCancelOrderMessage : public MessageBase
{
public:
    static const char TYPE = 'X';

    HotspotFxCancelOrderMessage();

    bool Load (PacketBase& packet);

    void LogMessage(FILE *pLogFile) const;

    const std::string& GetCurrencyPair() const {
        return m_strCurrencyPair;
    }
    const std::string& GetOrderID() const {
        return m_strOrderID;
    }

protected:
    std::string m_strCurrencyPair;
    std::string m_strOrderID;
};

}

#endif // HOTSPOTFXCANCELORDERMESSAGE_H
