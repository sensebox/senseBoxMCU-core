#include <WiFi101.h>

char ssid[] = "";      //  your network SSID (name)
char pass[] = "";   // your network password

int status = WL_IDLE_STATUS;
// Initialize the Wifi client library
WiFiClient client;
// server address:
char server[] = "internet-test.opensensemap.org";

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  pinMode(4, INPUT);
  digitalWrite(4, HIGH);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);

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
}

void loop() {
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
