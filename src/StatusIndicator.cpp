#include "StatusIndicator.h"
#include "NetworkSettings.h"
#include "MqttSettings.h"
#include "Hoymiles.h"
#include <Arduino.h>


void LEDsetup()
{
    // netowrk LED
    pinMode(14, OUTPUT);
    digitalWrite(14, LOW);
    pinMode(12, OUTPUT);
    digitalWrite(12, LOW);
    //inverter LED
    pinMode(27, OUTPUT);
    digitalWrite(27, LOW);
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW); 
}

void setNetworkLED(LEDcolor color) {
    switch(color) {
        case off: 
            digitalWrite(13, LOW);
            digitalWrite(27, LOW);
            break;  
        case green:
            digitalWrite(13, HIGH);
            digitalWrite(27, LOW);
            break;  
        case orange:
            digitalWrite(13, HIGH);
            digitalWrite(27, HIGH);
            break;
        case red:
            digitalWrite(13, LOW);
            digitalWrite(27, HIGH);
            break;    

    }
}


void setInvLED(LEDcolor color) {
    switch(color) {
        case off: 
            digitalWrite(14, LOW);
            digitalWrite(12, LOW);
            break;  
        case green:
            digitalWrite(14, LOW);
            digitalWrite(12, HIGH);
            break;  
        case orange:
            digitalWrite(14, HIGH);
            digitalWrite(12, HIGH);
            break;
        case red:
            digitalWrite(14, HIGH);
            digitalWrite(12, LOW);
            break;    

    }
}

void LEDloop()
{
    if (NetworkSettings.isConnected() && MqttSettings.getConnected()) {
        setNetworkLED(green);
    } 
    if (NetworkSettings.isConnected() && !MqttSettings.getConnected()) {
        setNetworkLED(orange);
    }
    if (!NetworkSettings.isConnected()) {
        // mqtt is definetly not connected
        setNetworkLED(red);
    }
    // inverter status, iterate over all inverter
    if (Hoymiles.getNumInverters()) {
        bool allReachable=true;
        bool allProducing=true;
        for (uint8_t i = 0; i < Hoymiles.getNumInverters(); i++) {
            auto inv = Hoymiles.getInverterByPos(i);
            if (!inv->isReachable()) {
                allReachable=false;
            }
            if (!inv->isProducing()) {
                allProducing=false;
            }
        }
        // set LED status
        if (allReachable && allProducing) {
            setInvLED(green);
        } 
        if (allReachable && !allProducing) {
            setInvLED(orange);
        } 
        if (!allReachable && !allProducing) {
            setInvLED(red);
        }
    }   
}
