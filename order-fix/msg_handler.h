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

namespace capk {

class MsgHandler
{   
    public:
        MsgHandler() {};
		//virtual MsgHandler(const MsgHandler& rhs) = 0;
        virtual ~MsgHandler() {};

        virtual void sendMsg() = 0;
        virtual void getMsgType() = 0;

		//virtual MsgHandler& operator=(const MsgHandler& rhs) = 0;
 
    private:
        
};

} // namespace capk

#endif // _KMSG_HANDLER_H_
