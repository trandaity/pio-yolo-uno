#include "DHT20.h"
#include "Wire.h"
#include "Arduino.h"
#include "ERa.hpp"

#define DHT20_SDA 11
#define DHT20_SCL 12
#define DHT20_TASK_DELAY 2000

extern DHT20 DHT_1;
extern int powerVal;

void DHT20_init();
void DHT20_run(void* parameters);
void powerMonitor(void* parameters);
void ERa_DHTReadEvent();
