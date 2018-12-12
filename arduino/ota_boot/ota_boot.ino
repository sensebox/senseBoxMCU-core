#define DEBUG
#define LOG SerialUSB

#define APP_START_ADDRESS 0x12000 // must correspond to the linker script flash_with_ota.ld
//#define RESET_MAGIC_ADDRESS ((volatile uint32_t *)(APP_START_ADDRESS - 4)) // last 4 bytes of OTA bootloader (not writeable?)

// WARNING!!! do not set this to (HMCRAMC0_ADDR + HMCRAMC0_SIZE - 4) !!! otherwise you cant double tap into the original bootloader anymore!
#define RESET_MAGIC_ADDRESS ((volatile uint32_t *)(HMCRAMC0_ADDR + HMCRAMC0_SIZE - 16)) // last 16 bytes of RAM, should survive a reset

//#define RESET_MAGIC_ADDRESS ((volatile uint32_t *)(0x200004u)) // start of ram
#define RESET_MAGIC 67
#define RESET_TIMEOUT 3000 // milliseconds within which a second reset has to occur to stay in OTA mode. this counts additionally to the bootloader timeout of 500ms
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

  #ifdef DEBUG
  LOG.println(app_reset_ptr);
  #endif  

  if (app_reset_ptr < APP_START_ADDRESS || app_reset_ptr > FLASH_SIZE) {
    #ifdef DEBUG
    LOG.println("no valid app!");
    #endif
    return false;
  }
  
  return true;
}

bool checkDoubleTapReset() {
  // check if magic value is set.
  // this is the case, when a reset was performed, before the timeout has run out.

  // FIXME: not working, this adress is always reset to 8192... but why? for the original bootloader it works..
  if (*RESET_MAGIC_ADDRESS == RESET_MAGIC) {
    return true;
  }

  *RESET_MAGIC_ADDRESS = RESET_MAGIC;
  delay(RESET_TIMEOUT);
  *RESET_MAGIC_ADDRESS = 12;
  
  return false;
}

void jumpToApp() {
  /* Load the Reset Handler address of the application */
  uint32_t app_reset_ptr = *(uint32_t *)(APP_START_ADDRESS + 4);

  // FIXME: not working..

  /* Rebase the Stack Pointer */
  __set_MSP(*(uint32_t *)APP_START_ADDRESS);

  /* Rebase the vector table base address */
  SCB->VTOR = ((uint32_t)APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);

  /* Jump to application Reset Handler in the application */
  asm("bx %0" ::"r"(app_reset_ptr));
}

void setup() {
  pinMode(0,INPUT);
  #ifdef DEBUG
  LOG.begin(115200);
  while(!LOG) {;} // dont continue until serial was opened
  #endif

  LOG.println(*RESET_MAGIC_ADDRESS);


  if (validateFlashedApp()) {
    // digitalRead(0) gives the value for the switch button 
    // 0 indicates the button is pressed 
    // 1 indicitaes the button is not pressed
    if (!digitalRead(0))
    {
      jumpToApp();
    }
  }
  // Setup  WiFi access point 
  ota_module.begin();
  
}

void loop() {
  // Function which listens for changes in the client 
  ota_module.update();
  LOG.println("double tapped!");
  LOG.println(*RESET_MAGIC_ADDRESS);
  delay(100);
  
}
