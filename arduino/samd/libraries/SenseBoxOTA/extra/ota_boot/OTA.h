// Libs needed for WiFi
#if (ARDUINO >= 100)
    #include "Arduino.h"
    #include <WiFi101.h>
#else
 #include "WProgram.h"
#endif

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
