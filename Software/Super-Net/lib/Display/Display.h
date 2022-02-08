#ifndef DISPLAY_H
#define DISPLAY_H

#include <SSD1306Wire.h>
#include <userdata.h>

/// <summary>
/// represents the oled built on the heltec esp32 WiFi LoRa v2
/// </summary>
class display{
public:
    /// <summary>
    /// singleton implementation
    /// </summary>
    /// <returns>returns the instance of this display</returns>
    static display* instance(){
        if(_inst == nullptr) _inst = new display();
        return _inst;
    }

    /// <summary>
    /// dipslays a fancy title screen containing the current adress of the node
    /// </summary>
    void update(){
        _disp->clear();
        _disp->setFont(ArialMT_Plain_10);
        _disp->drawString(0,0, "SUPERNET\nV1.1 by ###NISE###");
        _disp->setFont(ArialMT_Plain_24); 
        _disp->drawString(30,30,String(userdata::Instance()->MYADRESS()));
        _disp->display();
    }

    /// <summary>
    /// prints a plain message to the screen,<br>
    /// can be used for debugging
    /// </summary>
    /// <param name="msg">the message to be printed</param>
    void print_msg(String msg){
        _disp->clear();
        _disp->setFont(ArialMT_Plain_10);
        _disp->drawString(0,0, msg);
        _disp->display();
    }

private:
    /// <summary>
    /// singleton -> private constructor, <br>
    /// initializes the oled
    /// </summary>
    display(){
        _disp = new SSD1306Wire(0x3C, SDA_OLED, SCL_OLED, RST_OLED, GEOMETRY_128_64);
        _disp->init();
        _disp->setFont(ArialMT_Plain_10); 
        _disp->flipScreenVertically();
        _disp->clear();
        _disp->drawString(0, 0, "booting...");
        _disp->display();
        delay(500);
    }
    ~display(){
        delete _disp;
    }
    static display * _inst;
    SSD1306Wire * _disp;
};

#endif