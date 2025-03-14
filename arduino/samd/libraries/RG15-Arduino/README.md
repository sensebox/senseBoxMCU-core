# Overview

The **RG15 Arduino Library (RG15-Arduino)** provides robust communication with the [**RG15 Rain Gauge Sensor**](https://rainsensors.com/products/rg-15/) over a serial interface.

It was designed for use with [senseBox microcontrollers](https://sensebox.de/) and fixes problems with the [standard library](https://rainsensors.com/docs/rg-guides/rg-arduino/hydreon-arduino-library/) provided by hydreon and our [old version](https://github.com/sensebox/hydreon-rainsensor-library). This library is designed for accurate and reliable retrieval of rainfall data, supporting polling mode and essential sensor functionalities.

# Features

- Retrieves real-time rainfall data, including:
  - Accumulated rainfall since the last poll.
  - Accumulated rainfall for the current event.
  - Total accumulated rainfall since last reset.
  - Rainfall intensity.
- Supports metric (mm) and imperial (inches) units.
- Allows change of valid baud rates (1200 to 57600).
- Allows sensor reinitialization and polling mode.
- Provides error handling and debugging capabilities.
- Compatible with **Arduino-based microcontrollers**, including the [**senseBox**](https://sensebox.de/).

# Installation

Automatic installation is possible via the interface of the Arduino IDE. Manual Download works as follows:

1. Download or clone the repository.
2. Copy the **rg15-arduino-lib** folder into your Arduino **libraries** directory:
   ```
   Documents/Arduino/libraries/
   ```
3. Restart the Arduino IDE and include the library:
   ```cpp
   #include <RG15.h>
   ```

# Wiring Guide

The RG15 sensor can be directly connected to sensebox microcontrollers via the sensebox adapter board. JST for senseBox MCU (5V) and Qwicc for senseBox MCU-S2 (3.3V). Othetwise manual connaction works as follows:

| RG15 Pin on J2                  | Arduino Pin (Example) |
| ------------------------------- | --------------------- |
| VCC (Pin 2 for 5V+, 8 for 3.3V) | 5V or 3.3V            |
| GND (Pin 1)                     | GND                   |
| OUT (Pin 3)                     | RX (e.g., Serial1 RX) |
| IN (Pin 5)                      | TX (e.g., Serial1 TX) |

# Usage and Examples

Below is a simple Example. Make sure to use the right serial port by uncommenting or adding the one you connected the sensor to.

```cpp
#include <RG15.h>

// if senseBox MCU uncomment below
// #include <senseBoxIO.h>
// #define SerialSensor Serial1 // UART1

// if senseBox MCU-S2 (ESP32) uncomment below
// #define SerialSensor Serial0 // UART = UART0

RG15 rg15(SerialSerial); // create RG15 object

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
```

This and more examples can be found in the example folder:

- `simple-example/simple-example.ino`
- `test-all/test-all.ino`
- `reset-baud-rate-9600/reset-baud-rate-9600.ino`
- `direct-serial/direct-serial.ino` (for debugging)

# Available Methods

| Method                                                                                                            | Description                                                                                                                                   | Parameters                                                                                                                                                                                                                                                                                                                                                                                                       | Return Type                                                                                                                                                                                                                                                                       |
| ----------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `RG15(HardwareSerial& serial, unsigned int cleanTimeout, unsigned int responseTimeout, unsigned int maxAttempts)` | Construct a new RG15 sensor instance.                                                                                                         | `serial`: Reference to the serial port to use (e.g., Serial1 or Serial2 on the senseBox MCU and Serial0 on the senseBoc MCU-S2). `cleanTimeout`: Timeout for cleaning the serial communication stream in ms (should be at least 500 ms). `responseTimeout`: Timeout for response in sensor communication in ms (should be at least 1000 ms). `maxAttempts`: Maximum number of attempts for sensor communication. | None                                                                                                                                                                                                                                                                              |
| `RG15(HardwareSerial& serial)`                                                                                    | Construct a new RG15 sensor instance with default communication settings (cleanTimeout = 500 ms, responseTimeout = 1000 ms, maxAttempts = 5). | `serial`: Reference to the serial port to use (e.g., Serial1 or Serial2 on the senseBox).                                                                                                                                                                                                                                                                                                                        | None                                                                                                                                                                                                                                                                              |
| `bool begin(int baudRate, bool highResolution, char unit)`                                                        | Initialize the sensor and communication.                                                                                                      | `baudRate`: The baud rate for serial sensor communication. `highResolution`: True for high resolution, false for low resolution. `unit`: 'm' for metric, 'i' for imperial units.                                                                                                                                                                                                                                 | `true` if the sensor was initialized successfully, `false` otherwise.                                                                                                                                                                                                             |
| `bool begin()`                                                                                                    | Initialize the sensor and communication with default parameters: 9600 baud rate, high resolution, and metric units.                           | None                                                                                                                                                                                                                                                                                                                                                                                                             | `true` if the sensor was initialized successfully, `false` otherwise.                                                                                                                                                                                                             |
| `bool poll()`                                                                                                     | Poll all rain gauge data in polling mode and store it in the sensor object.                                                                   | None                                                                                                                                                                                                                                                                                                                                                                                                             | `true` if the data was successfully retrieved, `false` otherwise.                                                                                                                                                                                                                 |
| `bool restart()`                                                                                                  | Restart the sensor.                                                                                                                           | None                                                                                                                                                                                                                                                                                                                                                                                                             | `true` if the sensor was restarted successfully, `false` otherwise.                                                                                                                                                                                                               |
| `bool changeBaudRate(unsigned int baudRate)`                                                                      | Change the sensor's baud rate and reestablish communication. It is advised to not use this and keep the baud rate at 9600.                    | `baudRate`: The baud rate to change to. Only 1200, 2400, 4800, 9600, 19200, 38400, and 57600 are valid.                                                                                                                                                                                                                                                                                                          | `true` if the baud rate was changed correctly, `false` otherwise.                                                                                                                                                                                                                 |
| `bool setPollingMode()`                                                                                           | Set the sensor to polling mode (the only mode supported by this library). It can also be used as a ping test.                                 | None                                                                                                                                                                                                                                                                                                                                                                                                             | `true` if polling mode was set and acknowledged by the sensor, `false` otherwise.                                                                                                                                                                                                 |
| `bool setHighResolution()`                                                                                        | Enable high resolution mode.                                                                                                                  | None                                                                                                                                                                                                                                                                                                                                                                                                             | `true` if high resolution was set and acknowledged by the sensor, `false` otherwise.                                                                                                                                                                                              |
| `bool setLowResolution()`                                                                                         | Enable low resolution mode.                                                                                                                   | None                                                                                                                                                                                                                                                                                                                                                                                                             | `true` if low resolution was set and acknowledged by the sensor, `false` otherwise.                                                                                                                                                                                               |
| `bool setUnit(char unit)`                                                                                         | Set the sensor's unit to metric or imperial.                                                                                                  | `unit`: 'm' for metric, 'i' for imperial.                                                                                                                                                                                                                                                                                                                                                                        | `true` if the unit was set and acknowledged by the sensor, `false` otherwise.                                                                                                                                                                                                     |
| `char getUnit()`                                                                                                  | Get the sensor's unit.                                                                                                                        | None                                                                                                                                                                                                                                                                                                                                                                                                             | `'m'` for metric, `'i'` for imperial.                                                                                                                                                                                                                                             |
| `bool resetAccumulation()`                                                                                        | Reset the accumulated rainfall. This is only temporary until the next restart of the sensor.                                                  | None                                                                                                                                                                                                                                                                                                                                                                                                             | `true` if the reset was successful, `false` otherwise.                                                                                                                                                                                                                            |
| `float getAccumulation()`                                                                                         | Get the accumulated rainfall since the last poll.                                                                                             | None                                                                                                                                                                                                                                                                                                                                                                                                             | `float`: The accumulated rainfall since the last poll.                                                                                                                                                                                                                            |
| `float getEventAccumulation()`                                                                                    | Get the accumulated rainfall in the current event.                                                                                            | None                                                                                                                                                                                                                                                                                                                                                                                                             | `float`: The accumulated rainfall in the current event.                                                                                                                                                                                                                           |
| `float getTotalAccumulation()`                                                                                    | Get the total accumulated rainfall since the last reset.                                                                                      | None                                                                                                                                                                                                                                                                                                                                                                                                             | `float`: The total accumulated rainfall since the last reset.                                                                                                                                                                                                                     |
| `float getRainfallIntensity()`                                                                                    | Get the current rainfall intensity.                                                                                                           | None                                                                                                                                                                                                                                                                                                                                                                                                             | `float`: The current rainfall intensity.                                                                                                                                                                                                                                          |
| `int getErrorCode()`                                                                                              | Get the latest error code for debugging purposes.                                                                                             | None                                                                                                                                                                                                                                                                                                                                                                                                             | `0` if no error occurred, `1` if the serial connection does not exist, `2` if the serial connection could not write, `3` if the response is invalid, `4` if the response timed out, `5` if the baud rate is not supported, `6` if parsing failed, `7` if the unit does not match. |
| `int getAttempts()`                                                                                               | Get the number of attempted communications.                                                                                                   | None                                                                                                                                                                                                                                                                                                                                                                                                             | `int`: The number of attempts.                                                                                                                                                                                                                                                    |
| `char* getResponseBuffer()`                                                                                       | Get the response buffer for debugging purposes.                                                                                               | None                                                                                                                                                                                                                                                                                                                                                                                                             | `char*`: The response buffer as a C-string.                                                                                                                                                                                                                                       |

# Error Codes

| Code | Meaning                   |
| ---- | ------------------------- |
| 0    | No error                  |
| 1    | Serial does not exist     |
| 2    | Serial could not write    |
| 3    | Invalid response received |
| 4    | Response timeout          |
| 5    | Unsupported baud rate     |
| 6    | Data parsing failed       |
| 7    | Unit mismatch             |

# Notes:

- The library currently only supports polling mode. No continuous mode.
- Hardware switches and tipping bucket features are not yet implemented.

# References

- **RG15 Sensor Documentation:** [RG15 Manual](https://rainsensors.com/wp-content/uploads/sites/3/2020/07/rg-15_instructions_sw_1.000.pdf#page=2)

# License

This library is open-source and licensed under the **MIT License**.

# Authors

- **Paul Reichmuth** (@PaulReichmuth)
- **Björn Luig** (@BjoernLuig)
- **Inspiration from:** Jan-Patrick Bollow (@JBollow)
