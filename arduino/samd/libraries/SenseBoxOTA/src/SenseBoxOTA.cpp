#include "SenseBoxOTA.h"

// the .ota_boot symbol is referenced in the linker script and
// pushed to the beginning of the sketch, thus running first.
// The symbols length must be a multiple of 0x2000!

__attribute__((section(".sketch_boot")))
unsigned char ota_boot_ino_bin[0x10000] = {
  // contains the OTA bootloader binary, extracted from arduino .bin file via
  // extras/ota_boot/build.sh
  #include "boot/sensebox_mcu.h"
};
