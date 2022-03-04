#include <senseBoxIO.h>
#include "WebUtil.h"
#include <WiFi101.h>
#include <Adafruit_HDC1000.h>



WiFiServer server(80);

char ssid[] = "sensebox";
int status = WL_IDLE_STATUS;

Adafruit_HDC1000 hdc = Adafruit_HDC1000();


void setup() {
  Serial.begin(9600);
if (WiFi.status() == WL_NO_SHIELD) {
    while (true);
}
    status = WiFi.beginAP(ssid);
    Serial.println("AP created");
    delay(5000);
  server.begin();
  hdc.begin();

  Serial.println("starting loop");

}

void loop() {

WiFiClient client = server.available();
  if (client && client.available()) {
    Serial.println("device connected");
    String request_string = listenClient(client);
    Request request;
    if (parseRequestSafe(request_string, request)) {
    if (request.method == "GET" && request.uri == "/sensoren.html") {
      client.println(buildSuccessfulResponse(request, buildHTML("", buildTag("table",
       buildTag("tr",
       buildTag("tr",
       buildTag("td",
       buildTag("b",
       "Sensor")) +buildTag("td",
       buildTag("b",
       "Messwert")))) +buildTag("tr",
       buildTag("td",
       "Temperatur") +buildTag("td",
       String(hdc.readTemperature()))) +buildTag("tr",
       buildTag("td",
       "Luftfeuchtigkeit") +buildTag("td",
       String(hdc.readHumidity())))))));
    } else {
      client.println(buildNotFoundResponse(request));
    }
    }
    delay(1);
    client.stop();
    delay(1);
  }

}