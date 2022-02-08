#ifndef LOG_H
#define LOG_H
#include <Arduino.h>

//#define VERBOSE_LOGGING 1

/// <summary>
/// if define VERBOSE_LOGGING is activated the singleton-instance<br>
/// of this logger prints the log message via USB-Serial interface
/// </summary>
class Log{
public:
    /// <summary>
    /// singleton implementation, return the instance of the logger
    /// </summary>
    /// <returns>instance of the logger</returns>
    static Log * Instance();
    /// <summary>
    /// if verbose logging is activated, this logger
    /// prints the log message via Serial.println
    /// </summary>
    /// <param name="log_msg"></param>
    void Log_msg(String log_msg){
        #ifdef VERBOSE_LOGGING
        Serial.println(log_msg);
        #endif
    }

private:
    static Log * _instance;
    /// <summary>
    /// singleton implementation -> private constructor
    /// </summary>
    Log(){}
    
};


#endif