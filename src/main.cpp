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
#define ERA_AUTH_TOKEN "5c166647-56cb-42c1-b903-d65c54190bc2"

/* Define setting button */
// #define BUTTON_PIN              0

#if defined(BUTTON_PIN)
// Active low (false), Active high (true)
#define BUTTON_INVERT false
#define BUTTON_HOLD_TIMEOUT 5000UL

// This directive is used to specify whether the configuration should be erased.
// If it's set to true, the configuration will be erased.
#define ERA_ERASE_CONFIG false
#endif

#include <Arduino.h>
#include <ERa.hpp>
#include <Widgets/ERaWidgets.hpp>

#include <stepperMotor.h>
#include <irSensor.h>

#if defined(BUTTON_PIN)
  #include <pthread.h>
  #include <ERa/ERaButton.hpp>
#endif

const char ssid[] = "Vector";
const char pass[] = "avfi2025";

WiFiClient mbTcpClient;

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

ERaString estr;
ERaWidgetTerminalBox terminal(estr, V22, V23);

/* This function print uptime every second */
void timerEvent()
{
  ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
}



void terminalCallBack() {
  if (estr == "Hi!") {
    terminal.print("Hello! ");
  }

  terminal.print("Thank you for using ERa");
  terminal.flush();
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
  terminal.begin(terminalCallBack);

  /* Initializing the ERa library. */
  ERa.begin(ssid, pass);

  /* Setup timer called function every second */
  ERa.addInterval(1000L, timerEvent);

  ERa.virtualWrite(V21, "Hello, ERa!");

  Serial.print("\n The stepper's current position: ");
  Serial.print(stepper1.currentPosition());
  delay(5000);
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(100.0);
  stepper1.setSpeed(200);
  stepper1.moveTo(endPoint);

  setupIrSensor();
}

void loop()
{
  ERa.run();
  stepper1.run();
  readFromIrSensor();

  // if (stepper1.distanceToGo() == 0)
  // {
  //   Serial.println(stepper1.currentPosition());
  //   stepper1.setCurrentPosition(0);
  //   endPoint = -endPoint;
  //   stepper1.moveTo(endPoint);
  //   Serial.println(stepper1.currentPosition());
  // }
}