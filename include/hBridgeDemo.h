#include "M5UnitHbridge.h"

extern M5UnitHbridge mtDriver;
extern uint8_t fw_version;
extern int dr_speed;

void getCurrentVoltage();
void hBridgeDriverRun(void* parameter);
void ERa_HBridgeDriveForward();
void ERa_HBridgeDriveBackward();
void ERa_HBridgeStop();
void ERa_setSpeed8b();