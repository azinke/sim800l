/**
    @autor: AMOUSSOU Z. Kenneth
    @date: 17/09/2018
    @version: 1.0
    @external-library:
        SoftwareSerial
*/
#include "Sim800l.h"
#include <SoftwareSerial.h>

#ifdef SS
    SoftwareSerial _module(RX, TX);
#endif

/**
    @function: Constructor
    
*/
Sim800l::Sim800l(){
    _module.begin(BAUD);
    _buffer.reserve(255);
}

/**
    function: _read
    @summary: read String from serial
    @parameter: none
    @return:
        String: data read from the serial buffer
*/
String Sim800l::_read(){
    // unsigned int timeout = 0;
    String buffer = "";
    while(_module.available()){
        buffer += _module.read();
    }
    /** // reading with timeout
    while(true){
        if(_module.available()){
            _buffer += _module.read();
            timeout = 0;
            continue;
        }
        timeout++;
        if(timeout >= 60000) break;
    }
    */
    return buffer;
}

/**
    function: ping
    @summary: send an echo command to the module
    @parameter: none
    @return:
        bool: return true if response received and false if not
*/
bool Sim800l::ping(){
    _module.println(F("at"));
    _buffer = _read();
    if(_buffer.indexOf("OK")!= -1) return true;
    else return false;
}
