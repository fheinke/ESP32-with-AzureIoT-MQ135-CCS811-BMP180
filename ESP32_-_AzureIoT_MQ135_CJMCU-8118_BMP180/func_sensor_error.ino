void sensor_error()
{
  while (true)
  {
    digitalWrite(RedLedPin, HIGH);
    delay(500);
    digitalWrite(RedLedPin, LOW);
    delay(500);
  }
}
