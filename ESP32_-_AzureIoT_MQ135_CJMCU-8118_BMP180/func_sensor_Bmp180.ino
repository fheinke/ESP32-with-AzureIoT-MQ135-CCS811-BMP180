void read_Bmp180()
{
  String tempstring;
  String pressurestring;
  String altitudestring;
  String sealvlpressurestring;
  String realaltitudestring;



  if (bmp180_fakedata)
  {
    tempstring = "23.90";
    pressurestring = "101318.00";
    altitudestring = "0.08";
    sealvlpressurestring = "101328.00";
    realaltitudestring = "96.91";
  }
  else
  {
    // Read Data from Sensor
    float temp = bmp.readTemperature(); // in *C
    tempstring = String(temp);

    float pressure = bmp.readPressure(); // in Pa
    pressurestring = String(pressure);

    float altitude = bmp.readAltitude(); // in meters - Calculate altitude assuming 'standard' barometric pressure of 1013.25 millibar = 101325 Pascal
    altitudestring = String(altitude);

    float sealvlpressure = bmp.readSealevelPressure(); // in Pa
    sealvlpressurestring = String(sealvlpressure);

    float realaltitude = bmp.readAltitude(sealevelpressure); // in meters
    realaltitudestring = String(realaltitude);
  }



  if (hasIoTHub)
  {
    char buff[128];                                                               // Initialize Char Array

    // Add collected Data to Array
    String message = "{\"deviceID\":\"BMP180\",\"Temp\":\"" + tempstring + "\",\"Press\":\"" + pressurestring + "\",\"Alt\":\"" + altitudestring + "\",\"SeaLVLPress\":\"" + sealvlpressurestring + "\",\"RealAlt\":\"" + realaltitudestring + "\"}";
    int message_len = message.length() + 1;
    char message_array[message_len];
    message.toCharArray(message_array, message_len);

    snprintf(buff, 128, message_array);

    if (Esp32MQTTClient_SendEvent(buff))
    {
      Serial.println("");
      Serial.println("BMP180 Temperatur: " + tempstring);
      Serial.println("BMP180 Pressure: " + pressurestring);
      Serial.println("BMP180 Altitude: " + altitudestring);
      Serial.println("BMP180 Sealevel Pressure: " + sealvlpressurestring);
      Serial.println("BMP180 Real Altitude: " + realaltitudestring);
      Serial.println("Sending data succeed");
      Serial.println("");
    }
    else
    {
      Serial.println("Failure...");
      Serial.println("ESP restarting...");
      ESP.restart();
    }
  }
}
