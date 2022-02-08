#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H
#include <Arduino.h>
#include <map>
#include <buildconfig.h>

enum Serialtype{
    SENDMSG,
    RCVMSG,
    SENDSUCCESS,
    SENDERROR,
    SETADDR
};


class SerialHandler{
public:
    static void InterpretPacket();
    static void init();

private:
    static int get_byte();
    static void msg_Callback(char * str, int size, int rssi, int sender);
    static void error_Callback(int packref, String err);
    static void success_Callback(int packref);


    #ifdef DESKTOP
    
    static std::map<Serialtype, char> _type2byte;
    static std::map<char, Serialtype> _byte2type;
    const static char* magicpattern;
    static int patternlength;

    static int get_int();
    static char* get_bytes(int count);
    static bool check_4_magicpattern();

    static void send_bytes(char* bytes, int count);
    static void send_magicpattern();
    static void send_int(int i);
    
    #endif
    
};




#endif