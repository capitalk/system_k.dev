CC=g++
LIBS= -lzmq

zmq_version: zmq_version.cpp
	$(CC) zmq_version.cpp $(LIBS) -o $@

