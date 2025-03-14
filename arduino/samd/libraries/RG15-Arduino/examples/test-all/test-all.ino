/**
 * @brief Script to test all features of this Arduino library with the senseBox.
 * @author Björn Luig (@BjoernLuig)
 * @note See official documentation of the RG15:
 * https://rainsensors.com/wp-content/uploads/sites/3/2020/07/rg-15_instructions_sw_1.000.pdf#page=2
 */

#include <RG15.h>

// if senseBox MCU uncomment below
// #include <senseBoxIO.h>
// #define SerialSensor Serial1 // UART1

// if senseBox MCU-S2 (ESP32) uncomment below
// #define SerialSensor Serial0  // UART = UART0

int interval = 1000;      // delay between tests
RG15 rg15(SerialSensor);  // create RG15 object

void setup() {
  // USB seriel
  Serial.begin(9600);
  while (!Serial);

  // start sensor
  Serial.println("STARTING RG15");
  Serial.print(rg15.begin() ? "success" : "fail");
  Serial.print(", errorCode = ");
  Serial.print(rg15.getErrorCode());
  Serial.print(", attempts = ");
  Serial.println(rg15.getAttempts());
  Serial.println();
}

void loop() {
  // poll 5 times
  for (int i = 0; i < 5; i++) {
    Serial.print("POLL ");
    Serial.println(i);
    Serial.print(rg15.poll() ? "success" : "fail");
    Serial.print(", errorCode = ");
    Serial.print(rg15.getErrorCode());
    Serial.print(", attempts = ");
    Serial.println(rg15.getAttempts());
    Serial.print("response = \"");
    Serial.print(rg15.getResponseBuffer());
    Serial.println("\"");
    Serial.print("accumulation = ");
    Serial.print(rg15.getAccumulation());
    Serial.print(", eventAccumulation = ");
    Serial.print(rg15.getEventAccumulation());
    Serial.print(" totalAccumulation = ");
    Serial.print(rg15.getTotalAccumulation());
    Serial.print(" rainfallIntensity = ");
    Serial.println(rg15.getRainfallIntensity());
    Serial.println();
    delay(interval);
  }

  // change to imperial
  Serial.println("CHANGE TO IMPERIAL");
  Serial.print(rg15.setUnit('i') ? "success" : "fail");
  Serial.print(", errorCode = ");
  Serial.print(rg15.getErrorCode());
  Serial.print(", attempts = ");
  Serial.println(rg15.getAttempts());
  Serial.print("response = \"");
  Serial.print(rg15.getResponseBuffer());
  Serial.println("\"");
  Serial.print("unit = ");
  Serial.println(rg15.getUnit());
  Serial.println();
  delay(interval);

  // poll
  Serial.println("POLL");
  Serial.print(rg15.poll() ? "success" : "fail");
  Serial.print(", errorCode = ");
  Serial.print(rg15.getErrorCode());
  Serial.print(", attempts = ");
  Serial.println(rg15.getAttempts());
  Serial.print("response = \"");
  Serial.print(rg15.getResponseBuffer());
  Serial.println("\"");
  Serial.print("accumulation = ");
  Serial.print(rg15.getAccumulation());
  Serial.print(", eventAccumulation = ");
  Serial.print(rg15.getEventAccumulation());
  Serial.print(" totalAccumulation = ");
  Serial.print(rg15.getTotalAccumulation());
  Serial.print(" rainfallIntensity = ");
  Serial.println(rg15.getRainfallIntensity());
  Serial.println();
  delay(interval);

  // change to metric
  Serial.println("CHANGE TO METRIC");
  Serial.print(rg15.setUnit('m') ? "success" : "fail");
  Serial.print(", errorCode = ");
  Serial.print(rg15.getErrorCode());
  Serial.print(", attempts = ");
  Serial.println(rg15.getAttempts());
  Serial.print("response = \"");
  Serial.print(rg15.getResponseBuffer());
  Serial.println("\"");
  Serial.print("unit = ");
  Serial.println(rg15.getUnit());
  Serial.println();
  delay(interval);

  // reset accumulation
  Serial.println("RESET ACCUMULATION");
  Serial.print(rg15.resetAccumulation() ? "success" : "fail");
  Serial.print(", errorCode = ");
  Serial.print(rg15.getErrorCode());
  Serial.print(", attempts = ");
  Serial.println(rg15.getAttempts());
  Serial.println();
  delay(interval);

  // poll
  Serial.println("POLL");
  Serial.print(rg15.poll() ? "success" : "fail");
  Serial.print(", errorCode = ");
  Serial.print(rg15.getErrorCode());
  Serial.print(", attempts = ");
  Serial.println(rg15.getAttempts());
  Serial.print("response = \"");
  Serial.print(rg15.getResponseBuffer());
  Serial.println("\"");
  Serial.print("accumulation = ");
  Serial.print(rg15.getAccumulation());
  Serial.print(", eventAccumulation = ");
  Serial.print(rg15.getEventAccumulation());
  Serial.print(" totalAccumulation = ");
  Serial.print(rg15.getTotalAccumulation());
  Serial.print(" rainfallIntensity = ");
  Serial.println(rg15.getRainfallIntensity());
  Serial.println();
  delay(interval);

  // // change baud rate to 1200 NOT RECOMMENDED
  // Serial.println("CHANGE BAUD RATE TO 1200");
  // Serial.print(rg15.changeBaudRate(1200) ? "success" : "fail");
  // Serial.print(", errorCode = ");
  // Serial.print(rg15.getErrorCode());
  // Serial.print(", attempts = ");
  // Serial.println(rg15.getAttempts());
  // Serial.print("response = \"");
  // Serial.print(rg15.getResponseBuffer());
  // Serial.println("\"");
  // Serial.println();
  // delay(interval);

  // // change baud rate to 57600 ONLY TEST IF BOARD SUPPORTS THIS
  // Serial.println("CHANGE BAUD RATE TO 57600");
  // Serial.print(rg15.changeBaudRate(57600) ? "success" : "fail");
  // Serial.print(", errorCode = ");
  // Serial.print(rg15.getErrorCode());
  // Serial.print(", attempts = ");
  // Serial.println(rg15.getAttempts());
  // Serial.print("response = \"");
  // Serial.print(rg15.getResponseBuffer());
  // Serial.println("\"");
  // Serial.println();
  // delay(interval);

  // // change baud rate back to 9600
  // Serial.println("CHANGE BAUD RATE BACK TO DEFAULT OF 9600");
  // Serial.print(rg15.changeBaudRate(9600) ? "success" : "fail");
  // Serial.print(", errorCode = ");
  // Serial.print(rg15.getErrorCode());
  // Serial.print(", attempts = ");
  // Serial.println(rg15.getAttempts());
  // Serial.print("response = \"");
  // Serial.print(rg15.getResponseBuffer());
  // Serial.println("\"");
  // Serial.println();
  // delay(interval);

  // restart sensor
  Serial.println("RESTART SENSOR");
  Serial.print(rg15.restart() ? "success" : "fail");
  Serial.print(", errorCode = ");
  Serial.print(rg15.getErrorCode());
  Serial.print(", attempts = ");
  Serial.println(rg15.getAttempts());
  Serial.println();
  delay(interval);
}