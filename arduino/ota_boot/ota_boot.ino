//#define DEBUG
#define LOG SerialUSB

// these values must correspond to the linker script flash_with_ota.ld
#define OTA_START 0x2000
#define OTA_SIZE 0x10000
#define APP_START_ADDRESS (OTA_START + OTA_SIZE)

#define FLASH_SIZE 0x40000UL

#include "senseBoxOTA.h"
SenseBoxOTA ota_module;

bool validateFlashedApp() {
  /**
   * Test reset vector of application @APP_START_ADDRESS+4
   * Sanity check on the Reset_Handler address.
   * TODO: proper CRC check?
   */

  /* Load the Reset Handler address of the application */
  uint32_t app_reset_ptr = *(uint32_t *)(APP_START_ADDRESS + 4);

  return (app_reset_ptr >= APP_START_ADDRESS || app_reset_ptr <= FLASH_SIZE);
}

void jumpToApp() {
  /* Load the Reset Handler address of the application */
  uint32_t app_reset_ptr = *(uint32_t *)(APP_START_ADDRESS + 4);

  #ifdef DEBUG
  LOG.print("app_reset_ptr: ");
  LOG.println(String(app_reset_ptr, HEX));

  LOG.print("stack pointer: ");
  LOG.println(String(*(uint32_t *)APP_START_ADDRESS, HEX));

  LOG.print("vector table address: ");
  LOG.println(String(((uint32_t)APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk), HEX));
  #endif

  /* Rebase the Stack Pointer */
  __set_MSP(*(uint32_t *)APP_START_ADDRESS);

  /* Rebase the vector table base address */
  SCB->VTOR = ((uint32_t)APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);

  /* Jump to application Reset Handler in the application */
  asm("bx %0" ::"r"(app_reset_ptr));
}

void setup() {
  pinMode(0, INPUT_PULLUP);

  #ifdef DEBUG
  LOG.begin(115200);
  while(!LOG) {;} // dont continue until serial was opened
  #endif

  // digitalRead(0) gives the value for the switch button (0 means its pressed)
  if (digitalRead(0) == LOW) {
    #ifdef DEBUG
    LOG.println("[OTA] switch pressed, entering OTA mode");
    #endif
  } else if (validateFlashedApp()) {
    #ifdef DEBUG
    LOG.println("[OTA] jumping to application");
    #endif
    jumpToApp();
  } else {
    #ifdef DEBUG
    LOG.println("[OTA] no valid app installed, entering OTA mode");
    #endif
  }

  // Setup WiFi access point
  ota_module.begin();
}

void loop() {
  // Function which listens for changes in the client
  ota_module.update();
}
