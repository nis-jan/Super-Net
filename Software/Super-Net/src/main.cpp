#include <Arduino.h>
#include <LoRa.h>
#include <sendrcv.h>
#include <SerialHandler.h>
#include <Log.h>

//lora hardwarepins:
#define LoRa_RST 14
#define LoRa_CS 18
//entspricht IRQ
#define LoRa_DIO0 26

bool test_routing = false;
Packet * p;

void Button_ISR(){
  test_routing = true;
  Log::Instance()->Log_msg("pressed");
  digitalWrite(25, !digitalRead(25));
}


/// <summary>
/// Setup routine, runs once and initializes the node
/// </summary>
void setup() {
  pinMode(0, INPUT);
  attachInterrupt(digitalPinToInterrupt(0), Button_ISR, RISING);
  pinMode(25, OUTPUT);
  
  Serial.begin(9600);
  LoRa.setPins(LoRa_CS, LoRa_RST, LoRa_DIO0);
  if(!LoRa.begin(868e6)){
    Log::Instance()->Log_msg("LoRainit failed!");
    digitalWrite(25, HIGH);
    while(true);
  }
  SerialHandler::init();
  Log::Instance()->Log_msg("LoRa initialized");
  
}



int packetsize;
int ref;
String msgsend=  "";
char * msgptr;
std::vector<int> route = {2,1,2000};
/// <summary>
/// loop runs continuously and looks for incoming data via Usb serial or LoRa
/// </summary>
void loop() {
  if(Serial.available()){
    SerialHandler::InterpretPacket();
  }
  if(test_routing){
    Log::Instance()->Log_msg("sending msg");
    p = new Packet();
    ref = Packet::create_reference();
    msgsend = "hiii ref:" + String(ref);
    msgptr = (char*)msgsend.c_str();
    Packet::create_msg(p, route[route.size()-1], ref, route, msgsend.length(), msgptr);
    sendrcv::sendpacket(p, true);
    test_routing = false;
  }

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