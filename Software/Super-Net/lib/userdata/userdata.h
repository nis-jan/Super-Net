#ifndef USERDATA_H
#define USERDATA_H

/// <summary>
/// singleton implementation<br>
/// instance keeps important data that should be accessible by all other classes<br>
/// </summary>
class userdata{
public:
    /// <summary>
    /// return singleton instance
    /// </summary>
    /// <returns>the instance of this class</returns>
    static userdata * Instance();
    /// <summary>
    /// returns the Adress of the node
    /// </summary>
    /// <returns></returns>
    int MYADRESS(){return _MYADDRESS;}
    /// <summary>
    /// sets the adress of the node
    /// </summary>
    /// <param name="addr"></param>
    void set_ADDR(int addr){_MYADDRESS = addr;}

private:
    /// <summary>
    /// this nodes adress, initially 1
    /// </summary>
    int _MYADDRESS = 1;
    static userdata * _instance;
    userdata(){}
    
};

#endif
//zu bereinigende Dateien: definitiv platformio.ini