#include <boost/program_options.hpp>
using namespace boost::program_options;

#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
using namespace std;

#include "Session.h"
#include "./msg/server/ServerSequencedDataMessage.h"
using namespace itch;

#include <sys/times.h>
#include <sys/resource.h>

class CustomSession : public Session
{
public:
    inline CustomSession ( const char *pszHostname, int nPort, const char *pszSessionLogFile )
            : Session ( pszHostname, nPort, pszSessionLogFile )
    {}

protected:
    void OnLoginRejected ( const ServerLoginRejectedMessage& msg )
    {
        cerr << "FATAL ERROR: the server rejected the login request. exiting..." << endl;
        exit ( 2 );
    }
    
    void OnMarketSnapshot( const ServerSequencedDataMessage& msg ) 
    {
        cerr << "Snapshot received" << std::endl; 
    }

    void OnNewOrder( const ServerSequencedDataMessage& msg )  
    {

        cerr << "New order received" << std::endl; 
    }

    void OnModifyOrder( const ServerSequencedDataMessage& msg ) 
    {

        cerr << "Modify order received" << std::endl; 
    }

    void OnCancelOrder( const ServerSequencedDataMessage& msg ) 
    {
        cerr << "Cancel order received" << std::endl; 

    }

    void OnTicker( const ServerSequencedDataMessage& msg ) 
    {
        cerr << "Ticker received" << std::endl; 

    }
};

int main ( int argc, char **argv )
{

    setvbuf(stdout, NULL, _IONBF, NULL);
    setvbuf(stderr, NULL, _IONBF, NULL);

    try
    {
        string strConfigFile;

        string strHostname;
        int nPort;

        string strUsername;
        string strPassword;
        bool bMarketDataUnsubscribe = false;

        string strSessionLogFile;
        string strSymbolsFile;

        int nRetrySleepSeconds = 15;
        int nClientHeartbeatDelay = HEARTBEAT_DELAY_SECONDS;

        options_description cmdlineDesc ( "Command line-only parameters" );
        cmdlineDesc.add_options()
        ( "help,h", "show this help message" )
        ( "config,c", value ( &strConfigFile ), "configuration file name and path" )
        ;
        options_description configDesc ( "Parameters that can appear both on the command line and in the config file" );
        configDesc.add_options()
        ( "hostname", value ( &strHostname ), "server hostname/IP address" )
        ( "port", value ( &nPort ), "server port" )
        ( "username", value ( &strUsername ), "authentication username" )
        ( "password", value ( &strPassword ), "authentication password" )
        ( "unsubscribe", value ( &bMarketDataUnsubscribe ), "unsubscribe market data upon login" )
        ( "sessionlog", value ( &strSessionLogFile ), "session log file name and path" )
        ( "symbols", value ( &strSymbolsFile ), "market symbols file name and path" )
        ( "retrydelay", value ( &nRetrySleepSeconds ), "delay (in seconds) between re-connection attempts" )
        ( "heartbeatdelay", value ( &nClientHeartbeatDelay ), "time (in seconds) between client heartbeat packets" )
        ;

        variables_map vm;

        // Parse options that may appear on the command line only; besides,
        // we need to know the configuration file name first.
        //store ( parse_command_line ( argc, argv, cmdlineDesc ), vm );
        store ( command_line_parser ( argc, argv ).options ( cmdlineDesc ).allow_unregistered().run(), vm );
        notify ( vm );

        if ( vm.count ( "help" ) )
        {
            cout << cmdlineDesc << endl
                 << configDesc << endl;
            return 0;
        }

        // Load and parse the configuration file, if it was specified. Bail
        // out if it wasn't.
        if ( strConfigFile.empty() )
        {
            cerr << "error: configuration file name must be specified" << endl;
            return 1;
        }
        else
            cout << "notice: using configuration file: " << strConfigFile << endl;

        ifstream ifs ( strConfigFile.c_str() );
        if ( !ifs )
        {
            cerr << "error: can not open configuration file: " << strConfigFile << endl;
            return 1;
        }

        // Command-line options override values loaded from the configuration file
        store ( command_line_parser ( argc, argv ).options ( configDesc ).allow_unregistered().run(), vm );
        notify ( vm );

        store ( parse_config_file ( ifs, configDesc ), vm );
        notify ( vm );

        // Sanity check: we want all REQUIRED parameters to be specified.
        if ( !vm.count ( "hostname" ) )
        {
            cerr << "error: server hostname/IP address must be specified" << endl;
            return 1;
        }
        else
            cout << "notice: using server hostname/IP address: " << strHostname << endl;

        if ( !vm.count ( "port" ) )
        {
            cerr << "error: server port number must be specified" << endl;
            return 1;
        }
        else
            cout << "notice: using server port: " << nPort << endl;

        if ( !vm.count ( "username" ) )
        {
            cerr << "error: authentication username must be specified" << endl;
            return 1;
        }
        else
            cout << "notice: using authentication username: " << strUsername << endl;

        if ( !vm.count ( "password" ) )
        {
            cerr << "error: authentication password must be specified" << endl;
            return 1;
        }
        else
            cout << "notice: using authentication password: ********" << endl;

        if ( !vm.count ( "symbols" ) )
            cout << "notice: market symbols file was not specified, no subscription will occur" << endl;
        else
            cout << "notice: using market symbols file: " << strSymbolsFile << endl;

        if ( bMarketDataUnsubscribe )
            cout << "notice: the client will not subscribe to any market data for any currency pairs upon logging in" << endl;

        cout << "notice: using retry delay (seconds): " << nRetrySleepSeconds << endl;
        cout << "notice: using heartbeat delay (seconds): " << nClientHeartbeatDelay << endl;

        if (!strSessionLogFile.empty())
            cout << "notice: using session log file: " << strSessionLogFile << endl;

        CustomSession session ( strHostname.c_str(), nPort, strSessionLogFile.c_str() );
        session.SetHeartbeatDelay ( nClientHeartbeatDelay );

        struct rusage startTime;

        while ( true )
        {
            cout << "connecting to " << strHostname << " ..." << endl;
            session.Start();
            if ( session.IsConnected() )
            {
                getrusage ( RUSAGE_SELF, &startTime );
                cout << "connection established. logging in using " << strUsername << " credential..." << endl;
                if ( !session.Login ( strUsername.c_str(), strPassword.c_str(), bMarketDataUnsubscribe ) )
                {
                    cout << "login has completed with failure status" << endl;
                }
                else
                {
                    if ( !strSymbolsFile.empty() )
                    {
                        ifstream ifsSymbols ( strSymbolsFile.c_str() );
                        if ( ifsSymbols )
                        {
                            while ( !ifsSymbols.eof() && !ifsSymbols.fail() )
                            {
                                char szCurrencyPair[16] = "";
                                ifsSymbols.getline ( szCurrencyPair, sizeof ( szCurrencyPair ) );

                                if ( *szCurrencyPair == '\0' )
                                    continue;
                                cout << "snapshot subscribe for currency pair " << szCurrencyPair << endl;
                                if ( !session.GetMarketSnapshot ( szCurrencyPair ) )
                                {
                                    cout << "subscribe ticker has completed with failure status" << endl;
                                    break;
                                }
                                cout << "market subscribe for currency pair " << szCurrencyPair << endl;
                                if ( !session.SubscribeMarketData ( szCurrencyPair ) )
                                {
                                    cout << "subscribe ticker has completed with failure status" << endl;
                                    break;
                                }

                                cout << "ticker subscribe for currency pair " << szCurrencyPair << endl;
                                if ( !session.SubscribeTicker ( szCurrencyPair ) )
                                {
                                    cout << "subscribe ticker has completed with failure status" << endl;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            cout << "failure opening the symbols file: " << strSymbolsFile << endl;
                        }
                    }

                    if ( session.IsConnected() )
                        cout << "waiting for the connection to close..." << endl;
                    
                    while (!session.WaitClose(600000)) //900000
                    {
                        struct rusage currentTime;
                        getrusage ( RUSAGE_SELF, &currentTime );

                        long nUserSeconds = currentTime.ru_utime.tv_sec - startTime.ru_utime.tv_sec;
                        long nSystemSeconds = currentTime.ru_stime.tv_sec - startTime.ru_stime.tv_sec;

                        long nTotalSeconds = nUserSeconds + nSystemSeconds;
                        if ( nTotalSeconds == 0 )
                            nTotalSeconds = 1;

                        time_t nUptimeSeconds = session.GetUptimeSeconds();
                        long nMessagesSent = session.GetMessagesSentCount();
                        long nMessagesReceived = session.GetMessagesReceivedCount();

                        cout << nUptimeSeconds << "s uptime (user: "
                            << nUserSeconds << "s, system: "
                            << nSystemSeconds << "s); "
                            << nMessagesSent << " msgs sent (Abs: "
                            << nMessagesSent/nUptimeSeconds << "msg/s, Real: "
                            << nMessagesSent/nTotalSeconds << "msg/s); "
                            << nMessagesReceived << " msgs recv (Abs: "
                            << nMessagesReceived/nUptimeSeconds << "msg/s, Real: "
                            << nMessagesReceived/nTotalSeconds << "msg/s)."
                            << endl;
                    }

                    if ( session.IsConnected() )
                        cout << ">>> INTERNAL ERROR <<<" << endl;
                }
            }
            else
            {
                cout << "connection was not established" << endl;
            }
            session.Stop();

            cout << "pausing for " << nRetrySleepSeconds << " second(s)..." << endl;
            sleep ( nRetrySleepSeconds );
        }
    }
    catch ( exception& e )
    {
        cerr << e.what() << endl;
        return 2;
    }
}
