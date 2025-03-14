/**
 * @brief Script to test direct serial communication between the rain gauge
 * sensor RG15 and the senseBox.
 * @author Björn Luig (@BjoernLuig)
 * @note See official documentation of the RG15:
 * https://rainsensors.com/wp-content/uploads/sites/3/2020/07/rg-15_instructions_sw_1.000.pdf#page=2
 */

// if senseBox MCU uncomment below
// #include <senseBoxIO.h>
// #define SerialSensor Serial1 // UART1

// if senseBox MCU-S2 (ESP32) uncomment below
// #define SerialSensor Serial0 // UART = UART0

String data;                 // raw data
long sendTime, recieveTime;  // time tracking

void setup() {
  Serial.begin(9600);
  while (!Serial);
  SerialSensor.begin(9600);
  while (!SerialSensor);
}

void loop() {
  // send message
  while (Serial.available()) {
    sendTime = millis();
    data = Serial.readString();
    SerialSensor.println(data);
    Serial.print(sendTime);
    Serial.print(" send: ");
    Serial.print(data);
  }

  // recieve message
  while (SerialSensor.available()) {
    recieveTime = millis();
    data = SerialSensor.readString();
    Serial.print(recieveTime);
    Serial.print(" recieved: ");
    Serial.print(data);
  }
  delay(10);
}
