#include <Arduino.h>
#include <LoRa.h>
#include <sendrcv.h>
#include <SerialHandler.h>
#include <Log.h>


#define LoRa_RST 14
#define LoRa_CS 18
//entspricht IRQ
#define LoRa_DIO0 26

bool sendmsg = false;


void Button_ISR(){
  sendmsg = true;
  Log::Instance()->Log_msg("pressed");
  digitalWrite(25, !digitalRead(25));
}

/*
void msg_callback(char * str, int size, int rssi, int sender){
  Log::Instance()->Log_msg("Callback opened");
  String msg = "";
  for(int i = 0 ; i < size; i++){
    msg += str[i];
  }
  Log::Instance()->Log_msg("Got MSG:\n\t" + msg + "\nRSSI:\n   " + String(rssi));
  Log::Instance()->Log_msg(msg);
}

void error_Callback(int packref, String err){
  Log::Instance()->Log_msg("error callback opened");
  Log::Instance()->Log_msg("Fehler bei Paket mit Referens" + String(packref) + ":\n" + err);
}

void success_callback(int packref){
  Log::Instance()->Log_msg("Paket " + String(packref) + "\nerfolgreich gesendet!");
}
*/
unsigned long last = 0;
void setup() {
  pinMode(0, INPUT);
  pinMode(25, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(0), Button_ISR, FALLING);
  Serial.begin(9600);
  LoRa.setPins(LoRa_CS, LoRa_RST, LoRa_DIO0);
  if(!LoRa.begin(868e6)){
    Log::Instance()->Log_msg("LoRainit failed!");
    digitalWrite(25, HIGH);
    while(true);
  }
  SerialHandler::init();
  last = millis();
  //Log::Instance()->Log_msg("LoRa initialized");
  /*
  sendrcv::Msg_Callback = msg_callback;
  sendrcv::Send_Error = error_Callback;
  sendrcv::On_Send_Success = success_callback;
  */
}

/*
Packet * p;
#if MYADRESS == 1
std::vector<int> route = {1,2,3};
#else
std::vector<int> route = {3,2,1};
#endif
String msgsend = "hallo";
char* msgptr;
int ref;
*/

int packetsize;
void loop() {
  if(Serial.available()){
    SerialHandler::InterpretPacket();
  }


  //alter code:
  /*
  if(sendmsg){
    Log::Instance()->Log_msg("sending msg");
    p = new Packet();
    ref = Packet::create_reference();
    msgsend = "hiii ref:" + String(ref);
    msgptr = (char*)msgsend.c_str();
    Packet::create_msg(p, route[route.size()-1], ref, route, msgsend.length(), msgptr);
    sendrcv::sendpacket(p, true);
    sendmsg = false;
  }*/
  packetsize = LoRa.parsePacket();
  try{
    if(packetsize)
      sendrcv::rcv_packet(packetsize);
    
  }
  catch(const char * ex){
    Log::Instance()->Log_msg("Fehler :/ Nachricht: " + String(ex));
  }
  delay(10);
}