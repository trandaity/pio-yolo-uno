#include "M5UnitHbridge.h"

extern M5UnitHbridge mtDriver;
extern uint8_t fw_version;

void getCurrentVoltage();
void hBridgeDriverRun(void* parameter);
void hBridgeDriveForward();
void hBridgeDriveBackward();
void hBridgeStop();