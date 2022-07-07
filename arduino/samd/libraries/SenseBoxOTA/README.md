# senseBox OTA
This library provides over the air programming for the senseBox MCU.
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

## uploading a sketch
To upload a sketch, make sure...
- the MCU is in OTA mode & you are connected to its WiFi AP,
- your sketch contains the line `#include <SenseBoxOTA.h>`.

Export your compiled sketch (`ctrl+alt+s`). Now you can upload from your sketch directory:

```
curl 192.168.1.1/sketch --data-binary @<your-sketchname>ino.sensebox_mcu.bin
```

## development
This library has development dependencies on `WiFi101.h` and `FlashStorage.h` (during build time only).

To apply changes made to the `ota_boot.ino` sketch, the `./build.sh` script has to be run first.
For development you can enable DEBUG logging via the `OTA_DEBUG` define in `conf.h`; note that in debug mode the bootloader will wait until the Serial Monitor is opened before starting operation.

Internally, it works quite similar as Arduino's `SDU.h` library, except for swapping the SD reading functionality with a webserver.
To understand what is happening the following hints may help:

- The linker script reserves the first section of flash storage for the OTA functionality via the symbol name `.sketch_boot`.
  If this symbol is missing, the memory is as usual (8KB bootloader, then user code).
- The OTA functionality is defined in the code in `extras/ota_boot`, and put into the folder `src/boot/` in compiled binary form.
- The OTA bootloader directly hands over to a user application if one is present, otherwise starts a hotspot and webserver.

## known issues
- no checksumming or signature check on the received binary

## license
GPL-3.0, Norwin Roosen
