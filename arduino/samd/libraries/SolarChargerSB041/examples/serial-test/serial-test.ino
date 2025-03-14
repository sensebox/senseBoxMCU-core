#include <SolarChargerSB041.h>
#include <senseBoxIO.h> // only needer fo senseBox MCU

SolarChargerSB041 charger;

void setup() {
  charger.begin();
  Serial.begin(9600);
}

void loop() {
  charger.update();

  Serial.print("Charger Connected: ");
  Serial.println(charger.isChargerConnected() ? "true" : "false");
  Serial.print("Solar Panel Voltage: ");
  Serial.println(charger.getSolarPanelVoltage());
  Serial.print("Battery Voltage: ");
  Serial.println(charger.getBatteryVoltage());
  Serial.print("Charging: ");
  Serial.println(charger.isCharging() ? "true" : "false");
  Serial.print("Fast Charging: ");
  Serial.println(charger.isFastCharging() ? "true" : "false");
  Serial.print("Battery Level: ");
  Serial.println(charger.getBatteryLevel());
  Serial.print("Good Input Voltage: ");
  Serial.println(charger.isGoodInputVoltage() ? "true" : "false");
  Serial.print("Battery Present: ");
  Serial.println(charger.isBatteryPresent() ? "true" : "false");
  Serial.print("Battery Temperature: ");
  Serial.println(charger.getBatteryTemperature());
  Serial.println();

  delay(1000);
}
