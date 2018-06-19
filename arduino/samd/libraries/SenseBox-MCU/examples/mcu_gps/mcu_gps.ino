#include <SenseBoxMCU.h>

GPS gps;
float lat; //Geografische Breite
float lng; //Geografische Länge
float alt; //Höhe über Meeresspiegel in Metern
float speed;

void setup() {
  gps.begin();
}

void loop() {
  lat = gps.getLatitude();
  lng = gps.getLongitude();
  alt = gps.getAltitude();
  speed = gps.getSpeed();
  
  Serial.print(lat,6);
  Serial.print(F(","));
  Serial.print(lng,6);
  Serial.print(F(","));
  Serial.println(alt,1);
  Serial.print(F(","));
  Serial.println(speed,4);
  delay(100);
}

