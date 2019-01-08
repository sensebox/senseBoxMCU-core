// This #include enables the OTA boot mode. simple!
// To boot into OTA mode, press the gray button while resetting.
#include <SenseBoxOTA.h>

// just a tiny demo program. -> success if the led remains on.
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  SerialUSB.begin(115200);
}

void loop() {
  SerialUSB.println("hello OTA world");
  delay(500);
}
