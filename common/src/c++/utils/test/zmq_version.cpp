//  g++ -I /opt/src/zeromq-3.2.3/include/ -L /opt/src/zeromq-3.2.3/src -lzmq zmq_version.cpp -o zmq_version
#include <zmq.h> 
#include <stdio.h>

int
main()
{
  int major, minor, patch = 0;
  zmq_version(&major, &minor, &patch);
  printf("Version: %d.%d.%d\n", major, minor, patch);
}
