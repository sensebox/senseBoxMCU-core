#include "SenseBoxOTA.h"

// The .sketch_boot symbol is referenced in the linker script and
// pushed to the beginning of the sketch, thus running first.
__attribute__((section(".sketch_boot")))
// The symbol's length must be a multiple of 0x2000, due to linker script alignment.
// Changes to this length must be reflected in extra/ota_boot/OTA.h#OTA_SIZE too!
unsigned char ota_boot_ino_bin[0x10000] = {
  // contains the OTA bootloader binary, extracted from arduino .bin file via
  // extras/ota_boot/build.sh
  #include "boot/sensebox_mcu.h"
};
