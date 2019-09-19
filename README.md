# ESP32 with AzureIoT, MQ135, CJMCU-8118 and BMP180
Sends sensor data of the sensors MQ135, CJMCU-8118 and BMP180 into the Azure Cloud to an IoT Hub.

The internal ESP Led flashes every time new data is measured and sent to the cloud. An external RGB LED indicates the range of the measured Co2 value. ("Ok", "Warning", "Critical")

All functions are divided into different tabs in order to be able to work more precisely.

### Used Librarys:
* ESP32MQTTClient
* MQ135
* Adafruit_CCS811_Library (for the CJMCU-8118)
* Adafruit_BMP085_Library (for the BMP180)

### Pinout:

ESP32 DEVKIT V1 | MQ135 | CJMCU | BMP180 | RGBLed | Comment
--------------- | ----- | ----- | ------ | ------ | -------
D4 (GPIO 4) | | | | R |
RX2 (GPIO 16) | | | | G |
TX2 | | | | B |
GND | | | | GND |
D22 | | | SCL | |
D21 | | | SDA | |
GND | | | GND | |
3V3 | | | 3.3 | | 3.3 Volt
D22 | | SCL | | |
D21 | | SDA | | |
3V3 | | VCC | | | 3.3 Volt
GND | | GND | | |
GND | | WAK | | |
D34 | A0 | | | |
VIN | VCC | | | | 5.0 Volt
GND | GND | | | |
