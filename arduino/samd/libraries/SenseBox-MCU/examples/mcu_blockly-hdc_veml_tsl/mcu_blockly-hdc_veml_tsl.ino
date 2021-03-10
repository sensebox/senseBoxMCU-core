#include "SenseBoxMCU.h";

HDC1080 hdc;
VEML6070 veml;
Lightsensor lightsensor; //TSL45315 and LTR329 are both supported

void setup()
{
  Serial.begin(9600);
  hdc.begin();
  veml.begin();
  lightsensor.begin();
  delay(100);
}

void loop()
{
  double temper = hdc.getTemperature();
  double humidi = hdc.getHumidity();
  double uvi = veml.getUvIntensity();
  long illumi = lightsensor.getIlluminance();
  Serial.print("Temperature: ");
  Serial.println(temper);
  Serial.print("Humidity: ");
  Serial.println(humidi);
  Serial.print("UV-Intensity: ");
  Serial.println(uvi);
  Serial.print("Illuminance: ");
  Serial.print(illumi);
  delay(100);
}
