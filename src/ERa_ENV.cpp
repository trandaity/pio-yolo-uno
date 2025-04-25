#include "ERa_ENV.h"

SHT4X sht;
BMP280 bmp;

double dewPoint = 0;
double trunc_temp = 0;
double trunc_humi = 0;

void m5Env_init(TwoWire &wire) {
  if (!sht.begin(&wire, SHT40_I2C_ADDR_44, sda, scl, 400000U)) {
    Serial.println("Could not find SHT4x");
    while (1) delay(1);
  }

  sht.setPrecision(SHT4X_MED_PRECISION);
  sht.setHeater(SHT4X_NO_HEATER);

  if (!bmp.begin(&wire, BMP280_I2C_ADDR, sda, scl, 400000U)) {
    Serial.println("Could not find BMP280");
    while (1) delay(1);
  }

  bmp.setSampling(BMP280::MODE_NORMAL,     /* Operating Mode. */
                  BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  BMP280::FILTER_X16,      /* Filtering. */
                  BMP280::STANDBY_MS_500); /* Standby time. */
}

void ERa_ENVReadEvent() {
  if (sht.update()) {
    dewPoint = sht.cTemp - ((100.0 - sht.humidity) / 5.0);
    trunc_temp = round(sht.cTemp * 100) / 100.0;
    trunc_humi = round(sht.humidity * 100) / 100.0;
    dewPoint = round(dewPoint * 100) / 100.0;

    Serial.println("-----SHT4X-----");
    Serial.print("Temperature: ");
    Serial.print(sht.cTemp);
    Serial.println(" degrees C");
    Serial.print("Humidity: ");
    Serial.print(sht.humidity);
    Serial.println("% rH");

    Serial.print("Dew Point: ");
    Serial.print(dewPoint);
    Serial.println(" degrees C");
    Serial.println("-------------\r\n");

    ERa.virtualWrite(V14, trunc_temp);
    ERa.virtualWrite(V15, trunc_humi);
    ERa.virtualWrite(V17, dewPoint);
  }

  if (bmp.update()) {

    Serial.println("-----BMP280-----");
    Serial.print(F("Pressure: "));
    Serial.print(bmp.pressure);
    Serial.println(" Pa");
    Serial.println("-------------\r\n");

    ERa.virtualWrite(V16, bmp.pressure);
  }
}
