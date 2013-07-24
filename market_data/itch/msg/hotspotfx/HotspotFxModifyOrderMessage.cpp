#include "HotspotFxModifyOrderMessage.h"

#include <stdio.h>

#include "../MessageFactory.h"
#include "../PacketBase.h"

namespace itch
{

HotspotFxModifyOrderMessage::HotspotFxModifyOrderMessage()
        : MessageBase ( TYPE )
        , m_lfAmount ( 0 )
        , m_lfMinQty ( 0 )
        , m_lfLotSize ( 0 )
{
}

bool HotspotFxModifyOrderMessage::Load(PacketBase& packet)
{
    if (!packet.ReadString(m_strCurrencyPair, 7))
        return false;
    if (!packet.ReadString(m_strOrderID, 15))
        return false;
    if (!packet.ReadDbl(m_lfAmount, 16))
        return false;
    if (!packet.ReadDbl(m_lfMinQty, 16))
        return false;
    if (!packet.ReadDbl(m_lfLotSize, 16))
        return false;
    return MessageBase::Load(packet);
}

void HotspotFxModifyOrderMessage::LogMessage(FILE* pLogFile) const
{
    MessageBase::LogMessage(pLogFile);
    fprintf(pLogFile, SESSION_LOG_FIELD_DELIMITER "%s"
            SESSION_LOG_FIELD_DELIMITER "%s"
            SESSION_LOG_FIELD_DELIMITER "%lf"
            SESSION_LOG_FIELD_DELIMITER "%lf"
            SESSION_LOG_FIELD_DELIMITER "%lf",
            m_strCurrencyPair.c_str(),
            m_strOrderID.c_str(),
            m_lfAmount,
            m_lfMinQty,
            m_lfLotSize);
}

static MessageBase *CREATE() {
    return new HotspotFxModifyOrderMessage();
}
static MessageFactoryRegistry __registry(MESSAGE_HOTSPOTFX_NAMESPACE, HotspotFxModifyOrderMessage::TYPE, CREATE);

}
