void send_alert(int flag, String deviceID, String alertText)
{
  if (alertFlag != flag)
  {
    alertFlag = flag;
    if (hasIoTHub)
    {
      char buff[128];                                                               // Initialize Char Array

      // Add collected Data to Array
      String message = "{\"deviceID\":\"" + deviceID + "\",\"Flag\":\"" + flag + "\",\"Message\":\"" + alertText + "\"}";
      int message_len = message.length() + 1;
      char message_array[message_len];
      message.toCharArray(message_array, message_len);

      snprintf(buff, 128, message_array);

      if (Esp32MQTTClient_SendEvent(buff))
      {
        Serial.println("");
        Serial.println("Sending Alert data succeed");
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
}
