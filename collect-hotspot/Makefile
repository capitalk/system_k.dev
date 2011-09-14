all: itch


INC=-I/usr/local/boost

CXXFLAGS=-g -O2 $(INC)

ORDERBOOK_DIR=../orderBook

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

LIBS=-L/usr/local/boost_1_45_0/stage/lib/ -L../orderBook
LDFLAGS=-lrt -lpthread -lboost_program_options -lboost_filesystem -lOB2

itch: $(OBJS) libOB2.a
	g++ $(INC) -o itch $(LIBS) $(LDFLAGS) $(OBJS)

libOB2.a:
	cd $(ORDERBOOK_DIR) && make -f Makefile.book2


clean:
	rm -f $(OBJS) itch

