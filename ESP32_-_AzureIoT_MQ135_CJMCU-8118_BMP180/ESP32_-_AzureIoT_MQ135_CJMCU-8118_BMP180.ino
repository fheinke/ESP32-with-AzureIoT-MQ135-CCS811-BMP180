///////////////////////////////////////////////////////////////////////////////////////////////////
// 2019-09-19 FHeinke
//
// This Sketch reads Sensordata and send it to an Azure IoT Hub.
// The Onboard LED will blink every Time the ESP is measuring and sending the Data.
// Another RGB Led shows what Co2 Level is currently around.
//
// All functions are divided into different tabs in order to be able to work more precisely.
//
//
//  Used Librarys:
//  - ESP32MQTTClient
//  - MQ135
//  - Adafruit_CCS811_Library
//  - Adafruit_BMP085_Library
//
//
//  Pinout:
//  ESP32 DEVKIT V1 | MQ135   | CJMCU | BMP180  | RGBLed  | Comment
//  ----------------|---------|-------|---------|---------|--------
//  D4              |         |       |         | R       |
//  RX2             |         |       |         | G       |
//  TX2             |         |       |         | B       |
//  GND             |         |       |         | GND     |
//  D22             |         |       | SCL     |         |
//  D21             |         |       | SDA     |         |
//  GND             |         |       | GND     |         |
//  3V3             |         |       | 3.3     |         | 3.3 Volt
//  D22             |         | SCL   |         |         |
//  D21             |         | SDA   |         |         |
//  3V3             |         | VCC   |         |         | 3.3 Volt
//  GND             |         | GND   |         |         |
//  GND             |         | WAK   |         |         |
//  D34             | A0      |       |         |         |
//  VIN             | VCC     |       |         |         | 5.0 Volt
//  GND             | GND     |       |         |         |
//
//
// Data to Azure:
//  MQ135 - Value, Zero, PPM,
//  CJMCU - Temperature, Co2, TVOC
//  BMP180 - Pressure, Altitude, Temperature, Sealevel Altitude, Real Altitude
//  An Alert is send to Azure, every Time the Co2 Level changes between "OK", "Warning" and "Critical"
//
///////////////////////////////////////// Initializing /////////////////////////////////////////
#include <WiFi.h>
#include <Wire.h>
#include <Esp32MQTTClient.h>
#include <MQ135.h>
#include <Adafruit_CCS811.h>
#include <Adafruit_BMP085.h>

Adafruit_CCS811 ccs;
Adafruit_BMP085 bmp;



// You have to change the values of the variables
///////////////////////////////////////////////////////////////////////////////////////////////////
    // Wifi Network information
const char* wifi_ssid = "<SSID>";
const char* wifi_password = "<PASSWORD>";

    // String containing Hostname, Device Id & Device Key. Get it from your Azure IoT Hub.
static const char* connectionString = "HostName=<HOSTNAME>.azure-devices.net;DeviceId=<DEVICEID>;SharedAccessKey=<KEY>";
    
    // RGB Led for Co2 status
const int RedLedPin = 4; // D4
const int GreenLedPin = 16; // RX2
const int BlueLedPin = 17; // TX2

    // Time in milliseconds between each measurement (10000 ms = 10 seconds)
const unsigned long period = 10000; // milliseconds

    // MQ135 Datapin
const int mq135InPin = 34; // D34

    // Send Fake Sensor Data if you don't have one. (static Values)
static bool mq135_fakedata = false;
static bool cjmcu_fakedata = false;
static bool bmp180_fakedata = false;

    // you can get a more precise measurement of altitude if you know the current sea level pressure which will
    // vary with weather and such. If it is 1015 millibars that is equal to 101500 Pascals.
    // https://www.dwd.de/EN/ourservices/hobbymet_wcharts_europe/hobbyeuropecharts.html
int sealevelpressure = 102500;
///////////////////////////////////////////////////////////////////////////////////////////////////



int alertFlag = 3;                                                                                // The alert Flag is the last seen Status Flag
MQ135 gasSensor = MQ135(mq135InPin);
static bool hasIoTHub = false;                                                                    // If the ESP is connected to the IoT Hub, the value is true
unsigned long startMillis;
unsigned long currentMillis;



///////////////////////////////////////////////////////////////////////////////////////////////////
// Setup Loop
///////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);                                                                   // Initializes the onboard LED
  pinMode(RedLedPin, OUTPUT);                                                                     // Defines the Pin of the red light as an Output
  pinMode(GreenLedPin, OUTPUT);                                                                   // Defines the Pin of the green light as an Output
  pinMode(BlueLedPin, OUTPUT);                                                                    // Defines the Pin of the blue light as an Output

  Serial.begin(9600);                                                                             // Serial Bus Speed

  wifi_connect();                                                                                 // Calls the wifi_connect function

  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString))
  {
    hasIoTHub = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  hasIoTHub = true;

  if (!cjmcu_fakedata)
  {
    Serial.println("CCS811 Test");
    if (!ccs.begin())
    {
      Serial.println("Failed to start sensor! Please check your wiring.");
      sensor_error();
      while (1);
    }
    //calibrate temperature sensor
    while (!ccs.available());
    float temp = ccs.calculateTemperature();
    ccs.setTempOffset(temp - 25.0);
  }

  if (!bmp180_fakedata)
  {
    Serial.println("BMP180 Test");
    if (!bmp.begin())                                                                             // If no BMP is found, the Arduino will not continue
    {
      Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
      sensor_error();
      while (1) {}
    }
  }

  pinMode(mq135InPin, INPUT);                                                                    // Data Pin MQ135 Sensor

  delay(1000);
  startMillis = millis();
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// Main Loop
///////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  currentMillis = millis();                                                                       // get the current RunningTime
  if (currentMillis - startMillis >= period)                                                      // If Time to send Data is reached, the esp starts measuring
  {
    digitalWrite(LED_BUILTIN, HIGH);
    read_CjmcuMq135();
    read_Bmp180();
    digitalWrite(LED_BUILTIN, LOW);
    startMillis = currentMillis;
  }
}
