/*
 * senseBox:home dust particle/feinstaub addon
 * SDS011 sensor test
 * 
 * Note: Disconnect SDS011 sensor before uploading code.
 */

#include <SDS011-select-serial.h>

float p10,p25;
int error;

SDS011 SDS(Serial);

void setup() {
	// initialize normal Serial port
	Serial.begin(9600);
  Serial.println("Sensor needs up to 30 seconds for heating.\n");
}

void loop() {
	error = SDS.read(&p25,&p10);
	if (! error) {
		Serial.println("PM2.5: "+String(p25,1));
		Serial.println("PM10:  "+String(p10,1));
    Serial.println();
	}
	delay(3000);
}
