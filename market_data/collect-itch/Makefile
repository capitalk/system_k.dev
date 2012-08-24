all: itch


INCLUDE=-I/usr/local/boost

CXXFLAGS=-g -O2 $(INCLUDE)

ORDERBOOK_DIR=../../order_book/order_book.v2

OBJS=main.o \
	Session.o \
	Socket.o \
	Logging.o \
	ExpandableRingBuffer.o \
	./msg/hotspotfx/HotspotFxModifyOrderMessage.o \
	./msg/hotspotfx/HotspotFxMarketSnapshotMessage.o \
	./msg/hotspotfx/HotspotFxTickerMessage.o \
	./msg/hotspotfx/HotspotFxNewOrderMessage.o \
	./msg/hotspotfx/HotspotFxCancelOrderMessage.o \
	./msg/MessageBase.o \
	./msg/MessageFactory.o \
	./msg/server/ServerUnsupportedMessage.o \
	./msg/server/ServerInstrumentDirectoryMessage.o \
	./msg/server/ServerSequencedDataMessage.o \
	./msg/server/ServerLoginAcceptedMessage.o \
	./msg/server/ServerLoginRejectedMessage.o \
	./msg/server/ServerHeartbeatMessage.o \
	./msg/server/ServerErrorNotificationMessage.o \
	./msg/client/ClientLogoutMessage.o \
	./msg/client/ClientTickerSubscribeMessage.o \
	./msg/client/ClientMarketSnapshotMessage.o \
	./msg/client/ClientHeartbeatMessage.o \
	./msg/client/ClientMarketDataSubscribeMessage.o \
	./msg/client/ClientInstrumentDirectoryMessage.o \
	./msg/client/ClientCurrencyPairMessage.o \
	./msg/client/ClientTickerUnsubscribeMessage.o \
	./msg/client/ClientLoginMessage.o \
	./msg/client/ClientMarketDataUnsubscribeMessage.o \
	./msg/PacketBase.o

LIBS= -L$(ORDERBOOK_DIR) -lrt -lpthread -lboost_program_options -lboost_filesystem -lorder_book2

itch: $(OBJS) liborder_book2.a
	g++ $(INCLUDE) -o itch $(LIBS) $(LDFLAGS) $(OBJS)

liborder_book2.a:
	cd $(ORDERBOOK_DIR) && make 


clean:
	rm -f $(OBJS) itch

