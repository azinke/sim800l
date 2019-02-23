/** On Off
    Switch an LED on an off  by SMS
*/
#include <Sim800l.h>
#define LED 6

Sim800l modem;

String sms = "";
/**
    SMS struct{
        String status;
        String number;
        String date;
        String time;
        String content;
    }
**/
SMS parsedSMS;

uint8_t index = 1;

void setup(){
    Serial.begin(9600);
    delay(100);
    
    while(!modem.ping()){
        Serial.println("Modem not found!");
        delay(200);
    }
    Serial.println("Modem connected!");
    Serial.flush();

    // delete all sms
    modem.deleteAllSMS();
}

void loop(){
    sms = modem.readSMS(index);
    if(sms == "") return;
    Serial.println("New raw SMS");
    Serial.println(sms);

    parsedSMS = modem.parseSMS(sms);
    Serial.println("Parsed SMS:");
    Serial.print("Status: ");
    Serial.println(parsedSMS.status);
    Serial.print("Number: ");
    Serial.println(parsedSMS.number);
    Serial.print("Date: ");
    Serial.println(parsedSMS.date);
    Serial.print("Time:  ");
    Serial.println(parsedSMS.time);
    Serial.print("Content: ");
    Serial.println(parsedSMS.content);
    
    if(sms.indexOf("ON") != -1){
        digitalWrite(LED, HIGH);
    }else if(sms.indexOf("OFF") != -1){
        digitalWrite(LED, LOW);
    }
    modem.deleteAllSMS();
}
