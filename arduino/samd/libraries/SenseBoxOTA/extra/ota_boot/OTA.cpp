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
  String mac_str = String(mac[1], HEX) + String(mac[2], HEX);
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

// TODO: update LED blink
void OTA::pollWifiState() {
  // Web Server listens to changes
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      LOG.println("device connected to AP");
      led_interval = 300;
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      LOG.println("Device disconnected from AP");
      led_interval = 900;
    }
  }

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
      sendResponse(client);
      break;
    }

    if (c == '\n' && currentLineIsBlank && req_str.startsWith("POST")) {
      if (req_str.startsWith("POST /sketch "))
        handlePostSketch(client, req_str);

      sendResponse(client);
      break;
    }

    if (c == '\n')
      currentLineIsBlank = true;
    else if (c != '\r')
      currentLineIsBlank = false;
  }

  client.stop();
  LOG.println("client disconnected");
}

void OTA::handlePostSketch(WiFiClient& client, String& req_str) {
  // extract length of body
  String tmp = req_str.substring(req_str.indexOf("Content-Length:") + 15);
  tmp.trim();
  uint32_t contentLength = tmp.toInt();

  // skip the first part of the sketch which contains the OTA code we're currently running from
  uint32_t updateSize = contentLength - OTA_SIZE;

  if (contentLength <= OTA_SIZE) {
    LOG.println("update is too small, ignoring");
    return;
  }

  while (updateSize < contentLength--) {
    char c = client.read();
    req_str += c;
  }

  // write the body to flash, page by page
  FlashClass flash;
  uint8_t flashbuffer[FLASH_PAGE_SIZE];
  uint32_t flashbufferindex = 0;
  uint32_t flashAddress = APP_START_ADDRESS;

  flash.erase((void*)flashAddress, updateSize);

  for (uint32_t i = 0; i < updateSize; i += FLASH_PAGE_SIZE) {
    client.read(flashbuffer, sizeof(flashbuffer));

    LOG.print("writing received data buffer to flash at ");
    LOG.println(flashAddress);
    LOG.println((char*)flashbuffer);

    flash.write((void*)flashAddress, flashbuffer, FLASH_PAGE_SIZE);

    flashAddress += sizeof(flashbuffer);
  }

  jumpToApp();
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
