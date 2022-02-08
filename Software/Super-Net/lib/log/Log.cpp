#include "log.h"

Log * Log::_instance;

Log * Log::Instance(){
    if(_instance == nullptr) _instance = new Log();
    return _instance;
}
