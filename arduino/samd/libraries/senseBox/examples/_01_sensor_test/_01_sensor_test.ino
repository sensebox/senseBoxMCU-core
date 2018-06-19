/*  
 * senseBox:home sensor test
 * 
 * Institute for Geoinformatics, University of Muenster
 * Sept. 2017
*/

#include <Wire.h>
#include <HDC100X.h>
#include <BMP280.h>
#include <Makerblog_TSL45315.h>
#include <VEML6070.h>

#define I2C_ADDR 0x38
#define IT_1   0x1 //1T

HDC100X HDC(0x43);
BMP280 BMP;
VEML6070 VEML;
Makerblog_TSL45315 TSL = Makerblog_TSL45315(TSL45315_TIME_M4);


void setup()
{
  Serial.begin(9600);
  Serial.println("Starting test...\n");
  delay(1000);
  scan();
  delay(1000);
  
  HDC.begin(HDC100X_TEMP_HUMI, HDC100X_14BIT, HDC100X_14BIT, DISABLE);
  VEML.begin();
  TSL.begin();
  BMP.begin();
  BMP.setOversampling(4);

  delay(5000);
}


void loop()
{
  char result;
  double T=0, P=0;
  uint32_t lux;
  byte msb=0, lsb=0;
  uint16_t uv;

  //HDC1080
  Serial.print("Humidity:         "); 
    Serial.print(HDC.getHumi(),1); Serial.println(" %");
  delay(200);
  Serial.print("Temperature:      "); 
    Serial.print(HDC.getTemp(),1); Serial.print(" \xC2\xB0"); Serial.println("C");
  delay(200);

  //BMP280
  result = BMP.startMeasurment();
  delay(result);
  BMP.getTemperatureAndPressure(T, P);
  Serial.print("Air pressure:     "); 
    Serial.print(P, 2); Serial.println(" hPa");
  Serial.print("Temperature:      "); 
    Serial.print(T, 1); Serial.print(" \xC2\xB0"); Serial.println("C");
  delay(200);

  //TSL45315
  lux = TSL.readLux();
  Serial.print("Illuminance:      ");
  Serial.print(lux, DEC);
  Serial.println(" lx");

  //VEML6070
  uv = VEML.getUV();
  Serial.print("UV radiation:     "); 
    Serial.print(uv, DEC); Serial.println(" uW/cm2");

  Serial.println();
  delay(3000);
}

void scan(void)
{
  byte error, address;

  Serial.println("Looking for devices...");

  Wire.begin();
  for(address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if(error == 0)
    {
      Serial.print("Device found: 0x");
      if(address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
    else if(error==4)
    {
      Serial.print("Error: 0x");
      if(address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
  }
  Serial.println("");
}
