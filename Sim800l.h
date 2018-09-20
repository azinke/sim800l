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

#define DEBUG

#define SS
#define RX 10
#define TX 11
#define BAUD 9600

typedef struct {
    String result;
    uint8_t status = 0;
} RESPONSE;

class Sim800l{
    public:
        Sim800l();
        // void begin();
        
        bool ping();
        bool setEchoMode(bool enable);
        
    private:
        RESPONSE _buffer;        // allocate buffer in memory
        RESPONSE _read();
        inline bool _ack();
};


#endif
