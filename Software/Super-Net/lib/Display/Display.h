#ifndef DISPLAY_H
#define DISPLAY_H

#include <SSD1306Wire.h>
#include <userdata.h>
class display{
public:
    static display* instance(){
        if(_inst == nullptr) _inst = new display();
        return _inst;
    }

    
    void update(){
        _disp->clear();
        _disp->setFont(ArialMT_Plain_10);
        _disp->drawString(0,0, "SUPERNET\nV1.1 by ###NISE###");
        _disp->setFont(ArialMT_Plain_24); 
        _disp->drawString(30,30,String(userdata::Instance()->MYADRESS()));
        _disp->display();
    }
    void print_msg(String msg){
        _disp->clear();
        _disp->setFont(ArialMT_Plain_10);
        _disp->drawString(0,0, msg);
        _disp->display();
    }

private:
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