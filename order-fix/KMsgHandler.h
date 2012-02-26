#ifndef _KMSG_HANDLER_H_
#define _KMSG_HANDLER_H_

#include <iostream>
#include <string>
#include <exception>
#include <sstream>

#include <stdio.h>


#include "logging.h"

// Interface for passing to MsgProcessor so that messages can be sent to 
// exchange after being received on zmq queue

class KMsgHandler
{   
    public:
        KMsgHandler() {};
		//virtual KMsgHandler(const KMsgHandler& rhs) = 0;
        virtual ~KMsgHandler() {};

        virtual void sendMsg() = 0;
        virtual void getMsgType() = 0;

		//virtual KMsgHandler& operator=(const KMsgHandler& rhs) = 0;
 
    private:
        
};

#endif // _KMSG_HANDLER_H_
