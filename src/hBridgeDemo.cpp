#include "hBridgeDemo.h"

M5UnitHbridge mtDriver;
uint8_t fw_version = 0;
int dr_speed = 0;

void getCurrentVoltage() {
  if (fw_version >= 2) {
    Serial.printf("%.2fA\r\n", mtDriver.getMotorCurrent());
  }

  Serial.printf("%.2fV\r\n", mtDriver.getAnalogInput(_12bit) / 4095.0f * 3.3f / 0.09f);
}

void ERa_HBridgeDriveForward() {
  mtDriver.setDriverDirection(HBRIDGE_FORWARD);
}

void ERa_HBridgeDriveBackward() {
  mtDriver.setDriverDirection(HBRIDGE_BACKWARD);
}

void ERa_HBridgeStop() {
  mtDriver.setDriverDirection(HBRIDGE_STOP);
}

void ERa_setSpeed8b() {
  mtDriver.setDriverSpeed8Bits(dr_speed);
}

void hBridgeDriverRun(void* parameter) {
  while (1) {
    mtDriver.setDriverDirection(HBRIDGE_FORWARD);
    mtDriver.setDriverSpeed8Bits(127);
    for (int i = 100; i > 0; i--) {
        getCurrentVoltage();
    }

    mtDriver.setDriverDirection(HBRIDGE_STOP);
    mtDriver.setDriverSpeed8Bits(0);
    for (int i = 100; i > 0; i--) {
         getCurrentVoltage();
    }

    mtDriver.setDriverDirection(HBRIDGE_BACKWARD);
    mtDriver.setDriverSpeed8Bits(127);

    for (int i = 100; i > 0; i--) {
        getCurrentVoltage();
    }

    mtDriver.setDriverDirection(HBRIDGE_STOP);
    mtDriver.setDriverSpeed8Bits(0);
    for (int i = 100; i > 0; i--) {
        getCurrentVoltage();
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}