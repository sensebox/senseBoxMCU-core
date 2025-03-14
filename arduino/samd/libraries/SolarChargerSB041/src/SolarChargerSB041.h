/*
  SolarChargerSB041.h - Library to read data from the SB041 solar charger
  designed for the senseBox. Created by Björn Luig, Dec. 6, 2024. Released into
  the public domain.
*/

#ifndef SOLARCHARGERSB041_H
#define SOLARCHARGERSB041_H

#include <Wire.h>

#include "Arduino.h"

class SolarChargerSB041 {
 public:
  SolarChargerSB041(uint8_t address = 0x32);

  void begin();
  void update();
  bool isChargerConnected() const;
  float getSolarPanelVoltage() const;
  float getBatteryVoltage() const;
  bool isCharging() const;
  bool isFastCharging() const;
  int getBatteryLevel() const;
  bool isGoodInputVoltage() const;
  bool isBatteryPresent() const;
  float getBatteryTemperature() const;

 private:
  uint8_t address;
  bool chargerConnected;
  float solarPanelVoltage;
  float batteryVoltage;
  bool charging;
  bool fastCharging;
  int batteryLevel;
  bool goodInputVoltage;
  bool batteryPresent;
  float batteryTemperature;
};

#endif  // SOLARCHARGERSB041_H