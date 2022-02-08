#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H
#include <Arduino.h>
#include <map>
#include <buildconfig.h>

#ifdef DESKTOP
/// <summary>
/// possible packet types for the serial communication with the python application
/// </summary>
enum Serialtype{
    SENDMSG,
    RCVMSG,
    SENDSUCCESS,
    SENDERROR,
    SETADDR
};
#endif

/// <summary>
/// allows the communication between node and another device(running the app or the python desktop application) via USB serialinterface
/// </summary>
class SerialHandler{
public:
    /// <summary>
    /// interprets an via USBSerial incoming Packet
    /// </summary>
    static void InterpretPacket();

    /// <summary>
    /// initalizes the serialhandler
    /// </summary>
    static void init();

private:
    /// <summary>
    /// reads one byte with timout from the serial interface
    /// </summary>
    /// <returns>the read byte, if no byte retrieved: -1</returns>
    static int get_byte();

    /// <summary>
    /// callback method handling an incoming super-net message
    /// </summary>
    /// <param name="str">pointer to the byte array representing the incoming message string</param>
    /// <param name="size">size of the bytearray</param>
    /// <param name="rssi">signalstrength of the incoming packet</param>
    /// <param name="sender">sender of the Message</param>
    static void msg_Callback(char * str, int size, int rssi, int sender);

    /// <summary>
    /// callback method handling a send error
    /// </summary>
    /// <param name="packref">reference of the packet that couldn't be sent</param>
    /// <param name="err">error Message</param>
    static void error_Callback(int packref, String err);

    /// <summary>
    /// Callback method handling a successfully send message
    /// </summary>
    /// <param name="packref">reference to the packet that was sent successfully</param>
    static void success_Callback(int packref);


    #ifdef DESKTOP
    
    /// <summary>
    /// map allowing conversion from serial messagetype to corresponding bytevalue
    /// </summary>
    static std::map<Serialtype, char> _type2byte;

    /// <summary>
    /// map allowing conversion from bytevalue to corresponding serial messagetype
    /// </summary>
    static std::map<char, Serialtype> _byte2type;

    /// <summary>
    /// magic pattern anouncing the begin of a transmission between python application and node<br>
    /// so that eventual noise from serial interface can be ignored (when the interface is initialized, it sends information about it and other cases)
    /// </summary>
    const static char* _magicpattern;

    /// <summary>
    /// length of the magic pattern
    /// </summary>
    static int _patternlength;

    /// <summary>
    /// reads an integer from the serial interface
    /// </summary>
    /// <returns></returns>
    static int get_int();

    /// <summary>
    /// gets a specified amount of bytes from the serial interface
    /// </summary>
    /// <param name="count"></param>
    /// <returns></returns>
    static char* get_bytes(int count);

    /// <summary>
    /// checks if the magic pattern was received
    /// </summary>
    /// <returns>true if yes, false if not</returns>
    static bool check_4_magicpattern();

    /// <summary>
    /// sends byte array via the serial Interface
    /// </summary>
    /// <param name="bytes">the bytearray to send</param>
    /// <param name="count">size in bytes of the bytearray</param>
    static void send_bytes(char* bytes, int count);

    /// <summary>
    /// sends the magic pattern via the serial interface
    /// </summary>
    static void send_magicpattern();

    /// <summary>
    /// sends an integer via the serial interface
    /// </summary>
    /// <param name="i">the int to be sent</param>
    static void send_int(int i);
    
    #endif
    
};




#endif