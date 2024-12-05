#include <irSensor.h>

bool printFlag = true;

void setupIrSensor() {
	pinMode(IR_SENSOR_PIN, INPUT);
}

void readFromIrSensor(void* parameter) {
	while (1) {
		int sensorState = digitalRead(IR_SENSOR_PIN);
		switch (sensorState)
		{
		case LOW: {
			if (printFlag) {
				Serial.println("Obstacle Detected!");
				printFlag = false;
			}

			break;
		}

		case HIGH: {
			if (!printFlag) {
				Serial.println("No Detection!");
				printFlag = true;
			}

			break;
		}

		default:
			break;
		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}