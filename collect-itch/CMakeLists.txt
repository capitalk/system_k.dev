cmake_minimum_required(VERSION 2.8)

PROJECT(itch)

ADD_EXECUTABLE(itch ExpandableRingBuffer.cpp msg/hotspotfx/HotspotFxTickerMessage.cpp msg/hotspotfx/HotspotFxMarketSnapshotMessage.cpp msg/hotspotfx/HotspotFxCancelOrderMessage.cpp msg/hotspotfx/HotspotFxModifyOrderMessage.cpp msg/PacketBase.cpp msg/server/ServerInstrumentDirectoryMessage.cpp msg/hotspotfx/HotspotFxNewOrderMessage.cpp msg/server/ServerUnsupportedMessage.cpp msg/client/ClientInstrumentDirectoryMessage.cpp msg/client/ClientMarketDataUnsubscribeMessage.cpp msg/client/ClientMarketDataSubscribeMessage.cpp msg/client/ClientTickerUnsubscribeMessage.cpp msg/client/ClientCurrencyPairMessage.cpp msg/client/ClientTickerSubscribeMessage.cpp msg/client/ClientMarketSnapshotMessage.cpp msg/server/ServerErrorNotificationMessage.cpp msg/client/ClientLogoutMessage.cpp msg/server/ServerSequencedDataMessage.cpp msg/server/ServerHeartbeatMessage.cpp msg/server/ServerLoginRejectedMessage.cpp msg/server/ServerLoginAcceptedMessage.cpp msg/MessageFactory.cpp msg/client/ClientHeartbeatMessage.cpp msg/client/ClientLoginMessage.cpp msg/MessageBase.cpp Logging.cpp Socket.cpp Session.cpp main.cpp)
TARGET_LINK_LIBRARIES(itch rt pthread boost_program_options boost_filesystem)

add_subdirectory(msg)