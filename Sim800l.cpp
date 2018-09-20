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
    _buffer.status = 0;
    _buffer.result.reserve(255);
}

/**
    function: _read
    @summary: read String from serial
    @parameter: none
    @return:
        String: data read from the serial buffer
*/
RESPONSE Sim800l::_read(){
    unsigned long timeout = 200;
    RESPONSE buffer;
    while(!_module.available() && timeout--){
        delayMicroseconds(10);
    }
    if(timeout == 0){
        buffer.status = 0;
        buffer.result = "";
        return buffer;
    }
    buffer.status = 1;                      // set status
    buffer.result = _module.readString();   // write output data
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
    _module.print(F("AT\r\n"));
    _buffer = _read();
    if(_buffer.status){
        if(_buffer.result.indexOf("OK")!= -1) return true;
        else return false;
    }else return false;
}
