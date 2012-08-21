#ifdef _WIN32
# include <Windows.h>
# define sleep( seconds) Sleep( seconds * 1000);
#else
# include <unistd.h>
#endif

#include "PosixTestClient.h"
#include "stdio.h"
#include <boost/thread.hpp>
#include <uuid/uuid.h>
#include "Contract.h"

const unsigned MAX_ATTEMPTS = 50;
const unsigned SLEEP_TIME = 10;

int main(int argc, char** argv)
{
	const char* host = argc > 1 ? argv[1] : "";
	unsigned int port = argc > 2 ? atoi(argv[2]) : 4001;//7496;
    if (argc > 3) {
        for (int i = 3; i < argc; i++) {
            printf("SYMBOL %s\n", argv[i]); 
        } 
    }
	int clientId = 0;

    
	unsigned attempt = 0;
	printf( "Start of POSIX Socket Client Test %u\n", attempt);

	for (;;) {
		++attempt;
		printf( "Attempt %u of %u\n", attempt, MAX_ATTEMPTS);

		PosixTestClient client;

		client.connect( host, port, clientId);
        printf("Connected\n");

        while (!client.isConnected()) {
            printf(".");
            sleep(1);
        }

        Contract c;
        c.symbol = "EUR";
        c.secType = "CASH";
        c.currency = "USD";
        c.exchange = "IDEALPRO";

        printf("Begin time request\n");
        client.reqCurrentTime();
        printf("End time request\n");

        printf("Begin subscribe\n");
        //client.subscribe(1, c, "100,101,104,105,106,107,165,221,225,233,236,258,293,294,295,318", false);
        client.subscribe(1, c, "", false);
        printf("End subscribe\n");

        printf("Starting main loop\n");
		while( client.isConnected()) {
			client.processMessages();
		}

		if( attempt >= MAX_ATTEMPTS) {
			break;
		}

		printf( "Sleeping %u seconds before next attempt\n", SLEEP_TIME);
		sleep( SLEEP_TIME);
	}

	printf ( "End of POSIX Socket Client Test\n");
}

