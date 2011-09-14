#include "HotspotFxCancelOrderMessage.h"

#include <stdio.h>

#include "../MessageFactory.h"
#include "../PacketBase.h"

namespace itch
{

HotspotFxCancelOrderMessage::HotspotFxCancelOrderMessage()
        : MessageBase ( TYPE )
{
}

bool HotspotFxCancelOrderMessage::Load(PacketBase& packet)
{
    if (!packet.ReadString(m_strCurrencyPair, 7))
        return false;
    if (!packet.ReadString(m_strOrderID, 15))
        return false;
    return MessageBase::Load(packet);
}

void HotspotFxCancelOrderMessage::LogMessage(FILE* pLogFile) const
{
    MessageBase::LogMessage(pLogFile);
    fprintf(pLogFile, SESSION_LOG_FIELD_DELIMITER "%s"
            SESSION_LOG_FIELD_DELIMITER "%s",
            m_strCurrencyPair.c_str(),
            m_strOrderID.c_str());
}

static MessageBase *CREATE() {
    return new HotspotFxCancelOrderMessage();
}
static MessageFactoryRegistry __registry(MESSAGE_HOTSPOTFX_NAMESPACE, HotspotFxCancelOrderMessage::TYPE, CREATE);

}
