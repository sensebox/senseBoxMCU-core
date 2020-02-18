//#ifndef SenseBoxMCU_h
//#define SenseBoxMCU_h
#pragma once

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "Wire.h"
#include <SPI.h>
#include <WiFi101.h>
#include <TinyGPS++.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "senseBoxIO.h"
#include <SD.h>
#include "WebUtil.h"

#define HDC1080_ADDR 0x40
#define BMP280_ADDR 0x76
#define VEML6070_ADDR 0x38
#define TSL45315_ADDR 0x29
#define BMX055_ACCL_ADDR 0x18
#define BMX055_GYRO_ADDR 0x68
#define BMX055_MAGN_ADDR 0x10

#define VEML6070_INTEGRATION_TIME_1 0x01 //IT_1: 5.625 uW/cm2/step

class Bee 
{
	public:
	Bee();
		uint8_t connectToWifi(char* ssid);
		uint8_t connectToWifi(char* ssid, char* password);
		void startAP(char* ssid);
		char* getSsid();
		char* getPassword();
		char* getIpAddress();
	private:
		char* nwid = "";
		char* pw = "";
		char ip[15];
		int status = WL_IDLE_STATUS;
		void storeIpAddress();
};


class OpenSenseMap
{
	public:
		OpenSenseMap(const char* boxId, Bee* bee, const char* host);
		void uploadMeasurement(float value, char* sensorID);
		void uploadMobileMeasurement(float value, char* sensorID, float lat, float lng);
		void setUploadInterval(unsigned int);
	private:
		const char* senseBoxID;
		const char* server = "ingress.opensensemap.org";
		const int port = 80;
		unsigned int uploadInterval = 10000;
		Client* client = NULL;
		Bee* xbee = NULL;
		const char* osemHost;
};

class SDS011
{
	public:
		SDS011(Stream& serial);
		float getPm10(void);
		float getPm25(void);
	private:
		Stream& sds_data;
		int read(float *p25, float *p10);
		float pm10 = 0;
		float pm25 = 0;
};

class HDC1080
{ //Ausgabe in Fahrenheit
	public:
		uint8_t begin(void);
		double getTemperature(void);
		double getHumidity(void); 
};

class VEML6070
{
	public:
		uint8_t begin(void);
		double getUvIntensity(void);
};

class TSL45315
{
	public:
		uint8_t begin(void);
		unsigned long getIlluminance(void); 
};

class Ultrasonic
{
  public:
    Ultrasonic(int rx, int tx);
        long getDistance(void);
  private:
    int _rx;//pin of rx pin
    int _tx;//pin of tx pin
};

class BMX055
{
	public:
		uint8_t beginAcc(char range);
		uint8_t beginGyro(void);
		uint8_t beginMagn(void);
		void getAcceleration(float *x, float *y, float *z, float *accTotal);
		float getAccelerationX();
		float getAccelerationY();
		float getAccelerationZ();
		float getAccelerationTotal();
		void getMagnet(int *x, int *y, int *z);
		void getRotation(int *x, int *y, int *z);
	private:
		unsigned int _data[6];
		char _range;
		float accRange;
};

class GPS
{
	public:
		void begin();
		float getLatitude();
		float getLongitude();
		float getAltitude();
		float getSpeed();
		float getHdop();
		float getDate();
		float getTime();
	private:
		TinyGPSPlus* gps;
		void getGPS();
		float lat = 0.0;
		float lng = 0.0;
		float alt = 0.0;
		float speed = 0.0;
		float hdop = 0.0;
		float time = 0.0;
		float date = 0.0;
};

class Button

{
	public: 
	Button(int pin);
		void begin();
		bool getSwitch();
		bool isPressed();
		bool wasPressed();
		

	private:
	unsigned int _pin; //button pin
	bool _state = false;
	int _switchState = HIGH;
	int _wasPressed = LOW;
	int previous = LOW;
	
};

class Microphone

{
	public: 
	Microphone (int pin);
		void begin();
		float getValue();

	private:
	unsigned int _pin;
	unsigned int micValue;
	const int sampleTime = 100;
};


/***************************************************************************
  This is a library for the BMP280 pressure sensor

  Designed specifically to work with the Adafruit BMP280 Breakout
  ----> http://www.adafruit.com/products/2651

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

// BMP280 calibration registers
enum
{
	BMP280_REGISTER_DIG_T1              = 0x88,
	BMP280_REGISTER_DIG_T2              = 0x8A,
	BMP280_REGISTER_DIG_T3              = 0x8C,

	BMP280_REGISTER_DIG_P1              = 0x8E,
	BMP280_REGISTER_DIG_P2              = 0x90,
	BMP280_REGISTER_DIG_P3              = 0x92,
	BMP280_REGISTER_DIG_P4              = 0x94,
	BMP280_REGISTER_DIG_P5              = 0x96,
	BMP280_REGISTER_DIG_P6              = 0x98,
	BMP280_REGISTER_DIG_P7              = 0x9A,
	BMP280_REGISTER_DIG_P8              = 0x9C,
	BMP280_REGISTER_DIG_P9              = 0x9E,

	BMP280_REGISTER_CHIPID             = 0xD0,
	BMP280_REGISTER_VERSION            = 0xD1,
	BMP280_REGISTER_SOFTRESET          = 0xE0,

	BMP280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

	BMP280_REGISTER_CONTROL            = 0xF4,
	BMP280_REGISTER_CONFIG             = 0xF5,
	BMP280_REGISTER_PRESSUREDATA       = 0xF7,
	BMP280_REGISTER_TEMPDATA           = 0xFA,
};

//BMP280 calibration data
typedef struct
{
	uint16_t dig_T1;
	int16_t  dig_T2;
	int16_t  dig_T3;

	uint16_t dig_P1;
	int16_t  dig_P2;
	int16_t  dig_P3;
	int16_t  dig_P4;
	int16_t  dig_P5;
	int16_t  dig_P6;
	int16_t  dig_P7;
	int16_t  dig_P8;
	int16_t  dig_P9;

	uint8_t  dig_H1;
	int16_t  dig_H2;
	uint8_t  dig_H3;
	int16_t  dig_H4;
	int16_t  dig_H5;
	int8_t   dig_H6;
} bmp280_calib_data;

class BMP280
{
public:
	//BMP280();
	bool  begin();
	float getTemperature(void);
	float getPressure(void);
	float getAltitude(float seaLevelhPa = 1013.25);

private:
	void readCoefficients(void);
	uint8_t spixfer(uint8_t x);

	void      write8(byte reg, byte value);
	uint16_t  read16(byte reg);
	uint32_t  read24(byte reg);
	int16_t   readS16(byte reg);
	uint16_t  read16_LE(byte reg); // little endian
	int16_t   readS16_LE(byte reg); // little endian

	int32_t t_fine;

	int8_t _cs, _mosi, _miso, _sck;

	bmp280_calib_data _bmp280_calib;
};

//todo:
//BME680   [3]