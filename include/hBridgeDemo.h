#include "M5UnitHbridge.h"

extern M5UnitHbridge mtDriver;
extern uint8_t fw_version;

void getCurrentVoltage();
void hBridgeDriverRun(void* parameter);
void ERa_HBridgeDriveForward();
void ERa_HBridgeDriveBackward();
void ERa_HBridgeStop();