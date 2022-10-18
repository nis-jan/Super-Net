#include "userdata.h"

userdata * userdata::_instance;
userdata * userdata::Instance(){
    if(_instance == nullptr) _instance = new userdata();
    return _instance;
}