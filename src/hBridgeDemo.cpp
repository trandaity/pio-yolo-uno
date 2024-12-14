#include "hBridgeDemo.h"

M5UnitHbridge mtDriver;
uint8_t fw_version = 0;

void getCurrentVoltage() {
  if (fw_version >= 2) {
    Serial.printf("%.2fA\r\n", mtDriver.getMotorCurrent());
  }

  Serial.printf("%.2fV\r\n", mtDriver.getAnalogInput(_12bit) / 4095.0f * 3.3f / 0.09f);
}

void hBridgeDriveForward() {
  
}

void hBridgeDriveBackward() {

}

void hBridgeStop() {

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