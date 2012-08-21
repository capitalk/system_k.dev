#include "HotspotFxNewOrderMessage.h"

#include <stdio.h>

#include "../PacketBase.h"
#include "../MessageFactory.h"

namespace itch
{

HotspotFxNewOrderMessage::HotspotFxNewOrderMessage()
        : MessageBase ( TYPE )
        , m_cBuyOrSell ( '\0' )
        , m_lfAmount ( 0 )
        , m_lfMinQty ( 0 )
        , m_lfLotSize ( 0 )
{
}

bool HotspotFxNewOrderMessage::Load ( PacketBase& packet )
{
    if (!packet.ReadChar(m_cBuyOrSell))
        return false;
    if (!packet.ReadString(m_strCurrencyPair, 7))
        return false;
    if (!packet.ReadString(m_strOrderID, 15))
        return false;
    if (!packet.ReadString(m_strPrice, 10))
        return false;
    if (!packet.ReadDbl(m_lfAmount, 16))
        return false;
    if (!packet.ReadDbl(m_lfMinQty, 16))
        return false;
    if (!packet.ReadDbl(m_lfLotSize, 16))
        return false;
    return MessageBase::Load(packet);
}

void HotspotFxNewOrderMessage::LogMessage(FILE* pLogFile) const
{
    MessageBase::LogMessage(pLogFile);
    fprintf(pLogFile, SESSION_LOG_FIELD_DELIMITER "%c"
            SESSION_LOG_FIELD_DELIMITER "%s"
            SESSION_LOG_FIELD_DELIMITER "%s"
            SESSION_LOG_FIELD_DELIMITER "%s"
            SESSION_LOG_FIELD_DELIMITER "%lf"
            SESSION_LOG_FIELD_DELIMITER "%lf"
            SESSION_LOG_FIELD_DELIMITER "%lf",
            m_cBuyOrSell,
            m_strCurrencyPair.c_str(),
            m_strOrderID.c_str(),
            m_strPrice.c_str(),
            m_lfAmount,
            m_lfMinQty,
            m_lfLotSize);
}


static MessageBase *CREATE() {
    return new HotspotFxNewOrderMessage();
}
static MessageFactoryRegistry __registry(MESSAGE_HOTSPOTFX_NAMESPACE, HotspotFxNewOrderMessage::TYPE, CREATE);

}
