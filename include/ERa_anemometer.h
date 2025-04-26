#include "Arduino.h"
#include "ERa.hpp"

extern unsigned long lastDebounceTime;
extern unsigned long debounceDelay;

extern int anemo_count;
extern float wind_speed;

extern int pinInterrupt;

// Detect wheel spin and increment count, the pin state changes from HIGH to LOW
void onChange();
// void anemoSetup(void* pvParameters);
// void printWindSpeedAndResetCount(void* pvParameters);