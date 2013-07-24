#include "msg_pump.h"


using namespace std;

//#define STATE fprintf(stderr, "LINE: %d, _msgCode: %d, _read: %d, _consumed: %d, _strMsg: %s\n", __LINE__, _msgCode, _read, _consumed, _strMsg.c_str()); 
#define STATE

MP::MP():_inFile(""), _isStarted(false), _stopRequested(false), _msgCode(MSG_NOT_READY), _read(false), _consumed(true)
{
    STATE
}

MP::MP(std::string inFile):_inFile(inFile), _isStarted(false), _stopRequested(false), _msgCode(MSG_NOT_READY), _read(false), _consumed(true)
{
    STATE
}

MP::~MP()
{
    if (_in.is_open()) {
        _in.close();
    }
}

void
MP::join()
{
    _thread->join();
}

void 
MP::run()
{
    //boost::mutex::scoped_lock lock(_mutex);
    STATE
    while(!_stopRequested && _msgCode != MSG_EOF) {
        read();
    }
}


void
MP::getMsg(std::string& raw, MSG_RET* ret)
{
    STATE
    assert(ret);
    boost::mutex::scoped_lock lock(_mutex);
    while (_read == false) { _msgRead.wait(lock);}
    raw = _strMsg; 
    *ret =  _msgCode;    
    _consumed = true;
    _read = false;
    _msgConsumed.notify_one();
}

void
MP::read()
{
    boost::mutex::scoped_lock lock(_mutex);
    STATE
    while (_consumed == false) { _msgConsumed.wait(lock);}
    std::string rawLine;
    assert(_in.is_open());

    if (getline(_in, rawLine)) {
            _strMsg = rawLine;
#ifdef DEBUG
            fprintf(fp, "%s\n", _strMsg.c_str());
            fflush(fp);
#endif 
            _msgCode = MSG_OK;
    }
    else {
        if (_in.eof() == true) {
            _msgCode = MSG_EOF;
            //_stop.notify_one();
        }
    }
    _read = true;
    _consumed = false;
    _msgRead.notify_one();
}


void
MP::start()
{
    if (_isStarted != false || _stopRequested == true) {
        return;
    }
#ifdef DEBUG
    fp = fopen("MPLOG", "wb");
    assert(fp);
#endif
    _thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&MP::run, this)));
}

void 
MP::stop()
{
    _isStarted = false;
    _stopRequested = true;
    join();
}

bool 
MP::setInputFile(std::string inFile) 
{
    if (_in.is_open()) {
        return false;
    }
    _inFile = inFile;
    return true;
}


bool 
MP::open()
{
    if (_in.is_open()) {
        std::cerr << "Already open";
        return false;
    }
    _in.open(_inFile.c_str(), ifstream::in);
    if (!_in.is_open()) {
        return false;
    }
    return true;
}

