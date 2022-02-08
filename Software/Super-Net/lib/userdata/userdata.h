#ifndef USERDATA_H
#define USERDATA_H

class userdata{
public:
    static userdata * Instance();
    int MYADRESS(){return _MYADDRESS;}
    void set_ADDR(int addr){_MYADDRESS = addr;}

private:
    int _MYADDRESS = 1;
    static userdata * _instance;
    userdata(){}
    
};

#endif
//zu bereinigende Dateien: definitiv platformio.ini