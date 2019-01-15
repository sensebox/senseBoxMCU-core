// just a tiny demo program:
// If you flash it by wire, you have enabled the OTA boot mode.
// If you flash it via OTA, you should see a working hello world.
//   -> success if the LED blinks & "hello OTA world" is printed!

// This #include enables the OTA boot mode. simple!
// To boot into OTA mode, press the gray button while resetting.
#include <SenseBoxOTA.h>

unsigned long previousMillis = 0;
int led_interval = 200;
int ledState = LOW;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  SerialUSB.begin(115200);
}

void loop() {
  SerialUSB.println("hello OTA world");

  // blink the LED
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= led_interval) {
    if (ledState == HIGH)
      ledState = LOW;
    else
      ledState = HIGH;

    digitalWrite(LED_BUILTIN, ledState);
    previousMillis = currentMillis;
  }
  delay(100);
}
