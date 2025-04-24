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
#define ERA_AUTH_TOKEN "756df301-d9d8-458a-a268-bc216db45ee1"

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
#include <freeRTOS/FreeRTOS.h>
#include <ERa.hpp>
#include <Widgets/ERaWidgets.hpp>

#include <ERa_ENV.h>

#define SDA 11
#define SCL 12

#if defined(BUTTON_PIN)
  #include <pthread.h>
  #include <ERa/ERaButton.hpp>
#endif

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
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

/* This function print uptime every second */
// void timerEvent()
// {
//   ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
// }

byte busStatus;
/*-------------- Initialize I2C hardware instances ---------------*/ 
TwoWire I2C_0 = TwoWire(0);
TwoWire I2C_1 = TwoWire(1);

ERaString estr;
ERaWidgetTerminalBox IrSensorTerminal(estr, V22, V23);

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

  /*-------------- Begin I2C interface ---------------*/
  I2C_0.begin(SDA, SCL, 100000U);

  /*-------------- Initialize Peripherals ---------------*/
  m5Env_init(I2C_0);

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

  /* Setup timer called function every second */
  ERa.addInterval(3000L, ERa_ENVReadEvent);
  //ERa.addInterval(1000L, timerEvent);

  //xTaskCreatePinnedToCore(runStepper, "Run Stepper Motor", 4096, NULL, 1, NULL, app_cpu);
  //xTaskCreatePinnedToCore(hBridgeDriverRun, "Run H-Bridge DC Motor Driver", 4096, NULL, 1, NULL, app_cpu);
  //xTaskCreatePinnedToCore(readFromIrSensor, "Read IR Obstacle Avoidance Sensor", 4096, NULL, 1, NULL, app_cpu);
}

void loop()
{
  ERa.run();
}