/**
    @autor: AMOUSSOU Z. Kenneth
    @date: 17/09/2018
    @version: 1.0
*/
#ifndef H_SIM800L
#define H_SIM800L

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <SoftwareSerial.h>

#define SS
#define RX 10
#define TX 11
#define BAUD 9600



class Sim800l{
    public:
        Sim800l(uint8_t rx, uint8_t tx);
        // void begin();
        
        bool ping();
        
    private:
        String _buffer.reserve(255);        // allocate buffer in memory
        String _read();
        #ifdef SS
            SoftwareSerial _module(RX, TX);
        #endif
};


#endif
