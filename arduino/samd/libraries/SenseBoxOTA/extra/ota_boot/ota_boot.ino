#include "conf.h"
#include "OTA.h"

OTA ota_module;

bool validateFlashedApp() {
  /**
   * Test reset vector of application @APP_START_ADDRESS+4
   * Sanity check on the Reset_Handler address.
   * TODO FIXME: proper CRC / signature check?
   */

  /* Load the Reset Handler address of the application */
  uint32_t app_reset_ptr = *(uint32_t *)(APP_START_ADDRESS + 4);

  return (app_reset_ptr >= APP_START_ADDRESS && app_reset_ptr <= FLASH_SIZE);
}

void jumpToApp() {
  /* Load the Reset Handler address of the application */
  uint32_t app_reset_ptr = *(uint32_t *)(APP_START_ADDRESS + 4);

  LOG.print("app_reset_ptr: ");
  LOG.println(String(app_reset_ptr, HEX));

  LOG.print("stack pointer: ");
  LOG.println(String(*(uint32_t *)APP_START_ADDRESS, HEX));

  LOG.print("vector table address: ");
  LOG.println(String(((uint32_t)APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk), HEX));

  /* Rebase the Stack Pointer */
  __set_MSP(*(uint32_t *)APP_START_ADDRESS);

  /* Rebase the vector table base address */
  SCB->VTOR = ((uint32_t)APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);

  /* Jump to application Reset Handler in the application */
  asm("bx %0" ::"r"(app_reset_ptr));
}


void setup() {
  pinMode(0, INPUT_PULLUP); // switch button

  #ifdef OTA_DEBUG
  LOG.begin(115200);
  while(!LOG) {;} // dont continue until serial was opened
  #endif

  // digitalRead(0) gives the value for the switch button (0 means its pressed)
  if (digitalRead(0) == LOW) {
    LOG.println("[OTA] switch pressed, entering OTA mode");
  } else if (validateFlashedApp()) {
    LOG.println("[OTA] jumping to application");
    jumpToApp();
  } else {
    LOG.println("[OTA] no valid app installed, entering OTA mode");
  }

  // Setup WiFi access point
  delay(400); // otherwise the whole app hangs..
  ota_module.begin(true);
}

void loop() {
  ota_module.update(); // poll for updates in the OTA HTTP server
}
