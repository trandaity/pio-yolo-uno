#include "socket_ctrl.h"

UNIT_4RELAY relay_0;
UNIT_ACMEASURE sensor_0;

void ERa_ACReadEvent() {
  // if (sensor_0.getReady()) {
    
  // }

  Serial.print("Voltage (V):\t");
  Serial.println((float)(sensor_0.getVoltage()) / 100.0);
  Serial.print("Current (A):\t");
  Serial.println((float)(sensor_0.getCurrent()) / 100.0);
  Serial.print("Power (W):\t");
  Serial.println((float)(sensor_0.getPower()) / 100.0);
  Serial.print("Apparent Power (VA):\t");
  Serial.println((float)(sensor_0.getApparentPower()) / 100.0);
  Serial.print("Power Factor:\t");
  Serial.println((float)(sensor_0.getPowerFactor()) / 100.0);
  Serial.print("Energy (kW.h):\t");
  Serial.println((float)(sensor_0.getKWH()) / 100.0);
  Serial.print("\n");

  ERa.virtualWrite(V8, (float)(sensor_0.getVoltage()) / 100.0);
  ERa.virtualWrite(V9, (float)(sensor_0.getCurrent()) / 100.0);
  ERa.virtualWrite(V10, (float)(sensor_0.getPower()) / 100.0);
  ERa.virtualWrite(V11, (float)(sensor_0.getKWH()) / 100.0);
  ERa.virtualWrite(V12, (float)(sensor_0.getApparentPower()) / 100.0);
  ERa.virtualWrite(V13, (float)(sensor_0.getPowerFactor()) / 100.0);
}

ERA_WRITE(V4) {
  int val = param.getInt();

  relay_0.relayWrite(0, val);
}

ERA_WRITE(V5) {
  int val = param.getInt();

  relay_0.relayWrite(1, val);
}

ERA_WRITE(V6) {
  int val = param.getInt();

  relay_0.relayWrite(2, val);
}

ERA_WRITE(V7) {
  int val = param.getInt();

  relay_0.relayWrite(3, val);
}