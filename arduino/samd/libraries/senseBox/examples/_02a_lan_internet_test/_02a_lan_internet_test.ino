/*
 * Institute for Geoinformatics, University of Muenster
 * Oct. 2017
 * Network test for senseBox:home
 */

#include <SPI.h>
#include <Ethernet2.h>


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "internet-test.opensensemap.org";
IPAddress ip(192, 168, 0, 42);
EthernetClient client;

void setup() {
  Serial.begin(9600);

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }

  delay(1000);
  Serial.print("Teste Internetverbindung...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("verbunden!");
    Serial.println();
    // Make a HTTP request:
    client.println("GET / HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  }
  else {
    // kf you didn't get a connection to the server:
    Serial.println("Verbindung fehlgeschlagen!");
  }
}

void loop()
{
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  if (!client.connected()) {
    Serial.println();
    Serial.println("Verbindung trennen.");
    client.stop();

    while (true);
  }
}
