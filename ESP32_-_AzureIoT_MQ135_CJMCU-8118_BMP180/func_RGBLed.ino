void rgbled(int r, int g, int b)
{
      // Red LED
  if (r == 1)
  {
    digitalWrite(RedLedPin, HIGH);
  }
  else if (r == 0)
  {
    digitalWrite(RedLedPin, LOW);
  }

      // Green LED
  if (g == 1)
  {
    digitalWrite(GreenLedPin, HIGH);
  }
  else if (g == 0)
  {
    digitalWrite(GreenLedPin, LOW);
  }

      // Blue LED
  if (b == 1)
  {
    digitalWrite(BlueLedPin, HIGH);
  }
  else if (b == 0)
  {
    digitalWrite(BlueLedPin, LOW);
  }
}
