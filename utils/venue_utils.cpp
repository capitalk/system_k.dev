#include "venue_utils.h"

#include <string.h>
#include <stdio.h>

namespace capk 
{

    capkproto::venue_configuration
    get_venue_config(capkproto::configuration* config, 
            const char* mic_name) 
    {
        if (config != NULL) {
            for (int i = 0; i< config->configs_size(); i++) {
                const capkproto::venue_configuration vc  = config->configs(i);
                if (vc.mic_name() == mic_name) {
                    return vc;
                }
            }
        }
        return capkproto::venue_configuration();
    } 


    int 
    get_config_params(void* zmq_context, 
            const char* config_server_addr, 
            capkproto::configuration* cfg)
    {

#if 0
        void *socket = zmq_socket(zmq_context, ZMQ_REQ);

        int rc = zmq_connect(socket, config_server_addr);
        assert(rc == 0);

        zmq_msg_t outbound_msg;

        rc = zmq_msg_init_data(&outbound_msg, (void*)&capk::REQ_CONFIG, sizeof(char), NULL, NULL);
        assert(rc == 0); 

        rc = zmq_send(socket, &outbound_msg, 0);
        assert(rc == 0);
        zmq_msg_close(&outbound_msg);

        // inbound message
        zmq_msg_t in_reply_to_msg;
        zmq_msg_init(&in_reply_to_msg);
        zmq_msg_t inbound_msg;
        zmq_msg_init(&inbound_msg);

        int64_t more = 0;
        size_t more_size = sizeof(more);
        do {
            rc = zmq_recv(socket, &in_reply_to_msg, 0);
            assert(rc == 0);
            rc = zmq_recv(socket, &inbound_msg, 0);
            assert(rc == 0);
            //pan::log_DEBUG("Received msg size: ", pan::integer(zmq_msg_size(&inbound_msg)));
            //pan::log_DEBUG("Received msg: ", pan::blob(zmq_msg_data(&inbound_msg), zmq_msg_size(&inbound_msg)));
            zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &more_size);
        } while (more);

        capkproto::configuration config;

        bool parseOK = config.ParseFromArray(zmq_msg_data(&inbound_msg), zmq_msg_size(&inbound_msg));
        if (!parseOK) {
            assert(false);
        }
        else {
            fprintf(stderr, "There are %d %s", config.configs_size() , " venues available");
            for (int j = 0; j < config.configs_size(); j++) {
                const capkproto::venue_configuration& cfg = config.configs(j);
                fprintf(stderr, "venue_configuration: %s", cfg.DebugString().c_str());
            }
        }

        zmq_close(&inbound_msg);
#endif
#if 1
        int zero = 0;
        int rc; 
        assert(zmq_context != NULL);
        void* config_socket = zmq_socket(zmq_context, ZMQ_REQ);
        zmq_setsockopt(&config_socket, ZMQ_LINGER, &zero, sizeof(zero));
        assert(config_socket);

        rc = zmq_connect(config_socket, config_server_addr);
        assert(rc == 0);
        if (rc != 0) {
            zmq_close(config_socket);
            return -1;
        }

        zmq_msg_t request_frame;

        rc = zmq_msg_init_data(&request_frame, (void*)&capk::REQ_CONFIG, sizeof(capk::REQ_CONFIG), NULL, NULL);
        assert(rc == 0);
        if (rc != 0) {
            zmq_close(config_socket);
            return -1;
        }

        //memcpy(zmq_msg_data(&request_frame), (void*)&capk::REQ_CONFIG, sizeof(capk::REQ_CONFIG));
        rc = zmq_send(config_socket, &request_frame, 0);
        assert(rc == 0);
        zmq_msg_close(&request_frame);
        if (rc != 0) {
            zmq_close(config_socket);
            return -1;
        }

        // inbound messages
        zmq_msg_t in_reply_to_frame;
        zmq_msg_init(&in_reply_to_frame);
        zmq_msg_t config_frame;
        zmq_msg_init(&config_frame);

        int64_t more = 0;
        size_t more_size = sizeof(more);
        do {
            rc = zmq_recv(config_socket, &in_reply_to_frame, 0);
            assert(rc == 0);
            if (rc != 0) {
                zmq_close(config_socket);
                return -1;
            }

            rc = zmq_recv(config_socket, &config_frame, 0);
            assert(rc == 0);
            if (rc != 0) {
                zmq_close(config_socket);
                return -1;
            }
            zmq_getsockopt(config_socket, ZMQ_RCVMORE, &more, &more_size);
        } while (more);

        bool parseOK = cfg->ParseFromArray(zmq_msg_data(&config_frame), zmq_msg_size(&config_frame));
        if (!parseOK) {
            assert(false);
            if (rc != 0) {
                zmq_close(config_socket);
                return -1;
            }
            return -1;
        }
        else {
            if (rc != 0) {
                zmq_close(config_socket);
                return -1;
            }
            zmq_close(config_socket);
            return 0;
        }
        zmq_close(config_socket);
        return -1;
#endif
    }

} // namespae capk
