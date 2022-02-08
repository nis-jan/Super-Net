#ifndef LOG_H
#define LOG_H
#include <Arduino.h>

//#define VERBOSE_LOGGING 1

class Log{
public:
    static Log * Instance();
    void Log_msg(String log_msg){
        #ifdef VERBOSE_LOGGING
        Serial.println(log_msg);
        #endif
    }

private:
    static Log * _instance;
    Log(){}
    
};


#endif