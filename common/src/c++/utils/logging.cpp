#include "logging.h"
#ifdef PAN_BE_ZMQ
#include "pantheios_be_zmq/pantheios_be_zmq.h"
#endif // PAN_BE_ZMQ

//extern const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[];

pan_fe_N_t PAN_FE_N_SEVERITY_CEILINGS[] = 
{
    {1, PANTHEIOS_SEV_DEBUG}, 
    {2, PANTHEIOS_SEV_DEBUG}, 
    {3, PANTHEIOS_SEV_ERROR}, 
    {0, PANTHEIOS_SEV_DEBUG}
};

pan_be_N_t PAN_BE_N_BACKEND_LIST[] = 
{
	PANTHEIOS_BE_N_STDFORM_ENTRY(1, pantheios_be_fprintf, 0),
	PANTHEIOS_BE_N_STDFORM_ENTRY(2, pantheios_be_file, PANTHEIOS_BE_N_F_IGNORE_NONMATCHED_CUSTOM28_ID),
	PANTHEIOS_BE_N_STDFORM_ENTRY(3, pantheios_be_zmq, PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE),
	PANTHEIOS_BE_N_TERMINATOR_ENTRY
};

int 
logging_init(const char* log_file_name) 
{

    try {
		pantheios_be_file_setFilePath(log_file_name);
#ifdef DEBUG
		pantheios::log_DEBUG("pantheios logging_init()");
#endif
		return 1;
    }
    catch(std::bad_alloc&){
        pantheios::log_ALERT("pantheios out of memory");
    }
    catch(std::exception& x){
        pantheios::log_CRITICAL("pantheios exception: ", x);
    }
    catch(...){
        pantheios::puts(pantheios::emergency, "pantheios unknown error");
    }

    return 2;
};

std::string
createTimestampedLogFilename(const char* prefix)
{
    // initialize logging
    std::string logFileName;
    logFileName += prefix;
    logFileName += ".";
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer)-1, "%Y%m%d", timeinfo);
    //puts(buffer);
    logFileName += buffer;
    logFileName += ".log";

    return logFileName;
}

