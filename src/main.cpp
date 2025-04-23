/*************************************************************
  Download latest ERa library here:
    https://github.com/eoh-jsc/era-lib/releases/latest
    https://www.arduino.cc/reference/en/libraries/era
    https://registry.platformio.org/libraries/eoh-ltd/ERa/installation

    ERa website:                https://e-ra.io
    ERa blog:                   https://iotasia.org
    ERa forum:                  https://forum.eoh.io
    Follow us:                  https://www.fb.com/EoHPlatform
 *************************************************************/

// Enable debug console
// Set CORE_DEBUG_LEVEL = 3 first
#define ERA_DEBUG
#define ERA_SERIAL Serial

/* Select ERa host location (VN: Viet Nam, SG: Singapore) */
#define ERA_LOCATION_VN
// #define ERA_LOCATION_SG

// You should get Auth Token in the ERa App or ERa Dashboard
#define ERA_AUTH_TOKEN "500a2857-19ae-47ee-b2da-b5bc4830dd6b"

/* Define setting button */
// #define BUTTON_PIN              0

/* User-defined preprocessor */
#define SDA_0 11
#define SCL_0 12
#define SDA_1 17
#define SCL_1 18

#if defined(BUTTON_PIN)
// Active low (false), Active high (true)
#define BUTTON_INVERT false
#define BUTTON_HOLD_TIMEOUT 5000UL

// This directive is used to specify whether the configuration should be erased.
// If it's set to true, the configuration will be erased.
#define ERA_ERASE_CONFIG false
#endif

#include <Arduino.h>
#include <freeRTOS/FreeRTOS.h>
#include <ERa.hpp>
#include <Widgets/ERaWidgets.hpp>

#include <stepperMotor.h>
#include <irSensor.h>
#include <hBridgeDemo.h>
#include <rgb_light.h>
#include <socket_ctrl.h>

#if defined(BUTTON_PIN)
  #include <pthread.h>
  #include <ERa/ERaButton.hpp>
#endif

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t running_core = 0;
#else
  static const BaseType_t running_core = 1;
#endif

const char ssid[] = "TyTran";
const char pass[] = "vectortran151";

WiFiClient mbTcpClient;

static ERaFlash flash;
static WiFiClient ERaWiFiClient;
static ERaMqtt<WiFiClient, MQTTClient> mqtt(ERaWiFiClient);
ERaPnP< ERaMqtt<WiFiClient, MQTTClient> > ERa(mqtt, flash);

#if defined(BUTTON_PIN)
ERaButton button;
pthread_t pthreadButton;

static void *handlerButton(void *args)
{
  for (;;)
  {
    button.run();
    ERaDelay(10);
  }
  pthread_exit(NULL);
}

#if ERA_VERSION_NUMBER >= ERA_VERSION_VAL(1, 2, 0)
static void eventButton(uint8_t pin, ButtonEventT event)
{
  if (event != ButtonEventT::BUTTON_ON_HOLD)
  {
    return;
  }
  ERa.switchToConfig(ERA_ERASE_CONFIG);
  (void)pin;
}
#else
static void eventButton(ButtonEventT event)
{
  if (event != ButtonEventT::BUTTON_ON_HOLD)
  {
    return;
  }
  ERa.switchToConfig(ERA_ERASE_CONFIG);
}
#endif

void initButton()
{
  pinMode(BUTTON_PIN, INPUT);
  button.setButton(BUTTON_PIN, digitalRead, eventButton,
                   BUTTON_INVERT)
      .onHold(BUTTON_HOLD_TIMEOUT);
  pthread_create(&pthreadButton, NULL, handlerButton, NULL);
}
#endif

/* This function will run every time ERa is connected */
ERA_CONNECTED()
{
  ERA_LOG("ERa", "ERa connected!");
}

/* This function will run every time ERa is disconnected */
ERA_DISCONNECTED()
{
  ERA_LOG("ERa", "ERa disconnected!");
}

ERA_WRITE(V21) // Text box Widget
{
  if (!param.isString())
  {
    return;
  }

  const char *cstr = param.getString();

  if (cstr)
  {
    Serial.print("\n From ERa Text box Widget: ");
    Serial.print(cstr);
    Serial.print("\n");
  }
}

ERA_WRITE(V24) // Rotate Camera anti-Clockwise
{
  int value = param.getInt();

  if (value == 24) {
    rotateAntiClockwise();

    Serial.print("\n The stepper's current position: ");
    Serial.print(stepper1.currentPosition());
    Serial.print("\n");
  }
}

ERA_WRITE(V25) // Rotate Camera Clockwise
{
  int value = param.getInt();

  if (value == 25) {
    rotateClockwise();

    Serial.print("\n The stepper's current position: ");
    Serial.print(stepper1.currentPosition());
    Serial.print("\n");
  }
}

ERA_WRITE(V26) // Set the initial zero position of the camera
{
  int value = param.getInt();

  if (value == 26) {
    setZeroPosition();

    Serial.print("\n The initial zero position have been set!");
    Serial.print("\n The stepper's current position: ");
    Serial.print(stepper1.currentPosition());
    Serial.print("\n");
  }
}

ERA_WRITE(V27)
{
  int value = param.getInt();

  if (value == 27) {
    stepMultiplierIncrement();

    Serial.print("\n The step multiplier has been incremented");
    Serial.print("\n Current step multiplier: ");
    Serial.print(stepMultiplier);
    Serial.print("\n");
  }
}

ERA_WRITE(V28)
{
  int value = param.getInt();

  if (value == 28) {
    stepMultiplierDecrement();

    Serial.print("\n The step multiplier has been decremented");
    Serial.print("\n Current step multiplier: ");
    Serial.print(stepMultiplier);
    Serial.print("\n");
  }
}

ERA_WRITE(V30) {        // Set DC Motor Direction to Forward
  int value = param.getInt();

  if (value == 30) {
    ERa_HBridgeDriveForward();

    Serial.println("\n DC Motor Current Direction: Forward");
  }
}

ERA_WRITE(V31) {        // Stop the DC Motor
  int value = param.getInt();

  if (value == 31) {
    ERa_HBridgeStop();

    Serial.println("\n DC Motor Has Stopped!");
  }
}

ERA_WRITE(V32) {        // Set DC Motor Direction to Backward
  int value = param.getInt();

  if (value == 32) {
    ERa_HBridgeDriveBackward();

    Serial.println("\n DC Motor Current Direction: Backward");
  }
}

ERA_WRITE(V33) {        // Set DC Motor Speed (for 8-bits speed value)
  int value = param.getInt();
  dr_speed = 2.55f * value;

  ERa_setSpeed8b();
  Serial.printf("\n DC Motor Speed (8-bits): %d", dr_speed);
}

/* This function print uptime every second */
void timerEvent()
{
  //ERa_DHTReadEvent();
  ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
}

ERaString estr;
ERaWidgetTerminalBox IrSensorTerminal(estr, V22, V23);

byte busStatus;

/*-------------- Initialize I2C hardware instances ---------------*/ 
TwoWire I2C_0 = TwoWire(0);
TwoWire I2C_1 = TwoWire(1);

void IrSensorTerminalCallBack() {
  if (estr == "Hi!") {
    IrSensorTerminal.print("Hello! ");
  }

  IrSensorTerminal.print("Thank you for using ERa");
  IrSensorTerminal.flush();
}

void setup()
{
  /* Setup debug console */
#if defined(ERA_DEBUG)
  Serial.begin(115200);
#endif

#if defined(BUTTON_PIN)
  /* Initializing button. */
  initButton();
  /* Enable read/write WiFi credentials */
  ERa.setPersistent(true);
#endif

  /* Set board id */
  // ERa.setBoardID("Board_1");

  /* Setup Client for Modbus TCP/IP */
  ERa.setModbusClient(mbTcpClient);

  /* Set scan WiFi. If activated, the board will scan
     and connect to the best quality WiFi. */
  ERa.setScanWiFi(true);

  /* Initializing Terminal box Widget with callback function: terminalCallBack */
  IrSensorTerminal.begin(IrSensorTerminalCallBack);

  /* Initializing the ERa library. */
  ERa.begin(ssid, pass);

  /* Setup timer called function every second */
  //ERa.addInterval(1000L, timerEvent);

  /*-------------- Begin I2C interface ---------------*/
  I2C_0.begin(SDA_0, SCL_0, 100000U);
  I2C_1.begin(SDA_1, SCL_1, 100000U);

  /*-------------- Initialize Peripherals ---------------*/ 
  // DHT20_init();
  // rgb_1.begin();
  relay_0.begin(&I2C_0, SDA_0, SCL_0);
  relay_0.Init(1);
  sensor_0.begin(&I2C_0, UNIT_ACMEASURE_ADDR, SDA_0, SCL_0);

  /*-------------- I2C Devices Scanner ---------------*/ 
  for (int i2cAddress = 0x00; i2cAddress < 0x80; i2cAddress++)
  {
    I2C_0.beginTransmission(i2cAddress);
    busStatus = I2C_0.endTransmission();
    if (busStatus == 0x00)
    {
      Serial.print("I2C Device found at address: 0x");
      Serial.println(i2cAddress, HEX);
    }
    else
    {
      Serial.print("I2C Device not found at address: 0x");
      Serial.println(i2cAddress, HEX);
    }
  }

  /*-------------- Miscellaneous Tasks ---------------*/
  Serial.println("Voltage (V),\tCurrent (A),\tPower (W),\tEnergy (KWH)");


  /*-------------- Setup Timer Events ---------------*/
  //ERa.addInterval(2000L, ERa_DHTReadEvent);
  ERa.addInterval(2000L, ERa_ACReadEvent);

  /* 
  RTOS Tasks initialization section
  */
  
  //xTaskCreate(DHT20_run, "Task DHT20 Temperature Humidity", 2048, NULL, 2, NULL);
  //xTaskCreate(powerMonitor, "Power Monitoring", 2048, NULL, 2, NULL);

  /* This printf is for debugging, 
    to see if the program is reset from the beginning
    when running RTOS tasks. If this line is printed, it means
    there is something wrong when we initialize RTOS tasks 
  */

  // Serial.print("\n The stepper's current position: ");
  // Serial.print(stepper1.currentPosition());

  // stepper1.setMaxSpeed(1000.0);
  // stepper1.setAcceleration(100.0);
  // stepper1.setSpeed(200);
  // stepper1.moveTo(endPoint);

  //vTaskDelay (2000 / portTICK_PERIOD_MS);

  // while (!mtDriver.begin(&Wire, HBRIDGE_I2C_ADDR, 11, 12, 100000L)) {
  //   Serial.println("\n HBridge Not Found!");
  //   delay(1000);
  // }
  // fw_version = mtDriver.getFirmwareVersion();
  // Serial.printf("\n HBridge Firmware Version: %d\r\n", fw_version);


  //setupIrSensor();

  //xTaskCreatePinnedToCore(runStepper, "Run Stepper Motor", 4096, NULL, 1, NULL, app_cpu);
  //xTaskCreatePinnedToCore(hBridgeDriverRun, "Run H-Bridge DC Motor Driver", 4096, NULL, 1, NULL, app_cpu);
  //xTaskCreatePinnedToCore(readFromIrSensor, "Read IR Obstacle Avoidance Sensor", 4096, NULL, 1, NULL, app_cpu);

  // if (stepper1.distanceToGo() == 0)
  // {
  //   Serial.println(stepper1.currentPosition());
  //   stepper1.setCurrentPosition(0);
  //   endPoint = -endPoint;
  //   stepper1.moveTo(endPoint);
  //   Serial.println(stepper1.currentPosition());
  // }
}

void loop()
{
  ERa.run();
}