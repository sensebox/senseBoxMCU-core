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
