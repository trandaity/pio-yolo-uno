#include "dht20_tasks.h"

DHT20 DHT_1(&Wire);
int powerVal = 0;

void DHT20_init() {
  Serial.println(__FILE__);
  Serial.print("DHT20 LIBRARY VERSION: ");
  Serial.println(DHT20_LIB_VERSION);
  Serial.println();

  Wire.setPins(DHT20_SDA, DHT20_SCL);
  Wire.begin();

  DHT_1.begin();

  delay(2000);

  Serial.println("Type,\tStatus,\tHumidity (%),\tTemperature (C)");
}

void DHT20_run(void* parameters) {
  while (1) {
    Serial.print("DHT20, \t");
    int status = DHT_1.read();
    switch (status)
    {
    case DHT20_OK:
      Serial.print("OK,\t");
      break;
    case DHT20_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    case DHT20_ERROR_CONNECT:
      Serial.print("Connect error,\t");
      break;
    case DHT20_MISSING_BYTES:
      Serial.print("Missing bytes,\t");
      break;
    case DHT20_ERROR_BYTES_ALL_ZERO:
      Serial.print("All bytes read zero");
      break;
    case DHT20_ERROR_READ_TIMEOUT:
      Serial.print("Read time out");
      break;
    case DHT20_ERROR_LASTREAD:
      Serial.print("Error read too fast");
      break;
    default:
      Serial.print("Unknown error,\t");
      break;
    }

    //  DISPLAY DATA, sensor has only one decimal.
    Serial.print(DHT_1.getHumidity());
    Serial.print(",\t");
    Serial.println(DHT_1.getTemperature());

    vTaskDelay(DHT20_TASK_DELAY / portTICK_PERIOD_MS);
  }
}

void ERa_DHTReadEvent() {
  if (DHT_1.isMeasuring()) {
    return;
  }

  if (DHT_1.readData() < 0) {
    return;
  }

  if (DHT_1.convert() == DHT20_OK) {
    ERa.virtualWrite(V0, round(DHT_1.getTemperature()));
    ERa.virtualWrite(V1, round(DHT_1.getHumidity()));
  }

  DHT_1.requestData();
}

void powerMonitor(void* parameters) {
  while (1) {
    Serial.printf("\n Power Consumption: ");
    powerVal = 100 + rand() % 100;
    Serial.print(powerVal);
    Serial.printf("\n");

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}