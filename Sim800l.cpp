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
RESPONSE Sim800l::_read(unsigned long timeout = 2000000){
    RESPONSE buffer;
    while(!_module.available() && timeout--){
        delayMicroseconds(1);
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
    function: _ack
    @summary: read a "OK" response from the module
    @parameter: none
                this function use the _buffer variable directly
    @return:
        bool: return "true" if ACK received and "false" if not
*/
bool Sim800l::_ack(){
    if(_buffer.status){
        if(_buffer.result.indexOf("OK")!= -1) return true;
        else return false;
    }else return false;
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
    #ifdef DEBUG
        Serial.println(_buffer.result);
    #endif
    return _ack();
}

/**
    function: setEchoMode
    @summary: enable or disable echo mode on the ME
    @parameter: none
    @return:
        bool: return true if response received and false if not
*/
bool Sim800l::setEchoMode(bool enable){
    if(enable){
        _module.print(F("ATE1\r\n"));
        _buffer = _read();
        return _ack();
    }else{
        _module.print(F("ATE0\r\n"));
        _buffer = _read();
        return _ack();
    }
}

/**
    function: deleteAllSMS
    @summary: delete all message from memory
              This function only handle text mode message
    @parameter: none
    @return:
        bool: return true if response received and false if not
*/
bool Sim800l::deleteAllSMS(){
    _module.print(F("AT+CMGDA=\"DEL ALL\"\r\n"));
    _buffer = _read();
    return _ack();
}

/**
    function: deleteSMS
    @summary: delete message from prefered memory at specified index
    @parameter:
        index: messsage index in memory
        delflag: 
            [0]: delete the message specified in <index>
            [1]: delete all read messages from preferred message storage,
                 leaving unread messages and stored mobile originated messages
                 (whether sent or not) untouched
            [2]: Delete all read messages from preferred message storage,
                 and sent mobile originated messages, leaving unread 
                 messages and unsent mobile originated messages untouched
            [3]: Delete all read messages from preferred message storage,
                 sent and unsent mobile originated messages leaving
                 unread messages untouched
            [4]: Delete all messages from preferred message storage including 
                 unread messages
    @return:
        bool: return true if response received and false if not
*/
bool Sim800l::deleteSMS(uint8_t index, uint8_t delflag = 0){
    if(delflag > 4) return false;
    _module.print(F("AT+CMGD="));
    _module.print(String(index));
    _module.print(F(","));
    _module.print(String(delflag));
    _module.print(F("\r\n"));
    _buffer = _read(25000000);   // ~25s of response time
    return _ack();
}

/**
    function: setSmsFormat
    @summary: set the format of SMS
    @parameter:
        format:
            0: PUD mode
            1: text mode
    @return:
        bool: return true if response received and false if not
*/
bool Sim800l::setSmsFormat(uint8_t format){
    if(format == 0){
        _module.print(F("AT+CMGF=0\r\n"));
        _buffer = _read();
        return _ack();
    }else if(format ==  1){
        _module.print(F("AT+CMGF=1\r\n"));
        _buffer = _read();
        return _ack();
    }else return false;
}

/**
    function: readSMS
    @summary: read SMS from memory
    @parameter:
        index: value in the range of location numbers supported by the 
               associated memory
        mode: 
            [0]: normal
            [1]: not change status of the specified SMS record
    @return:
        String: sms content
*/
String Sim800l::readSMS(uint8_t index, uint8_t mode = 0){
    if(mode > 1) return "";
    if(setSmsFormat(1)){    // text mode
        _module.print(F("AT+CMGR="));
        _module.print(String(index));
        _module.print(F(","));
        _module.print(String(mode));
        _module.print(F("\r\n"));
        _buffer = _read(5000000); // ~5s of response time
        #ifdef DEBUG
            Serial.println(_buffer.result);
        #endif  
        uint8_t idx = _buffer.result.indexOf("+CMGR:");
        return _buffer.result.substring(idx+6);
    }else return "";
}

/**
    function: parseSMS
    @summary: get out of an sms read from memory useful informations such as
              date, time, sender's number, status and sms content
    @parameter:
        sms: raw sms read from the module
    @return:
        SMS: parsed SMS
*/
SMS Sim800l::parseSMS(String sms){
    SMS parsedSMS;
    uint8_t idx1 = 0;
    uint8_t idx2 = 0;
    idx1 = sms.indexOf("\",\"");
    idx2 = sms.indexOf("\",\"", idx1+3);
    parsedSMS.status = sms.substring(0, idx1);
    parsedSMS.number = sms.substring(idx1+3, idx2);
    idx1 = sms.indexOf("\",\"",idx2 + 2);
    idx2 = sms.indexOf("\"", idx1 + 3);
    parsedSMS.date = sms.substring(idx1 + 3, idx2);
    idx1 = parsedSMS.date.indexOf(",");
    parsedSMS.time = parsedSMS.date.substring(idx1+1);
    parsedSMS.date = parsedSMS.date.substring(0, idx1);
    parsedSMS.content = sms.substring(idx2 + 2);
    idx1 = parsedSMS.content.indexOf("OK");
    parsedSMS.content = parsedSMS.content.substring(0, idx1);
    parsedSMS.content.trim();
    return parsedSMS;
}

/**
    function: sendSMS
    @summary: send an SMS to a specific phone number
    @parameter:
        phone_number: number to send the message to
        text: content of the message
    @return:
        bool: return true if response received and false if not
*/
bool Sim800l::sendSMS(String phone_number, String text){
    if(setSmsFormat(1)){    // text mode
        _module.print(F("AT+CMGS=\""));
        _module.print(String(phone_number));
        _module.print(F("\"\r"));
        _module.print(text);
        _module.print(F("\r"));
        _module.print((char)26);
        _buffer = _read(60000000); // ~60s of response time
        if(_buffer.result.indexOf("+CMGS") != -1) return true;
        else return false;
    }else return false;
}

/**
    function: setPreferedSmsStorage
    @summary: set prefered SMS storage
    @parameter:
        storage: integer
            [0]: "SM" - SIM message storage
            [1]: "ME" - Phone message storage
            [2]: "SM_P" - SM message storage preferred
            [3]: "ME_P" - ME message storage preferred
            [4]: "MT" - SM or ME message storage ( SM preferred)
    @return:
        bool: return true if response received and false if not
*/
bool Sim800l::setPreferedSmsStorage(uint8_t storage = 0){
    if(storage > 4) return false;
    switch(storage){
        case 0: {
            _module.print(F("AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n"));
            break;
        }
        case 1:{
            _module.print(F("AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n"));
            break;
        }
        case 2:{
            _module.print(F("AT+CPMS=\"SM_P\",\"SM_P\",\"SM_P\"\r\n"));
            break;
        }
        case 3:{
            _module.print(F("AT+CPMS=\"ME_P\",\"ME_P\",\"ME_P\"\r\n"));
            break;
        }
        case 4:{
            _module.print(F("AT+CPMS=\"MT\",\"MT\",\"MT\"\r\n"));
            break;
        }
    }
    _buffer = _read();
    return _ack();
}

/**
    function: configureSmsCenter
    @summary: configure SMS Service center address
    @parameter:
        number: SMS service center number
    @return:
        bool: return true if response received and false if not
*/
bool Sim800l::configureSmsCenter(String number){
    _module.print(F("AT+CSCA=\""));
    _module.print(String(number));
    _module.print(F("\"\r\n"));
    _buffer = _read(5000000); // ~5s of response time
    return _ack();
}

/**
    function: status
    @summary: read phone activity status
    @parameter: none
    @return:
        uint8_t:
            [0]: Ready (MT allows commands from TA/TE)
            [2]: Unknown (MT is not guaranteed to respond to tructions)
            [3]: Ringing (MT is ready for commands from TA/TE, but the ringer
                 is active)
            [4]: Call in progress (MT is ready for commands from TA/TE, a call 
                 is in progress)
*/
uint8_t Sim800l::status(){
    _module.print(F("AT+CPAS\r\n"));
    _buffer = _read();
    return int(_buffer.result.charAt(7));
}

/**
    function: findPhonebookEntry
    @summary: find a phone number in memory
    @parameter:
        text: number to research
    @return:
        String: 
*/
String Sim800l::findPhonebookEntry(String text){
    _module.print(F("AT+CPBF=\""));
    _module.print(text);
    _module.print(F("\"\r\n"));
    _buffer = _read();
    return _buffer.result;
}

/**
    function: readPhonebookEntry
    @summary: read a phonebook entry from preferred memory
              return entries from <index1> to <index2>
    @parameter:
        index1: starting index of the phonebook entry
        index2: ending index
    @return:
        String: phonebook entry information
*/
String Sim800l::readPhonebookEntry(uint8_t index1, uint8_t index2 = 0){
    if(index2){
        _module.print(F("AT+CPBR="));
        _module.print(String(index1));
        _module.print(F(","));
        _module.print(String(index2));
        _module.print(F("\r\n"));
    }else{
        _module.print(F("AT+CPBR="));
        _module.print(String(index1));
        _module.print(F("\r\n"));
    }
    _buffer = _read();
    return _buffer.result;
}

/**
    function: setPhonebookMemoryStorage
    @summary: select the memory storage for phonebook transaction (writing, 
              reading, ...)
    @parameter:
        storage: 
            [0]: "SM" - SIM phonebook
            [1]: "ME" - Mobile Equipment phonebook
            [2]: "FD" - SIM fix dialing-phone book
    @return:
        String: phonebook entry information
*/
bool Sim800l::setPhonebookMemoryStorage(uint8_t storage = 0){
    if(storage > 2) return false;
    switch(storage){
        case 0:{
            _module.print(F("AT+CPBS=\"SM\"\r\n"));
            break;
        }  
        case 1:{
            _module.print(F("AT+CPBS=\"ME\"\r\n"));
            break;
        }
        case 2:{
            _module.print(F("AT+CPBS=\"FD\"\r\n"));
            break;
        }
    }
    _buffer = _read(3000000); // ~3s of response time
    return _ack();
}

/**
    function: writePhonebookEntry
    @summary: write a phone number in memory
    @parameter:
        index: index of the new entry
        name: name of the contact
        number: phone number
    @return:
        bool: return true if response received and false if not
*/
bool Sim800l::writePhonebookEntry(String name, String number, uint8_t index = 0){
    _module.print(F("AT+CPBW="));
    if(index != 0){
        _module.print(String(index));
        _module.print(F(","));
    }
    _module.print(F("\""));
    _module.print(number);
    _module.print(F("\""));
    if(number.charAt(0) == '+'){
        _module.print(F(",\"145\""));
    }else{ _module.print(F(",\"129\"")); }
    _module.print(F(",\""));
    _module.print(name);
    _module.print(F("\"\r\n"));
    _buffer = _read();
    return _ack();
}
