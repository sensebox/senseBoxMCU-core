# senseBox MCU
senseBox MCU is a Microchip SAMD21 based open-source board for sensors.

![senseBox MCU](https://github.com/watterott/senseBox-MCU/raw/master/hardware/senseBox-MCU_v12.jpg)

* [Microchip ATSAMD21](https://www.microchip.com/wwwproducts/en/ATSAMD21G18) ARM Cortex-M0+ Microcontroller
* [Microchip ATECC608A](https://www.microchip.com/wwwproducts/en/ATECC608A) Crypto Authentication
* [Bosch BMX055](https://www.bosch-sensortec.com/bst/products/all_products/bmx055) Orientation Sensor (Accelerometer + Gyroscope + Magnetometer)
* USB CDC+MSC Bootloader (Arduino compatible)
* XBee compatible sockets (with UART and SPI)
* 5V tolerant IOs, UART, I2C (JST-PH connectors)
* MicroUSB connector for power and data
* Idle power consumption about 20mA (single 5V power supply)
* Open-Source
* **[Documentation on learn.watterott.com](http://learn.watterott.com/sensebox/)**
* This repository is based on [Watterott's Core Package](https://github.com/watterott/senseBox-MCU)

## Library Versions

The senseBox MCU Core V.1.4.0 includes the following libraries:

| Name | Version | Link 
| -------- | -------- | -------- 
| senseBox MCU Lib | 1.4.1 | https://github.com/sensebox/SenseBoxMCU-Lib  
| Sparkfun SCD 30 Library | 1.0.12 | https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library 
| Adafruit GFX Library | 1.2.3 | https://github.com/adafruit/Adafruit-GFX-Library 
| Adafruit BME280 Library | 2.1.2 | https://github.com/adafruit/Adafruit_BME280_Library 
| Adafruit BMP280 Library | 2.1.0 | https://github.com/adafruit/Adafruit_BMP280_Library 
| Adafruit BME680 Library | 1.1.1 | https://github.com/adafruit/Adafruit_BME680 
| Adafruit_BMP3XX | 2.0.1 | https://github.com/adafruit/Adafruit_BMP3XX 
| Adafruit HDC1000 | 2.0.0 | https://github.com/adafruit/Adafruit_HDC1000_Library 
| Adafruit BusIO | 1.7.1 | https://github.com/adafruit/Adafruit_BusIO 
| Adafruit NeoPixel Library | 1.0.6 | https://github.com/adafruit/Adafruit_NeoPixel
| Adafruit_SSD1306 | 1.1.2 | https://github.com/adafruit/Adafruit_SSD1306 
| Adafruit Unified Sensor Driver | 1.0.2 | https://github.com/adafruit/Adafruit_Sensor 
| Arduino Library for Maxim Temperature | 3.8.0 | https://github.com/milesburton/Arduino-Temperature-Control-Library
| ArduinoBearSSL | 1.5.0 | https://github.com/arduino-libraries/ArduinoBearSSL 
| ArduinoECCX08 | 1.3.4 | https://github.com/arduino-libraries/ArduinoECCX08 
| Ethernet Library | 2.0.0 | http://www.arduino.cc/en/Reference/Ethernet
| FlashStorage library | 0.7.1 | https://github.com/cmaglie/FlashStorage
| HDC100X | 
| Arduino-LMIC library | 1.5.1 | https://github.com/matthijskooijman/arduino-lmic 
| LoRaMessage | 3.0.1 | https://github.com/thesolarnomad/lora-serialization 
| Makerblog_TSL45315 | 
| OneWire | 2.3.4 | http://www.pjrc.com/teensy/td_libs_OneWire.html
| RV8523 | 1.0.0 | https://github.com/watterott/Arduino-Libs/tree/master/RV8523 | 
| SDS011 library selectable | 0.0.6 | https://github.com/sensebox/SDS011-select-serial  
| SSD1306-Plot-Library | 1.0.0 | https://github.com/Lucas-Steinmann/SSD1306-Plot-Library
| SenseBoxOTA | 1.0.0 | https://github.com/sensebox/senseBoxMCU-core
| TinyGPSPlus | 1.0.0 | https://github.com/mikalhart/TinyGPSPlus
| VEML6070 | 0.0.1 | 
| LTR329 (based on LTR303 lib) | 0.1.0 | https://github.com/automote/LTR303
| WiFi101 | 0.16.1 | http://www.arduino.cc/en/Reference/WiFi101
| senseBox | 1.0.0 | 
| senseBoxIO | 1.0.0|


## Development & Preparing a new release

Please work in a separate branch and create a pull requests against the master branch!

Each pull request is validated through checks in CI by trying to install the current dev package and compiling against known sensor and model configurations.

### Development

- Create a branch from master
- Do you work, change, add or delete stuff.
- Remember to update the `arduino/samd.zip` file.
- After changing the `arduino/samd.zip` file, update the `size` and `checksum` properties in the `package_sensebox-dev_index.json`. (`stat -c "%s" arduino/samd.zip` and `shasum -a 256 arduino/samd.zip`)
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
