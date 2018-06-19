#include "SenseBoxMCU.h";

HDC1080 hdc;
VEML6070 veml;
TSL45315 tsl;

void setup() {
  Serial.begin(9600);
  hdc.begin();
  veml.begin();
  tsl.begin();
  delay(100);
}

void loop() {
  double temper = hdc.getTemperature();
  double humidi = hdc.getHumidity();
  double uvi = veml.getUvIntensity();
  long illumi = tsl.getIlluminance();
  Serial.print("Temperature: "); Serial.println(temper);
  Serial.print("Humidity: "); Serial.println(humidi);
  Serial.print("UV-Intensity: "); Serial.println(uvi);
  Serial.print("Illuminance: "); Serial.print(illumi);
  delay(100);
}
