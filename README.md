# senseBox MCU

senseBox MCU is a Microchip SAMD21 based open-source board for sensors.

![senseBox MCU](https://github.com/watterott/senseBox-MCU/raw/master/hardware/senseBox-MCU_v12.jpg)

- [Microchip ATSAMD21](https://www.microchip.com/wwwproducts/en/ATSAMD21G18) ARM Cortex-M0+ Microcontroller
- [Microchip ATECC608A](https://www.microchip.com/wwwproducts/en/ATECC608A) Crypto Authentication
- [Bosch BMX055](https://www.bosch-sensortec.com/bst/products/all_products/bmx055) Orientation Sensor (Accelerometer + Gyroscope + Magnetometer)
- USB CDC+MSC Bootloader (Arduino compatible)
- XBee compatible sockets (with UART and SPI)
- 5V tolerant IOs, UART, I2C (JST-PH connectors)
- MicroUSB connector for power and data
- Idle power consumption about 20mA (single 5V power supply)
- Open-Source
- **[Documentation on learn.watterott.com](http://learn.watterott.com/sensebox/)**
- This repository is based on [Watterott's Core Package](https://github.com/watterott/senseBox-MCU)

## Library Versions

The senseBox MCU Core V.1.6.0 includes the following libraries:

- [Arduino WiFi101 (WINC1500)](https://github.com/arduino-libraries/WiFi101)
- [SSD1306_PLOT_LIBRARY](https://github.com/sensebox/SSD1306-Plot-Library/archive/refs/tags/v1.0.0.zip)
- [SDS011_LIBRARY](https://github.com/sensebox/SDS011-select-serial/archive/refs/heads/master.zip)
- [RTC_LIBRARY](https://github.com/sensebox/RV8523-RTC-Arduino-Library/archive/refs/heads/main.zip)
- [BMX055_LIBRARY](https://github.com/sensebox/BMX055-Arduino-Library/archive/refs/heads/main.zip)
- [LTR329](https://github.com/sensebox/LTR329-Lightsensor-Arduino-Library/archive/refs/heads/main.zip)
- [VEML6070](https://github.com/sensebox/VEML6070-UV-Arduino-Library/archive/refs/heads/main.zip)
- [SENSEBOX_LIBWEB](https://github.com/sensebox/sensebox-libweb/archive/refs/heads/master.zip)

- [Ethernet](https://github.com/arduino-libraries/Ethernet)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [Adafruit HDC1000 Library](https://github.com/adafruit/Adafruit_HDC1000_Library)
- [Adafruit BME280 Library](https://github.com/adafruit/Adafruit_BME280_Library)
- [Adafruit BMP280 Library](https://github.com/adafruit/Adafruit_BMP280_Library)
- [Adafruit BME680 Library](https://github.com/adafruit/Adafruit_BME680)
- [Adafruit DPS310](https://github.com/adafruit/Adafruit_DPS310)
- [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit MQTT Library](https://github.com/adafruit/Adafruit_MQTT_Library)
- [Adafruit BusIO](https://github.com/adafruit/Adafruit_BusIO)
- [Adafruit SleepyDog Library](https://github.com/adafruit/Adafruit_SleepyDog)
- [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library)
- [ArduinoBearSSL](https://github.com/arduino-libraries/ArduinoBearSSL)
- [ArduinoECCX08](https://github.com/arduino-libraries/ArduinoECCX08)
- [SparkFun SCD30 Arduino Library](https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library)
- [SparkFun u-blox GNSS Arduino Library](https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library)
- [NewPing](https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home)
- [IBM LMIC framework](https://github.com/matthijskooijman/arduino-lmic)
- [LoRa Serialization](https://github.com/thesolarnomad/lora-serialization)
- [CayenneLPP](https://github.com/ElectronicCats/CayenneLPP)
- [OneWire](https://github.com/PaulStoffregen/OneWire)
- [Nova Fitness Sds dust sensors library](https://github.com/lewapek/sds-dust-sensors-arduino-library)
- [JC_Button](https://github.com/JChristensen/JC_Button)
- [SD](https://github.com/arduino-libraries/SD)
- [BSEC Software Library](https://github.com/BoschSensortec/BSEC-Arduino-library)
- [TheThingsNetwork](https://github.com/TheThingsNetwork/arduino-device-lib)
- [NTPClient](https://github.com/arduino-libraries/NTPClient)
- [phyphox BLE](https://github.com/phyphox/phyphox-arduino)

## Development & Preparing a new release

Please work in a separate branch and create a pull requests against the master branch!

Each pull request is validated through checks in CI by trying to install the current dev package and compiling against known sensor and model configurations.

### Development

- Create a branch from master
- Do you work, change, add or delete stuff.
- Remember to update the `arduino/samd.zip` file. `zip -r -X samd.zip samd `
- After changing the `arduino/samd.zip` file, update the `size` and `checksum` properties in the `package_sensebox-dev_index.json`. (`stat -c "%s" arduino/samd.zip` or `stat -f "%z" samd.zip` and `shasum -a 256 arduino/samd.zip`)
- Commit & push
- If not already open, create a pull request

### Preparing a new release

Once you're satisfied with the current state of the master branch:

- [Create a new release](https://github.com/sensebox/senseBoxMCU-core/releases/new)
  - Attach the current `arduino/samd.zip`
- Create a new branch
  - Update the `package_sensebox_index.json`
  - Prepend a new entry in the `packages.platforms` array
  - Use the version and url from your release above
  - Commit & push
- Create a PR and merge into master
- If everything was okay, no errors in CI should emerge
