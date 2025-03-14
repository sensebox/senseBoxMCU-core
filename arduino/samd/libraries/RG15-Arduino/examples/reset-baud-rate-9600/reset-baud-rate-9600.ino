/**
 * @brief Script to reset the baud rate of the rain gauge sensor to 9600.
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

// valid baud rates indexed by baud codes (0-6)
const unsigned int baudRates[7] = {1200, 2400, 4800, 9600, 19200, 38400, 57600};

// create RG15 object
RG15 rg15(SerialSensor);

void setup() {
  // USB seriel
  Serial.begin(9600);
  while (!Serial);

  // test baud rates and change back to default of 9600
  for (int baudCode = 0; baudCode < 7; baudCode++) {
    Serial.print("TRY CHANGING BAUD RATE FROM ");
    Serial.print(baudRates[baudCode]);
    Serial.println(" TO 9600");
    rg15.begin(baudRates[baudCode], true, 'm');
    if (rg15.changeBaudRate(9600)) {
      Serial.println("success");
      break;
    } else {
      Serial.print("fail, error = ");
      Serial.println(rg15.getErrorCode());
    }
    Serial.println();
  }
}

void loop() {}