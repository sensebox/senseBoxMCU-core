# senseBox OTA
This library provides over the air programming for the senseBox MCU.

## usage
To enable this operating mode, just include the following line in your sketch:

```c
#include <SenseBoxOTA.h>
```

The WiFi bee must be installed in XBee slot 1.
Your MCU will now have a secondary OTA bootloader, which enables a secondary operation mode:

- If an user sketch is installed, the MCU will boot normally.
- If it's missing, the OTA mode is entered, indicated by the blinking red LED.
    In this mode a WiFi hotspot with a webserver is started, where sketch binaries can be sent to.
- This mode can be entered manually by holding down the grey button on the MCU ("switch") while starting / resetting.

### uploading a sketch
To upload a sketch, make sure...
- the MCU is in OTA mode & you are connected to its WiFi AP,
- your sketch contains the line `#include <SenseBoxOTA.h>`.

Export your compiled sketch (`ctrl+alt+s`). Now you can upload from your sketch directory:

```
curl 192.168.1.1/sketch --data-binary @<your-sketchname>ino.sensebox_mcu.bin
```

Other known-working clients:
- [senseBox Blockly app](https://github.com/sensebox/blockly-app)

## how it works

This library provides a second stage ('userspace') bootloader, that runs after the first stage bootloader (which provides the USB mass-storage update facility), but runs before the user-provided code.
This works by inserting the OTA functionality at the start of the userspace.
This position in flash storage is defined for the symbol name `.sketch_boot` in the linker script (`variants/sensebox_mcu/linker_scripts/gcc/flash_with_bootloader.ld`).
The symbol `.sketch_boot` is defined in `src/SenseboxOTA.cpp` and contains the compiled sketch of the bootloader (see section [development].

Internally, this bootloader works quite similar as Arduino's [`SDU` library][sdu], except for swapping the SD reading functionality with a webserver:
The OTA bootloader directly hands over to a user application if one is present, otherwise starts a WiFi accesspoint and webserver.
On this WiFi accesspoint clients can send new sketches to the MCU via HTTP POST to http://192.168.1.1:80/sketch.

```
+---------------------------+
|   8KB  primary bootloader |
+---------------------------+
|  64KB  OTA bootloader     | // this section only exists, when .sketch_boot is defined,
|        .sketch_boot       | // i.e. when the user sketch contains `#include <SenseBoxOTA.h>` 
+---------------------------+
| 184KB  userspace code     |
|                           |
+---------------------------+
```

## development
- directory layout:
  - `examples/`: simple usage example
  - `extra/ota_boot/`: the bootloader
  - `src/` contains the runtime with the compiled bootloader, included by users via `#include <SenseBoxOTA.h>`

- To apply changes made to the `ota_boot.ino` sketch, the OTA bootloader needs to be built:
    Run `./build_cli.sh` to update the bootloader that users include via `#include <SenseBoxOTA.h>`.

- This library has development dependencies on [`WiFi101.h` 0.16.*][libwifi] and [`FlashStorage.h` v1.0.0][libflash] (during build time only).

- The recommended build tool is [arduino-cli][cli], but Arduino IDE may work too, use the respective `build_*.sh` script.

- For development you can enable DEBUG logging via the `OTA_DEBUG` define in `OTA.h`;
    note that in debug mode the bootloader will wait until the Serial Monitor is opened before starting operation!

## known issues
- no checksumming or signature check on the received binary
- OTA bootloader takes up almost 64KB of flash - most of it is the Wifi101 library.
    If this can be replaced  with a slimmer library, more space for user sketches will remain.
- OTA_DEBUG logging is silent after ~2 seconds without communication - cause unknown.
    Output is re-enabled by sending a message from the host.

## license
GPL-3.0, Norwin Roosen

[sdu]: https://github.com/arduino/ArduinoCore-samd/tree/master/libraries/SDU
[cli]: https://github.com/arduino/arduino-cli
[libwifi]: https://github.com/arduino-libraries/WiFi101
[libflash]: https://github.com/cmaglie/FlashStorage
