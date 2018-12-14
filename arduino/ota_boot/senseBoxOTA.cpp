#include <SPI.h>
#include <Arduino.h>
#include "senseBoxOTA.h"

SenseBoxOTA::SenseBoxOTA():
      server(80),
      status(WL_IDLE_STATUS)
    {
      
    }

void SenseBoxOTA::begin()
{ 
  // TODO: check for wifi shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
  // Assign mac address to byte array
  // & push it to a char array 
  WiFi.macAddress(mac);
  String ssid_string = String("senseBox:"+String(mac[1],HEX)+String(mac[0],HEX));
  char ssid[20];
  ssid_string.toCharArray(ssid,20);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);
  // TODO: initialize wifi
  // set SSID based on last 4 bytes of MAC address
  status = WiFi.beginAP(ssid);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
    }
  
  pinMode(LED_BUILTIN, OUTPUT);

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();
}

void SenseBoxOTA::sendPage(WiFiClient client)
{
    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Access-Control-Allow-Origin:*");
  client.println("Content-type:text/html");
  client.println();
  // the content of the HTTP response follows the header:
  client.println("<form method='post' enctype='multipart/form-data'>");
  client.println("<input name='file' type='file'>");
  client.println("<input class='button' type='submit' value='Upload'>");
  client.println("</form>"); 
  client.print("Click <a href=\'/H\'>here</a> turn the LED blue<br>");
  client.print("Click <a href=\'/L\'>here</a> turn the LED yellow<br>");

  // The HTTP response ends with another blank line:
  client.println();
}

void SenseBoxOTA::printMacAddress(byte mac[])
{
    for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void SenseBoxOTA::printWiFiStatus()
{

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void SenseBoxOTA::update() {
  // Web Server listens to changes 
  if (status != WiFi.status()) {
      // it has changed update the variable
      status = WiFi.status();
  
      if (status == WL_AP_CONNECTED) {
        byte remoteMac[6];
        // a device has connected to the AP
        Serial.print("Device connected to AP, MAC address: ");
        WiFi.APClientMacAddress(remoteMac);
        printMacAddress(remoteMac);
      } else {
        // a device has disconnected from the AP, and we are back in listening mode
        Serial.println("Device disconnected from AP");
      }
    }
    
    WiFiClient client = server.available();   // listen for incoming clients
  
    if (client) {                             // if you get a client,
      boolean skip = false;
      boolean post = false;
      Serial.println("new client");           // print a message out the serial port
      String currentLine = "";
      int webkitCounter = 0;                     // make a String to hold incoming data from the client
      String file = "";
      while (client.connected()) {                          // loop while the client's connected
        if (client.available()) {             // if there's bytes to read from the client,
          char c = client.read();             // read a byte, then
          Serial.write(c);                    // print it out the serial monitor
          if(webkitCounter == 1){
            file += c;
          }
          if (c == '\n') {                    // if the byte is a newline character
  
            if(currentLine.startsWith("POST")){   // if its a POST look at the whole response with body
              post = true;
              skip = true;
            }
            if(webkitCounter == 1){ // if the parser is within the sended file 
              skip=true;
              }
            
            if(currentLine.startsWith("Content-Type: application/octet-stream")){
              webkitCounter = webkitCounter + 1;
            }
            
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if ((currentLine.length()==0 && !skip) || webkitCounter == 1 && currentLine.startsWith("----")) {
              // Send response
              if(post){
              Serial.println("The file sent looks like this");
              Serial.println("---------------start----------------");
              Serial.println(file);
              Serial.println("---------------end------------------");
              }
              // give response back to the client
              sendPage(client);
              // break out of the while loop:
              break;
            }
            else if(currentLine.length()== 0 && skip){
                skip = false;
                continue;
                ;}
            else {      // if you got a newline, then clear currentLine:
              currentLine = "";
            }
          }
          else if (c != '\r') {    // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
          }
        }
  
        }
      
      // close the connection:
      client.stop();
      Serial.println("client disconnected");
}
}
