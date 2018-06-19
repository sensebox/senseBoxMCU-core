#include "VEML6070.h"

VEML6070 veml;

void setup() {
  Serial.begin(9600);
  veml.begin();
  delay(500);
}

void loop() {
  float uvi;
  uvi = veml.getUV();  
  Serial.println(uvi);
  delay(1000);
}
