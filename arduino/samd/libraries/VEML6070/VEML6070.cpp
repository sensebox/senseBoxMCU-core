// VEML6070 library

#include "VEML6070.h"

#include "Wire.h"

#define UV_ADDR 0x38
#define IT_1 0x1

VEML6070::VEML6070() {}

void VEML6070::begin() {
  Wire.begin();
  Wire.beginTransmission(UV_ADDR);
  Wire.write((IT_1 << 2) | 0x02);
  Wire.endTransmission();
}

float VEML6070::getUV() {
  byte msb = 0, lsb = 0;
  uint16_t uvValue;
  Wire.requestFrom(UV_ADDR + 1, 1); // MSB
  delay(1);
  if (Wire.available()) {
    msb = Wire.read();
  }
  Wire.requestFrom(UV_ADDR + 0, 1); // LSB
  delay(1);
  if (Wire.available()) {
    lsb = Wire.read();
  }
  uvValue = (msb << 8) | lsb;
  return ((float)uvValue * 5.625);
}
