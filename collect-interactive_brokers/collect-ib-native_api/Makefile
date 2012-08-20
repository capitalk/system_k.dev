CXX=g++
CXXFLAGS=-DIB_USE_STD_STRING -Wall -Wno-switch
ROOT_DIR=..
COLLECTOR=../..
PROTO=../../proto
BASE_SRC_DIR=${ROOT_DIR}/PosixSocketClient
INCLUDES=-I${ROOT_DIR}/Shared/ \
		 -I${BASE_SRC_DIR} \
		 -I${COLLECTOR}
TARGET=collect-interactive_brokers
LIBS= -lrt -lzmq -lprotobuf

INCLUDE=

all: $(TARGET)

include $(PROTO)/Makefile.inc

$(TARGET): Main.cpp PosixTestClient.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o EClientSocketBase.o -c $(BASE_SRC_DIR)/EClientSocketBase.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o EPosixClientSocket.o -c $(BASE_SRC_DIR)/EPosixClientSocket.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o PosixTestClient.o -c PosixTestClient.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o interactive_brokers.pb.o -c $(PROTO)/interactive_brokers.pb.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o capk_globals.pb.o -c $(PROTO)/capk_globals.pb.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o Main.o -c Main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(LIBS) EClientSocketBase.o EPosixClientSocket.o PosixTestClient.o Main.o interactive_brokers.pb.o capk_globals.pb.o

protos:
	cd $(PROTO) && make

clean:
	rm -f $(TARGET) *.o

