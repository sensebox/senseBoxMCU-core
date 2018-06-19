#include <SenseBoxMCU.h>

Bee* b = new Bee();

OpenSenseMap osem("senseBox ID", b);
float temp = 24.3;
void setup() {
  b->connectToWifi("SSID","PW");
  delay(1000);
  osem.uploadMeasurement(temp, "sensor ID");
}

void loop() {
  // put your main code here, to run repeatedly:

}
