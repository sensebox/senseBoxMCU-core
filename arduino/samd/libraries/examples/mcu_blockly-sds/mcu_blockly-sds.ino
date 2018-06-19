#include <SenseBoxMCU.h>

float p10,p25;
int error;

SDS011 my_sds(Serial1);

void setup() {
  // initialize normal Serial port
  Serial.begin(9600);
  while(!Serial);
  // initalize SDS Serial Port
  Serial1.begin(9600);
  delay(5000);
}

void loop() {
  //error = my_sds.read(&p25,&p10);
  p10 = my_sds.getPm10();
  p25 = my_sds.getPm25();
  if (! error) {
    Serial.println("P2.5: "+String(p25));
    Serial.println("P10:  "+String(p10));
  }
  delay(1000);
}
