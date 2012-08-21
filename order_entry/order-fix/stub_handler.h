#ifndef _STUB_HANDLER
#define _STUB_HANDLER

#include <iostream>
#include <string>
#include <exception>
#include <sstream>

#include <stdio.h>

#include "msg_handler.h"


#include "logging.h"


class StubHandler: public capk::MsgHandler
{   
    public:
        StubHandler() { pan::log_DEBUG("StubHandler()");};
        virtual ~StubHandler() { pan::log_DEBUG("~StubHandler()");};

        virtual void sendMsg() { pan::log_DEBUG("sendMsg()");};
        virtual void getMsgType() { pan::log_DEBUG("getMsgType()");};
 
    private:
        
};

#endif // _STUB_HANDLER
