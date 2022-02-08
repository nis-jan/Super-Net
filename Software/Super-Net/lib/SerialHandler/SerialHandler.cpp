#include "SerialHandler.h"
#include <sendrcv.h>
#include <Display.h>



#ifdef DESKTOP
std::map<Serialtype, char> SerialHandler::_type2byte = {{SENDMSG, 1}, {RCVMSG, 2}, {SENDSUCCESS, 3}, {SENDERROR, 4}, {SETADDR, 5}};
std::map<char, Serialtype> SerialHandler::_byte2type = {{1, SENDMSG}, {2, RCVMSG}, {3, SENDSUCCESS}, {4, SENDERROR}, {5, SETADDR}};
const char* SerialHandler::_magicpattern = "SuperNetSerial";
int SerialHandler::_patternlength = 14;
#endif


void SerialHandler::init(){
    sendrcv::On_MSG_Receive = msg_Callback;
    sendrcv::On_Send_Success = success_Callback;
    sendrcv::On_Send_Error = error_Callback;
    display::instance()->update();
}


void SerialHandler::msg_Callback(char * str, int size, int rssi, int sender){
    #ifdef MOBILE
    String msg = "";
    for(int i = 0; i < size; i++){
        msg += str[i];
    }
    Serial.println("RCVMS\n" + String(sender) + "\n" + String(size) + "\n" + msg + "\n");
    #else
    send_magicpattern();
    Serial.write(_type2byte[RCVMSG]);
    send_int(sender);
    send_int(size);
    send_bytes(str, size);
    #endif
}

void SerialHandler::success_Callback(int packref){
    #ifdef MOBILE
    Serial.println("SENDS\n" + String(packref) + "\n");
    #else
    send_magicpattern();
    Serial.write(_type2byte[SENDSUCCESS]);
    send_int(packref);
    #endif
    
}

void SerialHandler::error_Callback(int packref, String err){
    #ifdef MOBILE
    Serial.println("SENDE\n" + String(packref) + "\n");
    #else
    send_magicpattern();
    Serial.write(_type2byte[SENDERROR]);
    send_int(packref);
    #endif
    
}



void SerialHandler::InterpretPacket(){
    // interpret string-based packets from mobile app:
    #ifdef MOBILE
    String incoming = "";
    for(int i = 0; i < 5; i++){
        incoming += (char)get_byte();
    }
    //display::instance()->print_msg(incoming);
    if(incoming == "SETAD"){
        get_byte();
        incoming = Serial.readStringUntil('\n');
        userdata::Instance()->set_ADDR(incoming.toInt());
        display::instance()->update();
    }
    else if(incoming == "SENDM"){
        int recipient = Serial.readStringUntil('\n').toInt();
        String tmpref = Serial.readStringUntil('\n');
        incoming = Serial.readString();
        int packref = Packet::create_reference();
        sendrcv::send_msg((char*)incoming.c_str(), incoming.length(), recipient, packref);
        
        Serial.println("PSENT\n" + tmpref + "\n" + String(packref) + "\n");
        

    }

    // interpret byte-based Packets from Desktop python application
    #else
    if(!check_4_magicpattern())return;
    char typebyte = get_byte();
    Serialtype type = _byte2type[typebyte];
    //display::instance()->print_msg("type: " + String((int)typebyte));
    if(type == SETADDR){
        userdata::Instance()->set_ADDR(get_int());
        display::instance()->update();
    }
    else if(type == SENDMSG){
        int recipient = get_int();
        int msg_length = get_int();
        char* msg = get_bytes(msg_length);
        
        int packref = Packet::create_reference();
        send_int(packref);
        sendrcv::send_msg(msg, msg_length, recipient, packref);
        display::instance()->print_msg("sent msg with ref " + String(packref) + "\nto " + String(recipient));
        
        
    }
    #endif
    
}

int SerialHandler::get_byte(){
    char ret;
    int timeout = 500;
    while(timeout>0){
        if(Serial.available()){
            ret = Serial.read();
            if(ret != -1) return ret;
        }
        timeout--;
        delay(1);
    }
    return -1;
    
}

#ifdef DESKTOP

bool SerialHandler::check_4_magicpattern(){
    int current;
    for(int i = 0; i < _patternlength;i++){
        current = get_byte();
        //Serial.println(String((char)current) + " = " + _magicpattern[i]);
        if(current != _magicpattern[i]) return false;
    }
    return true;
}

int SerialHandler::get_int(){
    char * intbytes = get_bytes(4);
    int ret = bytes::bytes2int(intbytes);
    free(intbytes);
    return ret;
}

char * SerialHandler::get_bytes(int count){
    char* ret= (char*)malloc(count);
    for(int i = 0; i < count; i++){
        ret[i] = get_byte();
    }
    return ret;
}

void SerialHandler::send_int(int i){
    char * intptr = bytes::int2bytes(i);
    send_bytes(intptr, sizeof(int));
    free(intptr);
}

void SerialHandler::send_magicpattern(){
    send_bytes((char*)_magicpattern, _patternlength);
}

void SerialHandler::send_bytes(char* bytes, int count){
    for(int i = 0; i < count;i++){
        Serial.write(bytes[i]);
    }
}

#endif