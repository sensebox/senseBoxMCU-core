/**
* senseBox MCU Example for the Accelerometer
* Dot on the Display is moving according to orientation of the mcu 
* 
* @felixerdy Oktober 2018
* 
*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <senseBoxIO.h>
#include "SenseBoxMCU.h"

 #define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

 BMX055 bmx;

 // defining screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

 void setup() {
  // init display
  senseBoxIO.powerI2C(true);
  delay(2000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.display();
  delay(100);
  display.clearDisplay();
  Serial.begin(9600);

   // init BMX055
  bmx.beginAcc(0x08);
}

 void loop() {
  // clear screen
  display.fillScreen(0);

   // get sensor values
  float XA, YA, ZA, accTotal; // accelerometer axis
  bmx.getAcceleration(&XA, &YA, &ZA, &accTotal);
  Serial.println(XA);
  Serial.println(YA);
  Serial.println(ZA);
  Serial.println(accTotal);
   // map sensor values to screen dimensions
  float xMap = map((XA*1000), -1000, 1000, SCREEN_WIDTH, 0);
  float yMap = map((YA*1000), -1000, 1000, 0, SCREEN_HEIGHT);

   // drawing dot
  display.fillCircle(xMap, yMap, 3, 1);
  display.display();
}
