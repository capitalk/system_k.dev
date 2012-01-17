#ifndef _CAPK_SESSIONINFO_
#define _CAPK_SESSIONINFO_

#include <string>
#include <iostream>
#include <set>

#include "quickfix/Session.h"

#include "MsgDispatcher.h"
#include "MP.h"

class MsgDispatcher;

class SessionInfo
{
    public: 
        SessionInfo(const FIX::SessionID& sid);// : _sessionID(sid), _msgDispatcher(0), _msgPump(0) { }


        ~SessionInfo(); /*{
            if (_msgPump) {
                delete _msgPump;
            }
            if (_msgDispatcher) {
                delete _msgDispatcher;
            }
        } 
        */

        bool addSymbol(const std::string& symbol);/* { 
            std::pair<std::map<std::string, FIX::MDReqID>::iterator, bool> ret;
            ret = _symbols.insert(std::pair<std::string, FIX::MDReqID>(symbol, FIX::MDReqID("")));
            //_symbols[symbol] = FIX::MDReqID();
            return ret.second;
        }
        */
        void removeSymbol(const std::string& symbol);/* {
            _symbols.erase(symbol);
        }
        */
    
        bool hasSymbol(const FIX::Symbol& symbol); 
        bool hasSymbol(const std::string& symbol); /*{
            std::map<std::string, FIX::MDReqID>::iterator it = _symbols.find(symbol);
            return (it != _symbols.end());
        }
        */

        void  getMDReqID(const std::string& symbol, FIX::MDReqID& MDReqID); /*{
            std::map<std::string, FIX::MDReqID>::iterator it = _symbols.find(symbol);
            MDReqID = it->second;
        }
        */

        size_t size(); /*{
            return (_symbols.size());
        }
        */

        bool hasDispatcher(); /* { 
            return _msgDispatcher != NULL;
        }
        */

        bool setDispatcher(MsgDispatcher* md); /*{
            if (_msgDispatcher == NULL && md != NULL) {
                _msgDispatcher = md;
                return true;
            }
            return false;
        }
        */
        MsgDispatcher* getDispatcher();
        

        void getSessionID(FIX::SessionID& sessionID);/*{ sessionID = _sessionID;} */
    
    private:
        std::map<std::string, FIX::MDReqID> _symbols;
        FIX::SessionID _sessionID;
        FIX::SubscriptionRequestType _subscriptionRequestType;
        FIX::MarketDepth _marketDepth;
        FIX::MDUpdateType _mdUpdateType;
        MsgDispatcher* _msgDispatcher;

};

#endif //_CAPK_SESSIONIFO_
