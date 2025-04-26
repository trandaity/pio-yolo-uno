#include "ERa_anemometer.h"

int pinInterrupt = 21; // D10 - GPIO21
int anemo_count = 0;
float wind_speed = 0.0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 1000;

void onChange() {
  if (digitalRead(pinInterrupt) == LOW) {
    anemo_count++;
  }
}
