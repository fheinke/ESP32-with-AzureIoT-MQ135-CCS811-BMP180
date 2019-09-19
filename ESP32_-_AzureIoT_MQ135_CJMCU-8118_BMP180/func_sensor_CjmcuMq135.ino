void read_CjmcuMq135()
{
  String cjmcuTempString;
  float cjmcuCo2;
  String cjmcuCo2String;
  String cjmcuTVOCString;
  String mq135ValueString;
  String mq135ZeroString;
  String mq135PPMString;



  if (cjmcu_fakedata)
  {
    cjmcuTempString = "24.00";
    cjmcuCo2 = 1201;
    cjmcuCo2String = "1201";
    cjmcuTVOCString = "9.00";
  }
  else
  {
    if (ccs.available())
    {
      float cjmcuTemp = ccs.calculateTemperature();
      cjmcuTempString = String(cjmcuTemp);

      if (!ccs.readData())
      {
        float cjmcuCo2 = ccs.geteCO2();
        cjmcuCo2String = String(cjmcuCo2);

        float cjmcuTVOC = ccs.getTVOC();
        cjmcuTVOCString = String(cjmcuTVOC);
      }
      else
      {
        Serial.println("CJMCU - Fehler beim Auslesen der Sensordaten");
      }
    }
  }



  if (mq135_fakedata)
  {
    mq135ValueString = "146.00";
    mq135ZeroString = "526.10";
    mq135PPMString = "1.91";
  }
  else
  {
    // MQ135 Auslesen
    float mq135Value = analogRead(mq135InPin);
    mq135ValueString = String(mq135Value);

    float mq135Zero = gasSensor.getRZero();
    mq135ZeroString = String(mq135Zero);

    float mq135PPM = gasSensor.getPPM();
    mq135PPMString = String(mq135PPM);
  }



  // Auslesen CJMCU-8118
  if (hasIoTHub)
  {
    char buff[128];

    // Gesammelte Strings in ein Char Array schreiben, um es als eine Message an Azure zu senden
    String message = "{\"deviceID\":\"CJMCU\",\"Value\":\"" + mq135ValueString + "\",\"Zero\":\"" + mq135ZeroString + "\",\"PPM\":\"" + mq135PPMString + "\",\"Temp\":\"" + cjmcuTempString + "\",\"CO2\":\"" + cjmcuCo2String + "\",\"TVOC\":\"" + cjmcuTVOCString + "\"}";
    int message_len = message.length() + 1;
    char message_array[message_len];
    message.toCharArray(message_array, message_len);

    snprintf(buff, 128, message_array);

    // Senden dergesammelten Daten
    Serial.println("start sending events.");
    if (Esp32MQTTClient_SendEvent(buff))
    {
      Serial.println("");
      Serial.println("MQ135 Value: " + mq135ValueString);
      Serial.println("MQ135 Zero: " + mq135ZeroString);
      Serial.println("MQ135 PPM: " + mq135PPMString);
      Serial.println("CJMCU Temperatur: " + cjmcuTempString);
      Serial.println("CJMCU Co2: " + cjmcuCo2String);
      Serial.println("CJMCU TVOC: " + cjmcuTVOCString);
      Serial.println("Sending data succeed");
      Serial.println("");

      // CO2 Warn LED
      if (cjmcuCo2 <= 600)
      {
        rgbled(0,1,0);
        send_alert(0, "Alert_CJMCU", "Co2 OK");
      }
      else if (cjmcuCo2 > 1200)
      {
        rgbled(1,0,0);
        send_alert(2, "Alert_CJMCU", "Co2 Critical");
      }
      else
      {
        rgbled(1,1,0);
        send_alert(1, "Alert_CJMCU", "Co2 Warning");
      }
    }
    else
    {
      Serial.println("Failure...");
      Serial.println("ESP rebooting");
      ESP.restart();
    }
  }
}
