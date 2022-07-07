#include "OTA.h"

#include <SPI.h>
#include <Arduino.h>
#include <FlashStorage.h>

OTA::OTA() : server(80), status(WL_IDLE_STATUS) {}

void OTA::begin(bool accessPointMode)
{
  // switch modes: also allow no access point (for use with just webserver in an existing network)
  if (accessPointMode)
    createAccessPoint();

  server.begin(); // start the web server on port 80
}

void OTA::update()
{
  pollWifiState();
  pollWebserver();
}

void OTA::createAccessPoint()
{
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present");
    while (true)
      ;
  }
  // get wifi mac address and use last bytes to generate the wifi SSID
  char ssid[20];
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(ssid, "senseBox:%.2X%.2X", mac[1], mac[0]);

  LOG.print("Creating access point named: ");
  LOG.println(ssid);

  // initialize wifi: set SSID based on last 4 bytes of MAC address
  status = WiFi.beginAP(ssid);
  if (status != WL_AP_LISTENING)
  {
    LOG.println("Creating access point failed");
    while (true)
      ;
  }

  pinMode(LED_BUILTIN2, OUTPUT);
}

void OTA::pollWifiState()
{
  // blink faster if a device is connected to the access point
  if (status != WiFi.status())
  {
    status = WiFi.status();

    if (status == WL_AP_CONNECTED)
    {
      LOG.println("device connected to AP");
      led_interval = 900;
    }
    else
    {
      // a device has disconnected from the AP, and we are back in listening mode
      LOG.println("Device disconnected from AP");
      led_interval = 2000;
    }
  }

  // LED blink logic
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= led_interval)
  {
    if (ledState == HIGH)
      ledState = LOW;
    else
      ledState = HIGH;

    digitalWrite(LED_BUILTIN2, ledState);
    previousMillis = currentMillis;
  }
}

void OTA::pollWebserver()
{
  // listen for incoming clients
  WiFiClient client = server.available();
  if (!client)
    return;

  LOG.println("new connection");

  bool flashSuccess = false;
  bool currentLineIsBlank = true;
  String req_str = "";

  while (client.connected())
  {
    if (!client.available())
      continue;

    char c = client.read();
    req_str += c;

    // if you've gotten to the end of the line (received a newline
    // character) and the line is blank, the http request has ended,
    // so you can send a reply

    // POST request also needs to handle self update
    if (c == '\n' && currentLineIsBlank && req_str.startsWith("POST"))
    {
      // handlers in here must consume the rest of the body
      if (req_str.startsWith("POST /sketch "))
        flashSuccess = handlePostSketch(client, req_str);
      break;
    }

    if (c == '\n' && currentLineIsBlank)
      break;
    if (c == '\n')
      currentLineIsBlank = true;
    else if (c != '\r')
      currentLineIsBlank = false;
  }

  sendResponse(client, req_str.startsWith("GET") || flashSuccess);
  client.stop();
  LOG.println("client disconnected");

  if (flashSuccess)
  {
    // Reset system to load the flashed sketch
    stopHardware();
    NVIC_SystemReset();
  }
}

/**
 * this function implements the self update.
 * NOTE FIXME TODO: this does not do any verification on the received data
 * but blindly writes it to flash!
 */
bool OTA::handlePostSketch(WiFiClient &client, String &req_str)
{
  // extract length of body
  int contentLengthPos = req_str.indexOf("Content-Length:");
  if (contentLengthPos <= 0)
  {
    LOG.println("Content-Length is missing, ignoring request");
    return false;
  }
  String tmp = req_str.substring(contentLengthPos + 15);
  tmp.trim();
  uint32_t contentLength = tmp.toInt();
  LOG.println(contentLength);

  // if (contentLength <= OTA_SIZE) {
  //   LOG.println("update is too small, ignoring");
  //   return false;
  // }
  if (contentLength > (FLASH_SIZE - 0x2000))
  {
    LOG.println("update is too large, ignoring");
    return false;
  }

  // skip the first part of the sketch which contains the OTA code we're currently running from.
  // the new sketch needs to still include this section in order for internal memory adresses to
  // be compiled correctly (unconfirmed, but it didn't work withou)
  uint32_t updateSize = contentLength - OTA_SIZE;
  while (updateSize < contentLength)
  {
    if (!client.available())
      continue;
    contentLength--;
    char c = client.read();
    req_str += c;
  }

  // write the body to flash, page by page
  FlashClass flash;
  uint8_t flashbuffer[FLASH_PAGE_SIZE];
  uint32_t flashAddress = APP_START_ADDRESS;

  flash.erase((void *)flashAddress, updateSize);

  uint32_t numPages = ceil((float)updateSize / (float)FLASH_PAGE_SIZE);
  uint32_t lastPageBytes = updateSize % FLASH_PAGE_SIZE == 0
                               ? FLASH_PAGE_SIZE
                               : updateSize % FLASH_PAGE_SIZE;

  for (uint32_t i = 0; i < numPages; i++)
  {
    // fill the page buffer, reading one byte at a time.
    uint32_t bufferIndex = 0;
    uint32_t bytesToRead = i == numPages - 1 ? lastPageBytes : FLASH_PAGE_SIZE;
    while (bufferIndex < bytesToRead)
    {
      while (!client.available())
      {
        ;
      } // don't continue until we received new data
      flashbuffer[bufferIndex] = client.read();
      bufferIndex++;
    }

    LOG.print("writing received data buffer to flash at 0x");
    LOG.println(String(flashAddress, HEX));

    flash.write((void *)flashAddress, flashbuffer, sizeof(flashbuffer));
    flashAddress += sizeof(flashbuffer);
  }

  LOG.print("FLASH at 0x12000: 0x");
  LOG.println(String(*(uint32_t *)0x12000, HEX));
  LOG.print("FLASH at 0x12004: 0x");
  LOG.println(String(*(uint32_t *)0x12004, HEX));
  LOG.print("FLASH at 0x12008: 0x");
  LOG.println(String(*(uint32_t *)0x12008, HEX));
  LOG.print("FLASH at 0x1200c: 0x");
  LOG.println(String(*(uint32_t *)0x1200c, HEX));

  return true;
}

void OTA::sendResponse(WiFiClient client, bool flashSuccess)
{
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  if (flashSuccess)
    client.println("HTTP/1.1 200 OK");
  else
    client.println("HTTP/1.1 400 Bad Request");
  client.println("Content-type:text/html");
  client.println("Content-Length: 0");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");
  client.println();
  client.println();
}

// reset all configurations that have been made for the OTA module
// gets called when successfully flashed
void OTA::stopHardware()
{
  LOG.end();
  WiFi.end();
  digitalWrite(LED_BUILTIN2, LOW);
}
