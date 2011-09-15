#include "MsgPump.h"


using namespace std;

MsgPump::MsgPump(std::string inFile):_inFile(inFile), _isStarted(false), _stopRequested(false), _bEOF(false), _dbgLine(0)
{
}

MsgPump::MsgPump()
{
    _inFile = "";
    _isStarted = false;
    _stopRequested = false;
    _bEOF = false;
    _dbgLine = 0;
}

MsgPump::~MsgPump()
{
    if (_in.is_open()) {
        _in.close();
    }
}

void 
MsgPump::setDataDictionary(const FIX::DataDictionary& dict) {
    _dict = dict;
}

void
MsgPump::join()
{
    _thread->join();
}

void 
MsgPump::run()
{
    while(!_stopRequested) {
        if (read() == false) {
            _bEOF = true;
            break;
        };
    }
}

bool
MsgPump::getMessage(FIX::Message& m, std::string& raw)
{
    boost::mutex::scoped_lock lock(_consumedMutex);
    raw.clear();
    m.clear();
    while (_strMsg.empty() && !_bEOF) {
        _msgRead.wait(lock); 
    }
    m = _msg;
    raw = _strMsg;
    _strMsg.clear();
    _msg.clear();
    _msgConsumed.notify_all();
    return !_bEOF;
}

bool 
MsgPump::read()
{
    boost::mutex::scoped_lock lock(_consumedMutex);
    std::string rawLine;
    assert(_in.is_open());
    bool parseOK = false;

    if (getline(_in, rawLine) && _in.eof() == false) {
            // FIX message parse    
            _msg.clear();
            _parser.addToStream(rawLine);
            parseOK = _parser.readFixMessage(_strMsg);
            _msg.setString(_strMsg, false, &_dict);
            // Now, _strMsg contains the cleaned up string of the message read from the log as is
            // and _msg contains the properly parsed message using the dictionary passed to the msg pump
            // NOTE NB - Using the dictionary is the only way to get correct ordering when reading 
            // the message from a string using setString(...)!!!!!!!!!!!!!!!!!!          

#ifdef DEBUG
            if (!parseOK) {
                std::cerr << "Parse failed" << std::endl;
            }
            else {
                std::cerr << "Parse OK" << std::endl;
            }
            if (_msg.getHeader().isSetField(FIX::FIELD::MsgType)) {
                FIX::MsgType msgType;
                _msg.getHeader().getField(msgType);
                std::cerr << "msgType: " << msgType << std::endl;
            }
            else {
                std::cerr << "msgType is not set" << std::endl;
            }
            if (_msg.getHeader().isSetField(FIX::FIELD::SendingTime)) {
                std::cerr << "Sending time IS set" << std::endl;
            }
            else {
                std::cerr << "Sending time NOT set" << std::endl;
            }
#endif // DEBUG
        _msgRead.notify_all();
        _msgConsumed.wait(lock);
        return true;
/*
        for (FIX::Message::iterator it = _msg.begin(); it != _msg.end(); it++) {
            std::cout << it->first << ":" << it->second.getString() << std::endl;
        }
*/
/* KTK TODO - for parsing messages by ^A 
        std::vector<std::string> vec;
        boost::escaped_list_separator<char> sep('\\', ';', '\"');
        boost::tokenizer< boost::escaped_list_separator<char> > tok(line, sep);
        vec.assign(tok.begin(), tok.end());
        std::vector<std::string>::iterator it;
        for (it = vec.begin(); it != vec.end(); it++) {
            std::cout << "VEC[" << *it << "] " 
        }
*/
    }
    else {
    //{
        _msgRead.notify_all();
        return false;
    }
}


void
MsgPump::start()
{
    if (_isStarted != false || _stopRequested == true) {
        return;
    }
    _thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&MsgPump::run, this)));
    //pthread_create(NULL, NULL, this->run(this), this);
    _isStarted = true;
}

void 
MsgPump::stop()
{
    _isStarted = false;
    _stopRequested = true;
    _thread->join();
    //pthread_join(&_mutex_file_lock); 
}

bool 
MsgPump::setInputFile(std::string inFile) 
{
    if (_in.is_open()) {
        return false;
    }
    _inFile = inFile;
    return true;
}


bool 
MsgPump::open()
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

