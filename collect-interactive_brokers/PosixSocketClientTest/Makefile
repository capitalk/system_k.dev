CXX=g++
CXXFLAGS=-DIB_USE_STD_STRING -Wall -Wno-switch
ROOT_DIR=..
COLLECTOR=../..
PROTO_DIR=../../proto
BASE_SRC_DIR=${ROOT_DIR}/PosixSocketClient
INCLUDES=-I${ROOT_DIR}/Shared/ \
		 -I${BASE_SRC_DIR} \
		 -I${COLLECTOR}
TARGET=collect-interactive_brokers
LIBS= -lrt -lzmq -lprotobuf

$(TARGET): Main.cpp PosixTestClient.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o EClientSocketBase.o -c $(BASE_SRC_DIR)/EClientSocketBase.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o EPosixClientSocket.o -c $(BASE_SRC_DIR)/EPosixClientSocket.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o PosixTestClient.o -c PosixTestClient.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o interactive_brokers.pb.o -c $(PROTO_DIR)/interactive_brokers.pb.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o capk_globals.pb.o -c $(PROTO_DIR)/capk_globals.pb.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o Main.o -c Main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(LIBS) EClientSocketBase.o EPosixClientSocket.o PosixTestClient.o Main.o interactive_brokers.pb.o capk_globals.pb.o

clean:
	rm -f $(TARGET) *.o

