#ifndef H_STATUSINDICATOR
#define H_STATUSINDICATOR

// GPIO of LED indicators, use RG(B) LED, blue is not used
// Inverter
const int StatusInvRed = 14;
const int StatusInvGreen = 12;
// Netowrk connectivity
const int StatusNetRed = 27;
const int StatusNetGreen = 13; 

enum LEDcolor {off=0, green, orange, red};

void LEDsetup();
void LEDloop();

void setInvLED(LEDcolor color);
void setNetworkLED(LEDcolor color);

#endif