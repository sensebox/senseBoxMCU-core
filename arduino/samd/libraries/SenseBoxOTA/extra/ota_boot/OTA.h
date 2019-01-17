// Libs needed for WiFi
#if (ARDUINO >= 100)
    #include "Arduino.h"
    #include <WiFi101.h>
#else
 #include "WProgram.h"
#endif

// uncomment this for OTA debug statements. note: nothing will execute until serial monitor is opened!
//#define OTA_DEBUG
#define LOG SerialUSB

// these values must correspond  to the linker script flash_with_ota.ld
#define OTA_START 0x2000
#define OTA_SIZE 0x10000
#define APP_START_ADDRESS (OTA_START + OTA_SIZE)

#define FLASH_SIZE 0x40000UL
#define FLASH_PAGE_SIZE 512

class OTA {
    public:
        OTA();
        void begin(bool accessPointMode);
        void update(void);

    private:
        void createAccessPoint();
        void sendResponse(WiFiClient client, bool flashSuccess);
        void pollWebserver();
        void pollWifiState();
        bool handlePostSketch(WiFiClient& client, String& req_str);
        void stopHardware();

        byte mac[6];
        int status;
        WiFiServer server; // Server on Port 80

        // LED state handling
        unsigned long previousMillis = 0;
        int led_interval = 2000;
        int ledState = HIGH;
};
