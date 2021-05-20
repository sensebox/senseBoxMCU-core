/*
	Read Lux value from senseBox light sensor LTR329 (default I2C address is 0x29)
*/

#include <LTR329.h>
#include <Wire.h>
#include <senseBoxIO.h>

LTR329 light;

unsigned char gain = 1;     // Gain setting, values = 0-7
// If gain = 0, device is set to 1X gain (default)
// If gain = 1, device is set to 2X gain
// If gain = 2, device is set to 4X gain
// If gain = 3, device is set to 8X gain
// If gain = 4, invalid (do not use)
// If gain = 5, invalid (do not use)
// If gain = 6, device is set to 48X gain
// If gain = 7, device is set to 96X gain
unsigned char integrationTime = 0;  // Integration ("shutter") time
// If integrationTime = 0, integrationTime will be 100ms (default)
// If integrationTime = 1, integrationTime will be 50ms
// If integrationTime = 2, integrationTime will be 200ms
// If integrationTime = 3, integrationTime will be 400ms
// If integrationTime = 4, integrationTime will be 150ms
// If integrationTime = 5, integrationTime will be 250ms
// If integrationTime = 6, integrationTime will be 300ms
// If integrationTime = 7, integrationTime will be 350ms
unsigned char measurementRate = 3;  // Interval between DATA_REGISTERS update
// If measurementRate = 0, measurementRate will be 50ms
// If measurementRate = 1, measurementRate will be 100ms
// If measurementRate = 2, measurementRate will be 200ms
// If measurementRate = 3, measurementRate will be 500ms (default)
// If measurementRate = 4, measurementRate will be 1000ms
// If measurementRate = 5, measurementRate will be 2000ms
// If measurementRate = 6, measurementRate will be 2000ms
// If measurementRate = 7, measurementRate will be 2000ms

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("senseBox luxmeter example sketch");

  delay(100);   // 100ms initial startup time required
  light.begin();

  unsigned char ID;
  if (light.getPartID(ID)) {
    Serial.print("Sensor Part ID: 0X");
    Serial.print(ID, HEX);
  }
  else {
    byte error = light.getError();
    printError(error);
  }

  //gain setting
  Serial.println("Setting gain...");
  light.setControl(gain, false, false);

  //integration time and measurement rate setting
  Serial.println("Setting timing...");
  light.setMeasurementRate(integrationTime, measurementRate);

  // To start taking measurements, power up the sensor:
  Serial.println("Powerup...");
  light.setPowerUp();
}

void loop() {
  unsigned int data0, data1; //read values from the two integrated photodiodes of LTR329

  if (light.getData(data0, data1)) {
    // getData() returned true, communication was successful
    Serial.print("data0: ");
    Serial.println(data0);
    Serial.print("data1: ");
    Serial.println(data1);

    // The getLux() function will return 1 if successful or 0 if aturated (too much light)

    unsigned int lux;    // Resulting lux value
    boolean valid;  // True if neither sensor is saturated

    // Perform lux calculation:

    valid = light.getLux(gain, integrationTime, data0, data1, lux);

    // Print out the results:

    Serial.print(" lux: ");
    Serial.println(lux);
    if (valid) Serial.println(" (valid)"); else Serial.println(" (sensor saturated)");
  }
  else {
    // getData() returned false because of an I2C error, inform the user.
    byte error = light.getError();
    printError(error);
  }
  delay(100);
}

void printError(byte error) {
  // If there's an I2C error, this function will
  // print out an explanation.

  Serial.print("I2C error: ");
  Serial.print(error, DEC);
  Serial.print(", ");

  switch (error) {
    case 0:
      Serial.println("success");
      break;
    case 1:
      Serial.println("data too long for transmit buffer");
      break;
    case 2:
      Serial.println("received NACK on address (disconnected?)");
      break;
    case 3:
      Serial.println("received NACK on data");
      break;
    case 4:
      Serial.println("other error");
      break;
    default:
      Serial.println("unknown error");
  }
}
