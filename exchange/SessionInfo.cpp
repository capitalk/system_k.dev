
#include "SessionInfo.h"

SessionInfo::SessionInfo(const FIX::SessionID& sid) : _sessionID(sid), _msgDispatcher(0), _msgPump(0) { }


SessionInfo::~SessionInfo() {
    if (_msgPump) {
        delete _msgPump;
    }
    if (_msgDispatcher) {
        delete _msgDispatcher;
    }
}

bool
SessionInfo::addSymbol(const std::string& symbol) { 
    std::pair<std::map<std::string, FIX::MDReqID>::iterator, bool> ret;
    ret = _symbols.insert(std::pair<std::string, FIX::MDReqID>(symbol, FIX::MDReqID("")));
    return ret.second;
}

void 
SessionInfo::removeSymbol(const std::string& symbol) {
    _symbols.erase(symbol);
}
    
bool 
SessionInfo::hasSymbol(const FIX::Symbol& s) {
    return hasSymbol(s.getValue());
}


bool 
SessionInfo::hasSymbol(const std::string& symbol) {
    std::map<std::string, FIX::MDReqID>::iterator it = _symbols.find(symbol);
    return (it != _symbols.end());
}

void  
SessionInfo::getMDReqID(const std::string& symbol, FIX::MDReqID& MDReqID) {
    std::map<std::string, FIX::MDReqID>::iterator it = _symbols.find(symbol);
    MDReqID = it->second;
}

size_t 
SessionInfo::size() {
    return (_symbols.size());
}

bool 
SessionInfo::hasDispatcher() { 
    return _msgDispatcher != NULL;
}

bool 
SessionInfo::setDispatcher(MsgDispatcher* md) {
    if (_msgDispatcher == NULL && md != NULL) {
        _msgDispatcher = md;
        return true;
    }
    return false;
}
        
bool 
SessionInfo::setPump(MsgPump* mp) {
    if (_msgPump == NULL && mp != NULL) {
        _msgPump = mp;
        return true;
    } 
    return false;
}

void 
SessionInfo::getSessionID(FIX::SessionID& sessionID) { sessionID = _sessionID;}
    


