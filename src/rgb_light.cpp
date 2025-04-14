#include "rgb_light.h"

const int rgb_pin = 6;

Adafruit_NeoPixel rgb_1(4, rgb_pin, NEO_GRB + NEO_KHZ800);

void turnOnPixels() {
  rgb_1.setPixelColor(0, rgb_1.Color(255, 255, 255));
  rgb_1.setPixelColor(1, rgb_1.Color(255, 255, 255));
  rgb_1.setPixelColor(2, rgb_1.Color(255, 255, 255));
  rgb_1.setPixelColor(3, rgb_1.Color(255, 255, 255));
  rgb_1.show();
}

void turnOffPixels() {
  rgb_1.setPixelColor(0, rgb_1.Color(0, 0, 0));
  rgb_1.setPixelColor(1, rgb_1.Color(0, 0, 0));
  rgb_1.setPixelColor(2, rgb_1.Color(0, 0, 0));
  rgb_1.setPixelColor(3, rgb_1.Color(0, 0, 0));
  rgb_1.show();
}

ERA_WRITE(V3) {
  int val = param.getInt();

  val ? turnOnPixels() : turnOffPixels();
}