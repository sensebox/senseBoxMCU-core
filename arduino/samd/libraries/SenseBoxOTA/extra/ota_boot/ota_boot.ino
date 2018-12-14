#include "conf.h"
#include "OTA.h"

OTA ota_module;

bool validateFlashedApp() {
  /**
   * Test reset vector of application @APP_START_ADDRESS+4
   * Sanity check on the Reset_Handler address.
   * TODO: proper CRC check?
   */

  /* Load the Reset Handler address of the application */
  uint32_t app_reset_ptr = *(uint32_t *)(APP_START_ADDRESS + 4);

  return (app_reset_ptr >= APP_START_ADDRESS && app_reset_ptr <= FLASH_SIZE);
}

void setup() {
  pinMode(0, INPUT_PULLUP);

  #ifdef OTA_DEBUG
  LOG.begin(115200);
  while(!LOG) {;} // dont continue until serial was opened
  #endif

  // digitalRead(0) gives the value for the switch button (0 means its pressed)
  if (digitalRead(0) == LOW) {
    LOG.println("[OTA] switch pressed, entering OTA mode");
  } else if (validateFlashedApp()) {
    LOG.println("[OTA] jumping to application");
    ota_module.jumpToApp();
  } else {
    LOG.println("[OTA] no valid app installed, entering OTA mode");
  }

  // Setup WiFi access point
  delay(400); // otherwise the whole app hangs..
  ota_module.begin();
}

void loop() {
  // Function which listens for changes in the client
  ota_module.update();
}
