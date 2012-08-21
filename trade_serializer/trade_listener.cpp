#include <zmq.hpp>

#include <iostream>
#include <string>
#include <exception>

class TradeListener
{
    public: 
    TradeListener(zmq::context_t* context, const std::string& bindAddr) : 
        _pcontext(context), 
        _bindAddr(bindAddr)
    {
    }

    ~TradeListener()
    {
    }

    int  
    run()
    {
        int rc;
        std::cout << "Connecting to server...\n";
        zmq::socket_t subscriber(context, ZMQ_SUB);
        subscriber.connect(_bindAddr.c_str());
        const char* filter = "";
        subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
        GOOGLE_PROTOBUF_VERIFY_VERSION;
        capkproto::instrument_bbo bbo;

        while (1) {
            zmq::message_t msg;
            rc = subscriber.recv(&msg);
            assert(rc);
            bbo.ParseFromArray(msg.data(), msg.size());
        //    std::cout << bbo.DebugString() << "\n";
            std::cout << "Symbol   : " << bbo.symbol() << "\n";
            std::cout << "BB MIC   : " << bbo.bb_mic() << "\n";
            std::cout << "BB Price : " << (double)bbo.bb_price() << "\n";
            std::cout << "BB Size  : " << (double)bbo.bb_size() << "\n";

            std::cout << "BA MIC   : " << bbo.ba_mic() << "\n";
            std::cout << "BA Price : " << (double)bbo.ba_price() << "\n";
            std::cout << "BA Size  : " << (double)bbo.ba_size() << "\n";
        }
        google::protobuf::ShutdownProtobufLibrary();

    }
    void
    setFilter(const char* filter) 
    {
    }

    private:
    zmq::context_t* _pcontext;
    std::string _bindAddr;
}
