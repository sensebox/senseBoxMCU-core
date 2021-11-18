# 1 "/Users/mariopesch/Library/Arduino15/packages/sensebox/hardware/samd/1.4.2/libraries/SenseBoxOTA/extra/ota_boot/OTA.cpp"
# 2 "/Users/mariopesch/Library/Arduino15/packages/sensebox/hardware/samd/1.4.2/libraries/SenseBoxOTA/extra/ota_boot/OTA.cpp" 2

# 4 "/Users/mariopesch/Library/Arduino15/packages/sensebox/hardware/samd/1.4.2/libraries/SenseBoxOTA/extra/ota_boot/OTA.cpp" 2
# 5 "/Users/mariopesch/Library/Arduino15/packages/sensebox/hardware/samd/1.4.2/libraries/SenseBoxOTA/extra/ota_boot/OTA.cpp" 2
# 6 "/Users/mariopesch/Library/Arduino15/packages/sensebox/hardware/samd/1.4.2/libraries/SenseBoxOTA/extra/ota_boot/OTA.cpp" 2

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
    SerialUSB.println("WiFi shield not present");
    while (true)
      ;
  }
  // Assign mac address to byte array & push it to a char array
  WiFi.macAddress(mac);
  String mac_str = String(mac[1], 16) + String(mac[0], 16);
  mac_str.toUpperCase();
  String ssid_string = String("senseBox:" + mac_str);
  char ssid[20];
  ssid_string.toCharArray(ssid, 20);

  SerialUSB.print("Creating access point named: ");
  SerialUSB.println(ssid);

  // initialize wifi: set SSID based on last 4 bytes of MAC address
  status = WiFi.beginAP(ssid);
  if (status != WL_AP_LISTENING)
  {
    SerialUSB.println("Creating access point failed");
    while (true)
      ;
  }

  pinMode((8u) /* D8/PA28 (green)*/, OUTPUT);
}

void OTA::pollWifiState()
{
  // blink faster if a device is connected to the access point
  if (status != WiFi.status())
  {
    status = WiFi.status();

    if (status == WL_AP_CONNECTED)
    {
      SerialUSB.println("device connected to AP");
      led_interval = 900;
    }
    else
    {
      // a device has disconnected from the AP, and we are back in listening mode
      SerialUSB.println("Device disconnected from AP");
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

    digitalWrite((8u) /* D8/PA28 (green)*/, ledState);
    previousMillis = currentMillis;
  }
}

void OTA::pollWebserver()
{
  // listen for incoming clients
  WiFiClient client = server.available();
  if (!client)
    return;

  SerialUSB.println("new connection");

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
  SerialUSB.println("client disconnected");

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
    SerialUSB.println("Content-Length is missing, ignoring request");
    return false;
  }
  String tmp = req_str.substring(contentLengthPos + 15);
  tmp.trim();
  uint32_t contentLength = tmp.toInt();
  SerialUSB.println(contentLength);

  // if (contentLength <= OTA_SIZE) {
  //   LOG.println("update is too small, ignoring");
  //   return false;
  // }
  if (contentLength > (0x40000UL - 0x2000))
  {
    SerialUSB.println("update is too large, ignoring");
    return false;
  }

  // skip the first part of the sketch which contains the OTA code we're currently running from.
  // the new sketch needs to still include this section in order for internal memory adresses to
  // be compiled correctly (unconfirmed, but it didn't work withou)
  uint32_t updateSize = contentLength - 0x10000;
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
  uint8_t flashbuffer[512];
  uint32_t flashAddress = (0x2000 + 0x10000);

  flash.erase((void *)flashAddress, updateSize);

  uint32_t numPages = ceil((float)updateSize / (float)512);
  uint32_t lastPageBytes = updateSize % 512 == 0
                               ? 512
                               : updateSize % 512;

  for (uint32_t i = 0; i < numPages; i++)
  {
    // fill the page buffer, reading one byte at a time.
    uint32_t bufferIndex = 0;
    uint32_t bytesToRead = i == numPages - 1 ? lastPageBytes : 512;
    while (bufferIndex < bytesToRead)
    {
      while (!client.available())
      {
        ;
      } // don't continue until we received new data
      flashbuffer[bufferIndex] = client.read();
      bufferIndex++;
    }

    SerialUSB.print("writing received data buffer to flash at 0x");
    SerialUSB.println(String(flashAddress, 16));

    flash.write((void *)flashAddress, flashbuffer, sizeof(flashbuffer));
    flashAddress += sizeof(flashbuffer);
  }

  SerialUSB.print("FLASH at 0x12000: 0x");
  SerialUSB.println(String(*(uint32_t *)0x12000, 16));
  SerialUSB.print("FLASH at 0x12004: 0x");
  SerialUSB.println(String(*(uint32_t *)0x12004, 16));
  SerialUSB.print("FLASH at 0x12008: 0x");
  SerialUSB.println(String(*(uint32_t *)0x12008, 16));
  SerialUSB.print("FLASH at 0x1200c: 0x");
  SerialUSB.println(String(*(uint32_t *)0x1200c, 16));

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
  SerialUSB.end();
  WiFi.end();
  digitalWrite((8u) /* D8/PA28 (green)*/, LOW);
}
# 1 "/Users/mariopesch/Library/Arduino15/packages/sensebox/hardware/samd/1.4.2/libraries/SenseBoxOTA/extra/ota_boot/ota_boot.ino"


OTA ota_module;

bool validateFlashedApp() {
  /**
   * Test reset vector of application @APP_START_ADDRESS+4
   * Sanity check on the Reset_Handler address.
   * TODO FIXME: proper CRC / signature check?
   */

  /* Load the Reset Handler address of the application */
  uint32_t app_reset_ptr = *(uint32_t *)((0x2000 + 0x10000) + 4);

  return (app_reset_ptr >= (0x2000 + 0x10000) && app_reset_ptr <= 0x40000UL);
}

void jumpToApp() {
  /* Load the Reset Handler address of the application */
  uint32_t app_reset_ptr = *(uint32_t *)((0x2000 + 0x10000) + 4);

  SerialUSB.print("app_reset_ptr: ");
  SerialUSB.println(String(app_reset_ptr, 16));

  SerialUSB.print("stack pointer: ");
  SerialUSB.println(String(*(uint32_t *)(0x2000 + 0x10000), 16));

  SerialUSB.print("vector table address: ");
  SerialUSB.println(String(((uint32_t)(0x2000 + 0x10000) & (0xFFFFFFUL << 8U /*!< SCB VTOR: TBLOFF Position */) /*!< SCB VTOR: TBLOFF Mask */), 16));

  /* Rebase the Stack Pointer */
  __set_MSP(*(uint32_t *)(0x2000 + 0x10000));

  /* Rebase the vector table base address */
  ((SCB_Type *) ((0xE000E000UL) /*!< System Control Space Base Address */ + 0x0D00UL) /*!< System Control Block Base Address */ ) /*!< SCB configuration struct */->VTOR = ((uint32_t)(0x2000 + 0x10000) & (0xFFFFFFUL << 8U /*!< SCB VTOR: TBLOFF Position */) /*!< SCB VTOR: TBLOFF Mask */);

  /* Jump to application Reset Handler in the application */
  asm("bx %0" ::"r"(app_reset_ptr));
}


void setup() {
  pinMode(0, INPUT_PULLUP); // switch button


  SerialUSB.begin(115200);
  while(!SerialUSB) {;} // dont continue until serial was opened


  // digitalRead(0) gives the value for the switch button (0 means its pressed)
  if (digitalRead(0) == LOW) {
    SerialUSB.println("[OTA] switch pressed, entering OTA mode");
  } else if (validateFlashedApp()) {
    SerialUSB.println("[OTA] jumping to application");
    jumpToApp();
  } else {
    SerialUSB.println("[OTA] no valid app installed, entering OTA mode");
  }

  // Setup WiFi access point
  delay(400); // otherwise the whole app hangs..
  ota_module.begin(true);
}

void loop() {
  ota_module.update(); // poll for updates in the OTA HTTP server
}
