#include <SPI.h>
#include <Arduino.h>
#include <FlashStorage.h>

#include "conf.h"
#include "OTA.h"

OTA::OTA(): server(80), status(WL_IDLE_STATUS) {}

void OTA::begin()
{
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  // Assign mac address to byte array & push it to a char array
  WiFi.macAddress(mac);
  String mac_str = String(mac[1], HEX) + String(mac[0], HEX);
  mac_str.toUpperCase();
  String ssid_string = String("senseBox:" + mac_str);
  char ssid[20];
  ssid_string.toCharArray(ssid,20);

  LOG.print("Creating access point named: ");
  LOG.println(ssid);

  // initialize wifi: set SSID based on last 4 bytes of MAC address
  status = WiFi.beginAP(ssid);
  if (status != WL_AP_LISTENING) {
    LOG.println("Creating access point failed");
    while (true);
  }

  pinMode(LED_BUILTIN, OUTPUT);

  // start the web server on port 80
  server.begin();
}

void OTA::pollWifiState() {
  // Web Server listens to changes
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      LOG.println("device connected to AP");
      led_interval = 900;
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      LOG.println("Device disconnected from AP");
      led_interval = 1500;
    }
  }
  // led blink logik 
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= led_interval) {
    if (ledState == HIGH)
      ledState = LOW;
    else
      ledState = HIGH;

    digitalWrite(LED_BUILTIN, ledState);
    previousMillis = currentMillis;
  }
}

void OTA::pollWebserver() {
  WiFiClient client = server.available(); // listen for incoming clients
  if (!client) return;
  bool flashSuccess = false;

  LOG.println("new connection");

  bool currentLineIsBlank = true;
  String req_str = "";

  while (client.connected()) {
    if (!client.available()) continue;

    char c = client.read();
    req_str += c;

    // if you've gotten to the end of the line (received a newline
    // character) and the line is blank, the http request has ended,
    // so you can send a reply
    if (c == '\n' && currentLineIsBlank && req_str.startsWith("GET")) {
      break;
    }
    // POST Request handles self update
    if (c == '\n' && currentLineIsBlank && req_str.startsWith("POST")) {
      bool success;
      if (req_str.startsWith("POST /sketch "))
        // handle POST in other function 
        flashSuccess = handlePostSketch(client, req_str);

      break;
    }

    if (c == '\n')
      currentLineIsBlank = true;
    else if (c != '\r')
      currentLineIsBlank = false;
  }

  sendResponse(client);
  client.stop();
  LOG.println("client disconnected");
  
  if (flashSuccess){
    // Reset system to load the flashed sketch 
    stopHardware();
    NVIC_SystemReset();
  }
}

bool OTA::handlePostSketch(WiFiClient& client, String& req_str) {
  // extract length of body
  int contentLengthPos = req_str.indexOf("Content-Length:");
  if (contentLengthPos <= 0) {
    LOG.println("Content-Length is missing, ignoring request");
    return false;
  }
  // extract the actual size 
  String tmp = req_str.substring(contentLengthPos + 15);
  tmp.trim();
  // cast to int 
  uint32_t contentLength = tmp.toInt();
  LOG.println(contentLength);

  // skip the first part of the sketch which contains the OTA code we're currently running from
  uint32_t updateSize = contentLength - OTA_SIZE;
  
  if (contentLength <= OTA_SIZE) {
    LOG.println("update is too small, ignoring");
    return false;
  }

/*  while (updateSize < contentLength--)
    req_str += (char) client.read();
  */
  // skip ota section 
  while (updateSize < contentLength) {
    if (!client.available()) continue;
    contentLength--;
    char c = client.read();
    req_str += c;
  }

  // write the body to flash, page by page
  FlashClass flash;
  uint8_t flashbuffer[FLASH_PAGE_SIZE];
  uint32_t flashAddress = APP_START_ADDRESS;

  uint32_t numPages = ceil((float)updateSize / (float)FLASH_PAGE_SIZE);
  uint32_t lastPageBytes = updateSize % FLASH_PAGE_SIZE == 0
    ? FLASH_PAGE_SIZE
    : updateSize % FLASH_PAGE_SIZE;

  flash.erase((void*)flashAddress, updateSize);

  for (uint32_t i = 0; i < numPages; i++) {
    // do nothing until one buffer size of bytes is available 
    // while (i != numPages - 1 && client.available() < FLASH_PAGE_SIZE) {;}
   uint32_t bufferIndex = 0;
   uint32_t bytesToRead = i == numPages-1 
    ? lastPageBytes
    : FLASH_PAGE_SIZE;
    
   while(bufferIndex < bytesToRead){
    while(!client.available()){;}
    flashbuffer[bufferIndex]=client.read();
    bufferIndex++;
   }
    
    LOG.println(client.available());
    
    // client.read(flashbuffer, sizeof(flashbuffer));
    
    LOG.print("writing received data buffer to flash at 0x");
    LOG.println(String(flashAddress, HEX));
    //LOG.write(flashbuffer, FLASH_PAGE_SIZE);

    flash.write((void*)flashAddress, flashbuffer, sizeof(flashbuffer));

    flashAddress += sizeof(flashbuffer);
  }

  LOG.print("FLASH at 0x12000: 0x");
  LOG.println(String(*(uint32_t*)0x12000, HEX));
  LOG.print("FLASH at 0x12004: 0x");
  LOG.println(String(*(uint32_t*)0x12004, HEX));
  LOG.print("FLASH at 0x12008: 0x");
  LOG.println(String(*(uint32_t*)0x12008, HEX));
  LOG.print("FLASH at 0x1200c: 0x");
  LOG.println(String(*(uint32_t*)0x1200c, HEX));
  LOG.print("FLASH at 0x12010: 0x");
  LOG.println(String(*(uint32_t*)0x12010, HEX));
  LOG.print("FLASH at 0x12014: 0x");
  LOG.println(String(*(uint32_t*)0x12014, HEX));
  LOG.print("FLASH at 0x12018: 0x");
  LOG.println(String(*(uint32_t*)0x12018, HEX));
  LOG.print("FLASH at 0x1201c: 0x");
  LOG.println(String(*(uint32_t*)0x1201c, HEX));
  LOG.print("FLASH at 0x12020: 0x");
  LOG.println(String(*(uint32_t*)0x12020, HEX));
  LOG.print("FLASH at 0x139c1: 0x");
  LOG.println(String(*(uint32_t*)0x139c0, HEX));
  // PROBLEM: app_reset_ptr detected as "f" when jumping.
  // stack pointer set correctly though!
  // -> looks like a byte offset issue (uint32_t <> uint8_t), would explain the sizeof() in SDU lib

  return true;
}

void OTA::jumpToApp() {
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

// TODO: code & message as params
void OTA::sendResponse(WiFiClient client) {
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");
  client.println();
  // The HTTP response ends with another blank line:
  client.println();
}

void OTA::update() {
  pollWifiState();
  pollWebserver();
}
// end all configurations that have been made for the OTA module 
// gets called when succesfully flashed
void OTA::stopHardware(){
  LOG.end();
  WiFi.end();
  digitalWrite(LED_BUILTIN,LOW);
  
  
}
