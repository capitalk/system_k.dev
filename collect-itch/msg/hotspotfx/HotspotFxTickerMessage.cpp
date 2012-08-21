#include "HotspotFxTickerMessage.h"

#include <stdio.h>

#include "../MessageFactory.h"
#include "../PacketBase.h"

namespace itch
{

HotspotFxTickerMessage::HotspotFxTickerMessage()
        : MessageBase ( TYPE )
        , m_cBuyOrSell ( '\0' )
{
}

bool HotspotFxTickerMessage::Load(PacketBase& packet)
{
    if (!packet.ReadChar(m_cBuyOrSell))
        return false;
    if (!packet.ReadString(m_strCurrencyPair, 7))
        return false;
    if (!packet.ReadString(m_strPrice, 10))
        return false;
    if (!packet.ReadString(m_strTransactionDate, 8))
        return false;
    if (!packet.ReadString(m_strTransactionTime, 6))
        return false;
    return MessageBase::Load(packet);
}

void HotspotFxTickerMessage::LogMessage(FILE* pLogFile) const
{
    MessageBase::LogMessage(pLogFile);
    fprintf(pLogFile, SESSION_LOG_FIELD_DELIMITER "%c"
            SESSION_LOG_FIELD_DELIMITER "%s"
            SESSION_LOG_FIELD_DELIMITER "%s"
            SESSION_LOG_FIELD_DELIMITER "%s"
            SESSION_LOG_FIELD_DELIMITER "%s",
            m_cBuyOrSell,
            m_strCurrencyPair.c_str(),
            m_strPrice.c_str(),
            m_strTransactionDate.c_str(),
            m_strTransactionTime.c_str());
}


static MessageBase *CREATE() {
    return new HotspotFxTickerMessage();
}
static MessageFactoryRegistry __registry(MESSAGE_HOTSPOTFX_NAMESPACE, HotspotFxTickerMessage::TYPE, CREATE);

}
