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



/// <summary>
/// Setup routine, runs once and initializes the node
/// </summary>
void setup() {
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

/// <summary>
/// loop runs continuously and looks for incoming data via Usb serial or LoRa
/// </summary>
void loop() {
  if(Serial.available()){
    SerialHandler::InterpretPacket();
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
