#include "logging.h"

const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = "order_engine-fix";

pan_be_N_t PAN_BE_N_BACKEND_LIST[] = 
{
	PANTHEIOS_BE_N_STDFORM_ENTRY(1, pantheios_be_fprintf, 0),
	PANTHEIOS_BE_N_STDFORM_ENTRY(2, pantheios_be_file, PANTHEIOS_BE_N_F_IGNORE_NONMATCHED_CUSTOM28_ID),
	PANTHEIOS_BE_N_TERMINATOR_ENTRY
};

int 
logging_init(const char* log_file_name) {


    try {
		pantheios_be_file_setFilePath(log_file_name);
		pantheios::log_DEBUG("logging_init()");
		return 1;
    }
    catch(std::bad_alloc&){
        pantheios::log_ALERT("out of memory");
    }
    catch(std::exception& x){
        pantheios::log_CRITICAL("Exception: ", x);
    }
    catch(...){
        pantheios::puts(pantheios::emergency, "Unknown error");
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
    strftime(buffer, 80, "%Y%m%d", timeinfo);
    puts(buffer);
    logFileName += buffer;
    logFileName += ".log";

    return logFileName;
}

