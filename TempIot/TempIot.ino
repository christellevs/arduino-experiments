#include "DHT.h" 
#include "Arduino.h"

DHT dht(2, DHT11);

void setup()
{
  dht.begin();
  Serial.begin(115200);
  delay(7000);
  Serial.println("DHT11 Temperature and Humidity ");
}

void loop()
{
  delay(5000);
  float h = dht.readHumidity();
  delay(10);
  float t = dht.readTemperature();

  Serial.print('\n');
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(t); 
  delay(1000);
}