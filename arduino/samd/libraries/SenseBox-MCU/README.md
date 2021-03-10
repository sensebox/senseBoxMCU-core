# senseBox MCU Library

This Library is for the [senseBox with MCU](https://sensebox.kaufen/product/sensebox-edu). The Library gives easy access to the following features/sensors:

- Temperature and Humidity [Sensor HDC1080](https://sensebox.kaufen/product/temperatur-luftfeuchte)
- Airpressure and Temperature Sensor [BMP280](https://sensebox.kaufen/product/luftdruck-temperatur)
- Light Visible + UV [TSL45315 and VEML6070](https://sensebox.kaufen/product/licht-sensor)
- Ultrasonic Sensor [HC SR04]
- SDS011 Fine Particular Sensor [SDS011](https://sensebox.kaufen/product/feinstaub-sds011)
- senseBox GPS connected via I2C [CAM-M8Q](https://sensebox.kaufen/product/gps) --> need [TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus)
- Wifi Connection via WifiBee [WINC1500](https://sensebox.kaufen/product/wifi-bee) 
- send Data to [openSenseMap](https://opensensemap.org)

## Usage

Install via Arduino Library Manager or copy it to your Arduino Library folder. Include to your Arduino Programm via ```#include "senseBoxMCU.h"```



## Classes and Functions
The Library provides to following classes and functions. Create a new instance of the different classes to use the Functions, for example:

```Arduino
#include "senseBoxMCU.h"

HDC1080 hdc;
```

Now you have acces to the functions:

```Arduino
hdc.getTemperature();
```

### Bee
```Arduino 
public:
	Bee();
		uint8_t connectToWifi(char* ssid, char* password);
		void startAP(char* ssid);
		char* getSsid();
		char* getPassword();
		char* getIpAddress();
```		

### OpenSenseMap
```Arduino 
Classname: OpenSenseMap

functions public:
			OpenSenseMap(const char* boxId, Bee* bee, const char* host);
			void uploadMeasurement(float value, char* sensorID);
			void uploadMobileMeasurement(float value, char* sensorID, float lat, float lng);
			void setUploadInterval(unsigned int);
```

### SDS011
```Arduino 
Classname: SDS011

functions public:
			SDS011(Stream& serial);
			float getPm10(void);
			float getPm25(void);
```

### HDC1080
```Arduino 
Classname: HDC1080

functions public:
			uint8_t begin(void);
			double getTemperature(void);
			double getHumidity(void); 
```

### VEML6070
```Arduino 
Classname: TSL45315

functions public:
			uint8_t begin(void);
			double getUvIntensity(void);
```

### Lightsensor (support TSL45315/LTR329 and detect automatically)

```Arduino 
Classname: Lightsensor

public:
	void begin();
	unsigned long getIlluminance(void);

private:
	int sensortype = 0; // 0 for tsl | - 1 for liteon sensor
};
```

### TSL45315
```Arduino 
Classname: TSL45315

functions public:
			uint8_t begin(void);
			unsigned long getIlluminance(void); 
```

### Ultrasonic
```Arduino 
Classname: Ultrasonic

functions public:
			Ultrasonic(int rx, int tx);
        	long getDistance(void);
```

### BMP280
```Arduino 
Classname: BMP280

functions public:
			bool  begin();
			float getTemperature(void);
			float getPressure(void);
			float getAltitude(float seaLevelhPa = 1013.25);
```

### GPS
```Arduino 
Classname: GPS

functions public:
			void begin();
			float getLatitude();
			float getLongitude();
			float getAltitude();
			float getSpeed();
			float getHdop();
			float getDate();
			float getTime();
```

### Mic

```Arduino

Classname: Microphone

functions public: 
		Microphone (int pin);
		float getValue();
```

### BMX

```Arduino

Classname BMX055

functions	public:
		uint8_t beginAcc(char range);
		uint8_t beginGyro(void);
		uint8_t beginMagn(void);
		void getAcceleration(float *x, float *y, float *z, float *accTotal);
		void getMagnet(int *x, int *y, int *z);
		void getRotation(int *x, int *y, int *z);
```

### Button

```Arduino

Classname Button

functions public: 
		Button(int pin);
		void begin();
		bool getSwitch();
		bool isPressed();
		bool wasPressed();
```


