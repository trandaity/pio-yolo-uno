#include "M5UnitENV.h"
#include "ERa.hpp"
#include "Wire.h"

#define sda 11
#define scl 12

extern SHT4X sht;
extern BMP280 bmp;

extern double trunc_temp;
extern double trunc_humi;
extern double dewPoint;

void m5Env_init(TwoWire &wire);
void ERa_ENVReadEvent();