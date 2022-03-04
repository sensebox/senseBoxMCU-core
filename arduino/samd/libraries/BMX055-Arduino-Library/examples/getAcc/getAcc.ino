#include <senseBoxIO.h>
#include "BMX055.h"

BMX055 bmx;


void setup(){
Serial.begin(9600);
bmx.beginAcc(0x3);


}

void loop(){
Serial.print("x: ");
Serial.println(bmx.getAccelerationX());
Serial.print("y: ");
Serial.println(bmx.getAccelerationY());
Serial.print("z: ");
Serial.println(bmx.getAccelerationZ());
delay(100);


}