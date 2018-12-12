// Libs needed for WiFi
// #include <SPI.h>
// #include <Arduino.h>
#if (ARDUINO >= 100)
    #include "Arduino.h"
    #include <WiFi101.h>

#else
 #include "WProgram.h"
#endif


class SenseBoxOTA{
    public:
        SenseBoxOTA();
        void begin(void);
        void update(void);

    private:
        void sendPage(WiFiClient client);
        void printMacAddress(byte mac[]);
        void printWiFiStatus(void);

    private:
        byte mac[6];
        int status;
        WiFiServer server; // Server on Port 80

};