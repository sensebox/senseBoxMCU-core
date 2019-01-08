// Libs needed for WiFi
// #include <SPI.h>
// #include <Arduino.h>
#if (ARDUINO >= 100)
    #include "Arduino.h"
    #include <WiFi101.h>

#else
 #include "WProgram.h"
#endif


class OTA {
    public:
        OTA();
        void begin(void);
        void update(void);
        void jumpToApp();

    private:
        void sendResponse(WiFiClient client);
        void pollWebserver();
        void pollWifiState();
        bool handlePostSketch(WiFiClient& client, String& req_str);

        byte mac[6];
        int status;
        WiFiServer server; // Server on Port 80

        // LED state handling
        unsigned long previousMillis = 0;
        int led_interval = 1500;
        int ledState = HIGH;
};
