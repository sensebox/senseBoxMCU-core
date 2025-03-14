#include "SolarChargerSB041.h"

#include "Arduino.h"

SolarChargerSB041::SolarChargerSB041(uint8_t address)
    : address(address),
      chargerConnected(false),
      solarPanelVoltage(-1),
      batteryVoltage(-1),
      charging(false),
      fastCharging(false),
      batteryLevel(-1),
      goodInputVoltage(false),
      batteryPresent(false),
      batteryTemperature(-1) {}

void SolarChargerSB041::begin() {
  Wire.begin();
  SolarChargerSB041::update();
}

/*
 * I2C i/f with following info on address 0x32:
 * - Register 0: cell voltage, 20mV/LSB
 * - Register 1: input voltage, 100mV/LSB
 * - Register 2: status bits: [B,I,L3,L2,L1,L0,F,C]
 *    B=battery present >2.8V
 *    I=Input voltage present >4.5V
 *    L0-L3=battery status LEDs
 *    F=Fast charge enabled
 *    C=Charging
 * - Register 3: temperature in C, signed 8-bit
 * Thresholds: L0: 3.2V, L1: 3.6V, L2: 3.7V, L3: 3.9V
 * Switch to slow charge at 8*C
 * Switch to fast charge at 12*C
 */
void SolarChargerSB041::update() {
  Wire.beginTransmission(address);
  byte error = Wire.endTransmission();
  if (error == 0) {
    chargerConnected = true;
    Wire.requestFrom((uint8_t)address, (uint8_t)4);

    // scale battery voltage
    uint8_t batteryVoltageRaw = Wire.read();
    batteryVoltage = 0.02 * batteryVoltageRaw;

    // scale panel voltage
    uint8_t vin_raw = Wire.read();
    solarPanelVoltage = 0.1 * vin_raw;

    // status flags
    uint8_t flags = Wire.read();
    charging = flags & 1;
    fastCharging = flags & 2;
    if (flags & 32)
      batteryLevel = 4;
    else if (flags & 16)
      batteryLevel = 3;
    else if (flags & 8)
      batteryLevel = 2;
    else if (flags & 4)
      batteryLevel = 1;
    else
      batteryLevel = 0;
    goodInputVoltage = flags & 64;
    batteryPresent = flags & 128;

    // battery temperature
    batteryTemperature = (int8_t)(Wire.read());
  } else {
    chargerConnected = false;  // or worse error
    batteryVoltage = -1;
    solarPanelVoltage = -1;
    charging = false;
    fastCharging = false;
    batteryLevel = -1;
    goodInputVoltage = false;
    batteryPresent = false;
    batteryTemperature = -1;
  }
}

bool SolarChargerSB041::isChargerConnected() const { return chargerConnected; }
float SolarChargerSB041::getSolarPanelVoltage() const {
  return solarPanelVoltage;
}
float SolarChargerSB041::getBatteryVoltage() const { return batteryVoltage; }
bool SolarChargerSB041::isCharging() const { return charging; }
bool SolarChargerSB041::isFastCharging() const { return fastCharging; }
int SolarChargerSB041::getBatteryLevel() const { return batteryLevel; }
bool SolarChargerSB041::isGoodInputVoltage() const { return goodInputVoltage; }
bool SolarChargerSB041::isBatteryPresent() const { return batteryPresent; }
float SolarChargerSB041::getBatteryTemperature() const {
  return batteryTemperature;
}
