#pragma once

//#define OTA_DEBUG
#define LOG SerialUSB

// these values must correspond  to the linker script flash_with_ota.ld
#define OTA_START 0x2000
#define OTA_SIZE 0x10000
#define APP_START_ADDRESS (OTA_START + OTA_SIZE)

#define FLASH_SIZE 0x40000UL
#define FLASH_PAGE_SIZE 512
