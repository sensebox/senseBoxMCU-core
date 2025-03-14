/**
 * @brief Script to test this Arduino RG15 library with the senseBox.
 * @author Björn Luig (@BjoernLuig)
 * @note See official documentation of the RG15:
 * https://rainsensors.com/wp-content/uploads/sites/3/2020/07/rg-15_instructions_sw_1.000.pdf#page=2
 */

#include <RG15.h>

// if senseBox MCU uncomment below
// #include <senseBoxIO.h>
// #define SerialSensor Serial1 // UART1

// if senseBox MCU-S2 (ESP32) uncomment below
// #define SerialSensor Serial0 // UART = UART0

RG15 rg15(SerialSensor);  // create RG15 object

void setup() {
  // USB seriel
  Serial.begin(9600);
  while (!Serial);

  // start sensor
  Serial.println("Start RG15.");
  if (rg15.begin()) {
    Serial.println("RG15 initialized successfully!");
  } else {
    Serial.println("Failed to initialize RG15!");
  }
}

void loop() {
  // poll for rainfall data
  if (rg15.poll()) {
    Serial.print("Total Accumulated Rain: ");
    Serial.println(rg15.getTotalAccumulation());
  } else {
    Serial.println("Failed to read RG15 data.");
  }
  delay(2000);
}