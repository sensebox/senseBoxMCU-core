/*SenseBoxMCU.cpp
 * Library for easy usage of senseBox MCU
 * Created: 2018/04/10
 * last Modified: 2019/04/10 10:12:03
 * senseBox @ Institute for Geoinformatics WWU Münster
 */

#include "SenseBoxMCU.h"

Bee::Bee(){}

uint8_t Bee::connectToWifi(char* ssid, char* password)
{
	nwid = ssid;
	pw = password;
	if (!Serial)
	{
		Serial.begin(9600); //check if already connected
		delay(1000);
	}
	
	if (WiFi.status() == WL_NO_SHIELD) {
		Serial.println("WiFi Bee not present.");
		while (true);
	}

	while (status != WL_CONNECTED) {
		Serial.print("Attempting to connect to SSID: ");
		delay(100);
		Serial.println(ssid);
		status = WiFi.begin(ssid, password);

		delay(5000);
	}
	Serial.println("Successfully connected to your WiFi.");
	this->storeIpAddress();

	return status;
}

void Bee::startAP(char* ssid){
	nwid = ssid;
  senseBoxIO.powerXB1(false); 
  delay(250);
  senseBoxIO.powerXB1(true);
  if(WiFi.status() == WL_NO_SHIELD)
  {
    senseBoxIO.statusRed(); 
    WiFi.end();
    senseBoxIO.powerXB1(false);
    return; 
  }
  int status = WiFi.beginAP(nwid);
  if(status != WL_AP_LISTENING)
  {
    senseBoxIO.statusRed();
    WiFi.end();
    senseBoxIO.powerXB1(false);
    return;
  }	
}

char* Bee::getSsid()
{
	return this->nwid;
}

char* Bee::getPassword()
{
	return this->pw;
}

char* Bee::getIpAddress()
{
    return this->ip;
}

 void Bee::storeIpAddress()
{
    IPAddress ip = WiFi.localIP();
    sprintf(this->ip, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
}

OpenSenseMap::OpenSenseMap(const char* boxId, Bee* bee)
{
	senseBoxID = boxId;
	client = new WiFiClient;
	xbee = bee;
}

void OpenSenseMap::uploadMeasurement(float measurement, char* sensorID)
{ 
	if (WiFi.status() != WL_CONNECTED) {
		WiFi.disconnect();
		delay(1000); // wait 1s
		WiFi.begin(xbee->getSsid(), xbee->getPassword());
		delay(5000); // wait 5s
	}
	// close any connection before send a new request.
	// This will free the socket on the WiFi shield
	if (client->connected()) {
		client->stop();
		delay(1000);
	}
	// prepare data. json must look like: {"value":"12.5"} 
	char obs[10]; 
	snprintf(obs, sizeof(obs), "%f", measurement); //http://forum.arduino.cc/index.php?topic=243660.0
	//dtostrf(measurement, 5, 2, obs);
	String value = "{\"value\":"; 
	value += obs; 
	value += "}"; 
	Serial.println(value); 
	Serial.println(value.length());
	Serial.println(senseBoxID);
	Serial.println(sensorID);
	// post observation to: http://opensensemap.org:80/boxes/boxId/sensorId
	Serial.print("connecting...");
	if (client->connect(server, port)) 
	{
		Serial.println("connected"); 
		// Make a HTTP Post request: 
		client->print("POST /boxes/"); 
		client->print(senseBoxID);
		client->print("/"); 
		client->print(sensorID); 
		client->println(" HTTP/1.1"); 
		client->println("Host: ingress.opensensemap.org"); 
		client->println("Content-Type: application/json"); 
		client->println("Connection: close");  
		client->print("Content-Length: "); 
		client->println(value.length()); 
		client->println(); 
		client->print(value); 
		client->println();
	}
	else 
	{
		Serial.println("Connection failed!");
		return;
	}
	Serial.println();
	delay(1000);
	while (client->available()) {
		char c = client->read();
		Serial.write(c);
		// if the server's disconnected, stop the client:
		if (!client->connected()) {
			Serial.println();
			Serial.println("disconnecting from server.");
			client->stop();
		break;
		}
	}
	Serial.flush();
}

void OpenSenseMap::uploadMobileMeasurement(float measurement, char* sensorID, float lat, float lng)
{ 
	if (WiFi.status() != WL_CONNECTED) {
		WiFi.disconnect();
		delay(1000); // wait 1s
		WiFi.begin(xbee->getSsid(), xbee->getPassword());
		delay(5000); // wait 5s
	}
	// close any connection before send a new request.
	// This will free the socket on the WiFi shield
	if (client->connected()) {
		client->stop();
		delay(1000);
	}
	// prepare data. json must look like: {"value":"12.5"} 
	char obs[10];
	char clat[10];
	char clng[10];
	snprintf(obs, sizeof(obs), "%f", measurement);
	snprintf(clat, sizeof(obs), "%f", lat);
	snprintf(clng, sizeof(obs), "%f", lng); //http://forum.arduino.cc/index.php?topic=243660.0
	//dtostrf(measurement, 5, 2, obs);
	String value = "{\"value\":"; 
	value += obs;
	value +=  ",";
	value += "\"location\":[";
	value += clng;
	value += ",";
	value += clat;
	value += "]}"; 
	Serial.println(value); 
	Serial.println(value.length());
	Serial.println(senseBoxID);
	Serial.println(sensorID);
	// post observation to: http://opensensemap.org:80/boxes/boxId/sensorId
	Serial.print("connecting...");
	if (client->connect(server, port)) 
	{
		Serial.println("connected"); 
		// Make a HTTP Post request: 
		client->print("POST /boxes/"); 
		client->print(senseBoxID);
		client->print("/"); 
		client->print(sensorID); 
		client->println(" HTTP/1.1"); 
		client->println("Host: ingress.opensensemap.org"); 
		client->println("Content-Type: application/json"); 
		client->println("Connection: close");  
		client->print("Content-Length: "); 
		client->println(value.length()); 
		client->println(); 
		client->print(value); 
		client->println();
	}
	else 
	{
		Serial.println("Connection failed!");
		return;
	}
	Serial.println();
	delay(1000);
	while (client->available()) {
		char c = client->read();
		Serial.write(c);
		// if the server's disconnected, stop the client:
		if (!client->connected()) {
			Serial.println();
			Serial.println("disconnecting from server.");
			client->stop();
		break;
		}
	}
	Serial.flush();
}

void OpenSenseMap::setUploadInterval(unsigned int time)
{
	uploadInterval = time;
}

float SDS011::getPm10()
{
	this->read(&pm25, &pm10);
	return pm10;
}

float SDS011::getPm25()
{
	this->read(&pm25, &pm10);
	return pm25;
}

SDS011::SDS011(Stream& serial):sds_data(serial) {}

// --------------------------------------------------------
// SDS011:read
// --------------------------------------------------------
int SDS011::read(float *p25, float *p10) {
	byte buffer;
	int value;
	int len = 0;
	int pm10_serial = 0;
	int pm25_serial = 0;
	int checksum_is;
	int checksum_ok = 0;
	int error = 1;
	while ((sds_data.available() > 0) && (sds_data.available() >= (10-len))) {
		buffer = sds_data.read();
		value = int(buffer);
		switch (len) {
			case (0): if (value != 170) { len = -1; }; break;
			case (1): if (value != 192) { len = -1; }; break;
			case (2): pm25_serial = value; checksum_is = value; break;
			case (3): pm25_serial += (value << 8); checksum_is += value; break;
			case (4): pm10_serial = value; checksum_is += value; break;
			case (5): pm10_serial += (value << 8); checksum_is += value; break;
			case (6): checksum_is += value; break;
			case (7): checksum_is += value; break;
			case (8): if (value == (checksum_is % 256)) { checksum_ok = 1; } else { len = -1; }; break;
			case (9): if (value != 171) { len = -1; }; break;
		}
		len++;
		if (len == 10 && checksum_ok == 1) {
			*p10 = (float)pm10_serial/10.0;
			*p25 = (float)pm25_serial/10.0;
			len = 0; checksum_ok = 0; pm10_serial = 0.0; pm25_serial = 0.0; checksum_is = 0;
			error = 0;
		}
		yield();
	}
	return error;
}

uint8_t HDC1080::begin(){
  delay(20);
  Wire.begin();
  //Write to configuration register
  Wire.beginTransmission(HDC1080_ADDR); // I2C Adress of HDC1080
  Wire.write(0x02); // Point to configuration register on 0x02 
  Wire.write(0x90); // Configuration 1 0 0 1 0 0 0 0 0x00
  Wire.write(0x00); // 
  
  return Wire.endTransmission();
}

double HDC1080::getTemperature(){
	uint8_t Byte[4];
	uint16_t temp;

	Wire.beginTransmission(HDC1080_ADDR);
	Wire.write(0x00); //Point to temperature register
	Wire.endTransmission();

	delay(20); //conversion time

	Wire.requestFrom(HDC1080_ADDR, 4); //Request 4 bytes of data

	//store 4 bytes of data in the array
	if(4 <= Wire.available())
	{
		Byte[0] = Wire.read();
		Byte[1] = Wire.read();
		Byte[3] = Wire.read();
		Byte[4] = Wire.read();

		temp = (((unsigned int)Byte[0] <<8 | Byte[1]));
		return (double)(temp)/(65536)*165-40;
	}
	else return 0;
}

double HDC1080::getHumidity(){
	uint8_t Byte[4];
	uint16_t humi;

	Wire.beginTransmission(HDC1080_ADDR);
	Wire.write(0x00); //Point to temperature register
	Wire.endTransmission();

	delay(20); //conversion time

	Wire.requestFrom(HDC1080_ADDR, 4); //Request 4 bytes of data

	//store 4 bytes of data in the array
	if(4 <= Wire.available())
	{
	Byte[0] = Wire.read();
	Byte[1] = Wire.read();
	Byte[3] = Wire.read();
	Byte[4] = Wire.read();

	humi = (((unsigned int) Byte[3] <<8 | Byte[4]));
	return (double)(humi)/(65536)*100;
	}
}

uint8_t VEML6070::begin(){
	delay(20);
	Wire.begin();
	Wire.beginTransmission(VEML6070_ADDR);
	Wire.write((VEML6070_INTEGRATION_TIME_1<<2) | 0x02);
	
	return Wire.endTransmission();
}

double VEML6070::getUvIntensity(){
	byte msb=0, lsb=0;
	uint16_t uv;

	Wire.requestFrom(VEML6070_ADDR+1, 1); //MSB
	delay(1);
	if(Wire.available())
		msb = Wire.read();

	Wire.requestFrom(VEML6070_ADDR+0, 1); //LSB
	delay(1);
	if(Wire.available())
		lsb = Wire.read();

	uv = (msb<<8) | lsb; //output in steps (16bit)
	return (double)(uv)*(5.625);
}

uint8_t TSL45315::begin(){
	Wire.begin();
	Wire.beginTransmission(TSL45315_ADDR);
	Wire.write(0x80|0x00); //0x00 control reg
	Wire.write(0x03); //power on
	Wire.endTransmission();
	delay(20);
	Serial.println("Config...");
	Wire.beginTransmission(TSL45315_ADDR);
	Wire.write(0x80|0x01); //0x01 config reg
	Wire.write(0x00); //M=1 IT=400ms
	// Wire.write(0x01); //M=2 IT=200ms
	// Wire.write(0x02); //M=4 IT=100ms
	return Wire.endTransmission();
}

unsigned long TSL45315::getIlluminance(){
	uint16_t l, h;
	uint32_t lux;

	Wire.beginTransmission(TSL45315_ADDR);
	Wire.write(0x80|0x04); //0x04 reg datalow
	Wire.endTransmission();
	Wire.requestFrom(TSL45315_ADDR, 2); //request 2 bytes
	l = Wire.read();
	h = Wire.read();
	while(Wire.available()){ Wire.read(); } //received more bytes?
	lux  = (h<<8) | (l<<0);
	lux *= 1; //M=1
	// lux *= 2; //M=2
	// lux *= 4; //M=4
	return (unsigned long)(lux);
}

uint8_t BMX055::beginAcc(char range){
	
	char _range = range;	//2g Range 0x03

	switch (range){
	case 0x03: 
	accRange = (2.0/2048.0);
	break;
	case 0x05:
	accRange = (4.0/2048.0);
	break;
	case 0x08:
	accRange = (8.0/2048.0);
	break;
	case 0x0C:
	accRange = (16.0/2048.0);
	break;
	}
	// Initialise I2C communication as MASTER
	Wire1.begin();

	// Start I2C Transmission
	Wire1.beginTransmission(BMX055_ACCL_ADDR);
	// Select PMU_Range register
	Wire1.write(0x0F);
	// Range = +/- 2g
	Wire1.write(range);
	// Stop I2C Transmission
	Wire1.endTransmission();

	// Start I2C Transmission
	Wire1.beginTransmission(BMX055_ACCL_ADDR);
	// Select PMU_BW register
	Wire1.write(0x10);
	// Bandwidth = 7.81 Hz
	Wire1.write(0x08);
	// Stop I2C Transmission
	Wire1.endTransmission();

	// Start I2C Transmission
	Wire1.beginTransmission(BMX055_ACCL_ADDR);
	// Select PMU_LPW register
	Wire1.write(0x11);
	// Normal mode, Sleep duration = 0.5ms
	Wire1.write(0x00);
	// Stop I2C Transmission on the device
	Wire1.endTransmission();

}

uint8_t BMX055::beginGyro (){


	// Start I2C Transmission
	Wire1.beginTransmission(BMX055_GYRO_ADDR);
	// Select Range register
	Wire1.write(0x0F);
	// Full scale = +/- 125 degree/s
	Wire1.write(0x04);
	// Stop I2C Transmission
	Wire1.endTransmission();

	// Start I2C Transmission
	Wire1.beginTransmission(BMX055_GYRO_ADDR);
	// Select Bandwidth register
	Wire1.write(0x10);
	// ODR = 100 Hz
	Wire1.write(0x07);
	// Stop I2C Transmission
	Wire1.endTransmission();

	// Start I2C Transmission
	Wire1.beginTransmission(BMX055_GYRO_ADDR);
	// Select LPM1 register
	Wire1.write(0x11);
	// Normal mode, Sleep duration = 2ms
	Wire1.write(0x00);
	// Stop I2C Transmission
	Wire1.endTransmission();

}

uint8_t BMX055::beginMagn(){

	// Start I2C Transmission
	Wire1.beginTransmission(BMX055_MAGN_ADDR);
	// Select Mag register
	Wire1.write(0x4B);
	// Soft reset
	Wire1.write(0x83);
	// Stop I2C Transmission
	Wire1.endTransmission();

	// Start I2C Transmission
	Wire1.beginTransmission(BMX055_MAGN_ADDR);
	// Select Mag register
	Wire1.write(0x4C);
	// Normal Mode, ODR = 10 Hz
	Wire1.write(0x00);
	// Stop I2C Transmission
	Wire1.endTransmission();

	// Start I2C Transmission
	Wire1.beginTransmission(BMX055_MAGN_ADDR);
	// Select Mag register
	Wire1.write(0x4E);
	// X, Y, Z-Axis enabled
	Wire1.write(0x84);
	// Stop I2C Transmission
	Wire1.endTransmission();

	// Start I2C Transmission
	Wire1.beginTransmission(BMX055_MAGN_ADDR);
	// Select Mag register
	Wire1.write(0x51);
	// No. of Repetitions for X-Y Axis = 9
	Wire1.write(0x04);
	// Stop I2C Transmission
	Wire1.endTransmission();

	// Start I2C Transmission
	Wire1.beginTransmission(BMX055_MAGN_ADDR);
	// Select Mag register
	Wire1.write(0x52);
	// No. of Repetitions for Z-Axis = 15
	Wire1.write(0x0F);
	// Stop I2C Transmission
	Wire1.endTransmission();
	delay(300);
}

void BMX055::getAcceleration(float *x, float *y, float *z, float *accTotal){

	for (int i = 0; i < 6; i++)
	{
		// Start I2C Transmission
		Wire1.beginTransmission(BMX055_ACCL_ADDR);
		// Select data register
		Wire1.write((2 + i));
		// Stop I2C Transmission
		Wire1.endTransmission();
		// Request 1 byte of data
		Wire1.requestFrom(BMX055_ACCL_ADDR, 1);
		// Read 6 bytes of data
		// xAccl lsb, xAccl msb, yAccl lsb, yAccl msb, zAccl lsb, zAccl msb
		if (Wire1.available() == 1) _data[i] = Wire1.read();
	}

	// Convert the data to 12-bits
	int xAccl = ((_data[1] * 256) + (_data[0] & 0xF0)) / 16;
	if (xAccl > 2047) xAccl -= 4096;
	*x = xAccl*accRange;

	int yAccl = ((_data[3] * 256) + (_data[2] & 0xF0)) / 16;
	if (yAccl > 2047) yAccl -= 4096;
	*y = yAccl*accRange;

	int zAccl = ((_data[5] * 256) + (_data[4] & 0xF0)) / 16;
	if (zAccl > 2047) zAccl -= 4096;
	*z = zAccl*accRange;

	*accTotal = 9.81 * sqrt((sq(*x)+sq(*y)+sq(*z)));
}

float BMX055::getAccelerationX(){
		for (int i = 0; i < 6; i++)
	{
		// Start I2C Transmission
		Wire1.beginTransmission(BMX055_ACCL_ADDR);
		// Select data register
		Wire1.write((2 + i));
		// Stop I2C Transmission
		Wire1.endTransmission();
		// Request 1 byte of data
		Wire1.requestFrom(BMX055_ACCL_ADDR, 1);
		// Read 6 bytes of data
		// xAccl lsb, xAccl msb, yAccl lsb, yAccl msb, zAccl lsb, zAccl msb
		if (Wire1.available() == 1) _data[i] = Wire1.read();
	}

	// Convert the data to 12-bits
	int xAccl = ((_data[1] * 256) + (_data[0] & 0xF0)) / 16;
	if (xAccl > 2047) xAccl -= 4096;
	float x = xAccl*accRange;
	return x;

	}

float BMX055::getAccelerationY(){
			for (int i = 0; i < 6; i++)
	{
		// Start I2C Transmission
		Wire1.beginTransmission(BMX055_ACCL_ADDR);
		// Select data register
		Wire1.write((2 + i));
		// Stop I2C Transmission
		Wire1.endTransmission();
		// Request 1 byte of data
		Wire1.requestFrom(BMX055_ACCL_ADDR, 1);
		// Read 6 bytes of data
		// xAccl lsb, xAccl msb, yAccl lsb, yAccl msb, zAccl lsb, zAccl msb
		if (Wire1.available() == 1) _data[i] = Wire1.read();
	}

	int yAccl = ((_data[3] * 256) + (_data[2] & 0xF0)) / 16;
	if (yAccl > 2047) yAccl -= 4096;
	float y = yAccl*accRange;	
	return y;
	}

float BMX055::getAccelerationZ(){
				for (int i = 0; i < 6; i++)
	{
		// Start I2C Transmission
		Wire1.beginTransmission(BMX055_ACCL_ADDR);
		// Select data register
		Wire1.write((2 + i));
		// Stop I2C Transmission
		Wire1.endTransmission();
		// Request 1 byte of data
		Wire1.requestFrom(BMX055_ACCL_ADDR, 1);
		// Read 6 bytes of data
		// xAccl lsb, xAccl msb, yAccl lsb, yAccl msb, zAccl lsb, zAccl msb
		if (Wire1.available() == 1) _data[i] = Wire1.read();
	}

	int zAccl = ((_data[5] * 256) + (_data[4] & 0xF0)) / 16;
	if (zAccl > 2047) zAccl -= 4096;
	float z = zAccl*accRange;
	return z;
}

float BMX055::getAccelerationTotal(){

	for (int i = 0; i < 6; i++)
	{
		// Start I2C Transmission
		Wire1.beginTransmission(BMX055_ACCL_ADDR);
		// Select data register
		Wire1.write((2 + i));
		// Stop I2C Transmission
		Wire1.endTransmission();
		// Request 1 byte of data
		Wire1.requestFrom(BMX055_ACCL_ADDR, 1);
		// Read 6 bytes of data
		// xAccl lsb, xAccl msb, yAccl lsb, yAccl msb, zAccl lsb, zAccl msb
		if (Wire1.available() == 1) _data[i] = Wire1.read();
	}

	// Convert the data to 12-bits
	int xAccl = ((_data[1] * 256) + (_data[0] & 0xF0)) / 16;
	if (xAccl > 2047) xAccl -= 4096;
	float x = xAccl*accRange;

	int yAccl = ((_data[3] * 256) + (_data[2] & 0xF0)) / 16;
	if (yAccl > 2047) yAccl -= 4096;
	float y = yAccl*accRange;

	int zAccl = ((_data[5] * 256) + (_data[4] & 0xF0)) / 16;
	if (zAccl > 2047) zAccl -= 4096;
	float z = zAccl*accRange;

	float accTotal = 9.81 * sqrt((sq(x)+sq(y)+sq(z)));
	return accTotal;
}

void BMX055::getMagnet(int *x, int *y, int *z){

	for (int i = 0; i < 6; i++)
	{
		// Start I2C Transmission
		Wire1.beginTransmission(BMX055_MAGN_ADDR);
		// Select data register
		Wire1.write((66 + i));
		// Stop I2C Transmission
		Wire1.endTransmission();
		// Request 1 byte of data
		Wire1.requestFrom(BMX055_MAGN_ADDR, 1);
		// Read 6 bytes of data
		// xMag lsb, xMag msb, yMag lsb, yMag msb, zMag lsb, zMag msb
		if (Wire1.available() == 1) _data[i] = Wire1.read();
	}

	// Convert the data
	int xMag = ((_data[1] * 256) + (_data[0] & 0xF8)) / 8;
	if (xMag > 4095) xMag -= 8192;
	*x = xMag;

	int yMag = ((_data[3] * 256) + (_data[2] & 0xF8)) / 8;
	if (yMag > 4095) yMag -= 8192;
	*y = yMag;

	int zMag = ((_data[5] * 256) + (_data[4] & 0xFE)) / 2;
	if (zMag > 16383) zMag -= 32768;
	*z = zMag;
}
void BMX055::getRotation(int *x, int *y, int *z){

	for (int i = 0; i < 6; i++)
	{
		// Start I2C Transmission
		Wire1.beginTransmission(BMX055_GYRO_ADDR);
		// Select data register
		Wire1.write((2 + i));
		// Stop I2C Transmission
		Wire1.endTransmission();
		// Request 1 byte of data
		Wire1.requestFrom(BMX055_GYRO_ADDR, 1);
		// Read 6 bytes of data
		// xGyro lsb, xGyro msb, yGyro lsb, yGyro msb, zGyro lsb, zGyro msb
		if (Wire1.available() == 1) _data[i] = Wire1.read();
	}

	// Convert the data
	int xGyro = (_data[1] * 256) + _data[0];
	if (xGyro > 32767) xGyro -= 65536;
	*x = xGyro;

	int yGyro = (_data[3] * 256) + _data[2];
	if (yGyro > 32767) yGyro -= 65536;
	*y = yGyro;

	int zGyro = (_data[5] * 256) + _data[4];
	if (zGyro > 32767) zGyro -= 65536;
	*z = zGyro;
}


Ultrasonic::Ultrasonic(int rx, int tx)
{
    _rx = rx;
    _tx = tx;
}
/*The measured distance from the range 0 to 400 Centimeters*/
long Ultrasonic::getDistance(void)
{
    pinMode(_rx, OUTPUT);
    digitalWrite(_rx, LOW);
    delayMicroseconds(2);
    digitalWrite(_rx, HIGH);
    delayMicroseconds(5);
    digitalWrite(_rx,LOW);
    pinMode(_tx,INPUT);
    long duration;
    duration = pulseIn(_tx,HIGH);
    long distance;
    distance = duration/58;
    return distance;
}

void GPS::begin()
{
	delay(20);
	Wire.begin();
	gps = new TinyGPSPlus;
}

float GPS::getLatitude()
{
	getGPS();
	return lat;
}

float GPS::getLongitude()
{
	getGPS();
	return lng;
}
float GPS::getAltitude()
{
	getGPS();
	return alt;
}

float GPS::getSpeed()
{
	getGPS();
	return speed;
}

float GPS::getHdop()
{
	getGPS();
	return hdop;
}

float GPS::getDate()
{
	getGPS();
	return date;
}

float GPS::getTime()
{
	getGPS();
	return time;
}

void GPS::getGPS()
{
	Wire.requestFrom(0x42, 10);
	while(Wire.available())
	//while (Wire.available() > 0)
		if (gps->encode(Wire.read()))
				if(gps->location.isValid())
				{
					lat = gps->location.lat();
					lng = gps->location.lng();
					alt = gps->altitude.meters();
					speed = gps->speed.kmph();
					hdop = gps->hdop.hdop();
					time = gps->time.value();
					date = gps->date.value();
				}
}

/*  This is a library for the BMP280 pressure sensor

  Designed specifically to work with the Adafruit BMP280 Breakout
  ----> http://www.adafruit.com/products/2651

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
*/

bool BMP280::begin() {
	delay(20);
	Wire.begin();
	Wire.beginTransmission(118);

	Wire.begin();

	readCoefficients();
	write8(BMP280_REGISTER_CONTROL, 0x3F);
	return true;
}


/**************************************************************************/
/*!
    @brief  Writes an 8 bit value over I2C/SPI
*/
/**************************************************************************/
void BMP280::write8(byte reg, byte value)
{

    Wire.beginTransmission(BMP280_ADDR);
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)value);
    Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Reads a 16 bit value over I2C/SPI
*/
/**************************************************************************/
uint16_t BMP280::read16(byte reg)
{
  uint16_t value;
	    Wire.beginTransmission(BMP280_ADDR);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)BMP280_ADDR, (byte)2);
    value = (Wire.read() << 8) | Wire.read();
  return value;
}

uint16_t BMP280::read16_LE(byte reg) {
  uint16_t temp = read16(reg);
  return (temp >> 8) | (temp << 8);

}

/**************************************************************************/
/*!
    @brief  Reads a signed 16 bit value over I2C/SPI
*/
/**************************************************************************/
int16_t BMP280::readS16(byte reg)
{
  return (int16_t)read16(reg);

}

int16_t BMP280::readS16_LE(byte reg)
{
  return (int16_t)read16_LE(reg);

}


/**************************************************************************/
/*!
    @brief  Reads a 24 bit value over I2C/SPI
*/
/**************************************************************************/
uint32_t BMP280::read24(byte reg)
{
  uint32_t value;
    Wire.beginTransmission(BMP280_ADDR);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom(BMP280_ADDR, (byte)3);
    value = Wire.read();
    value <<= 8;
    value |= Wire.read();
    value <<= 8;
    value |= Wire.read();
  return value;
}

/**************************************************************************/
/*!
    @brief  Reads the factory-set coefficients
*/
/**************************************************************************/
void BMP280::readCoefficients(void)
{
    _bmp280_calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
    _bmp280_calib.dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
    _bmp280_calib.dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);

    _bmp280_calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
    _bmp280_calib.dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
    _bmp280_calib.dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
    _bmp280_calib.dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
    _bmp280_calib.dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
    _bmp280_calib.dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
    _bmp280_calib.dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
    _bmp280_calib.dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
    _bmp280_calib.dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);
}

float BMP280::getTemperature(void)
{
  int32_t var1, var2;

  int32_t adc_T = read24(BMP280_REGISTER_TEMPDATA);
  adc_T >>= 4;

  var1  = ((((adc_T>>3) - ((int32_t)_bmp280_calib.dig_T1 <<1))) *
	   ((int32_t)_bmp280_calib.dig_T2)) >> 11;

  var2  = (((((adc_T>>4) - ((int32_t)_bmp280_calib.dig_T1)) *
	     ((adc_T>>4) - ((int32_t)_bmp280_calib.dig_T1))) >> 12) *
	   ((int32_t)_bmp280_calib.dig_T3)) >> 14;

  t_fine = var1 + var2;

  float T  = (t_fine * 5 + 128) >> 8;
  return T/100;
}

float BMP280::getPressure(void) {
  int64_t var1, var2, p;

  // Must be done first to get the t_fine variable set up
  getTemperature();

  int32_t adc_P = read24(BMP280_REGISTER_PRESSUREDATA);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)_bmp280_calib.dig_P6;
  var2 = var2 + ((var1*(int64_t)_bmp280_calib.dig_P5)<<17);
  var2 = var2 + (((int64_t)_bmp280_calib.dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)_bmp280_calib.dig_P3)>>8) +
    ((var1 * (int64_t)_bmp280_calib.dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_bmp280_calib.dig_P1)>>33;

  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((int64_t)_bmp280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)_bmp280_calib.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)_bmp280_calib.dig_P7)<<4);
  return (float)p/256;
}

float BMP280::getAltitude(float seaLevelhPa) {
  float altitude;
  float pressure = getPressure(); // in Si units for Pascal
  pressure /= 100;
  altitude = 44330 * (1.0 - pow(pressure / seaLevelhPa, 0.1903));
  return altitude;
}

/* Button Library*/

Button::Button(int pin)
{
    _pin = pin;
}

void Button::begin(){
	pinMode(_pin, INPUT_PULLUP);
	_state = digitalRead(_pin);
}

bool Button::isPressed(){
	bool buttonState = 0;       
	bool state = LOW;
	buttonState = digitalRead(_pin);
  	if (buttonState != _state) {
    	state = HIGH;
  	} else {
    	state = LOW;
  	}
  	return state;
}



bool Button::getSwitch(){
	int reading;
	long time = 0;         // the last time the output pin was toggled
	long debounce = 200;   // the debounce time, increase if the output flickers
	reading = digitalRead(_pin);

  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (_switchState == HIGH)
      _switchState = LOW;
    else
      _switchState = HIGH;

    time = millis();    
  }
  previous = reading;
  return _switchState;
}

bool Button::wasPressed(){
	int reading;
	long time = 0;         // the last time the output pin was toggled
	long debounce = 200;   // the debounce time, increase if the output flickers
	reading = digitalRead(_pin);

if (reading != previous && millis() - time > debounce) {
      if (reading == _state)
	  _wasPressed = HIGH;
	}
    else
      _wasPressed = LOW;

    time = millis();    
	previous = reading;
	delay(50);
  return _wasPressed;
}

Microphone::Microphone (int pin){
	_pin = pin;
}

void Microphone::begin (){

}

float Microphone::getValue(){
unsigned long start = millis();  // Start des Messintervalls
 unsigned int peakToPeak = 0;   // Abstand von maximalem zu minimalem Amplitudenausschlag
 unsigned int signalMax = 0;    
 unsigned int signalMin = 1023;

 // Sammle Daten für 100 Millisekunden
 while (millis() - start < sampleTime)
    {
    micValue = analogRead(_pin); // Messe den aktuellen Wert
        if (micValue < 1023)  // sortiere Fehlmessungen aus, deren Werte über dem max Wert 1024 liegen 
        {
            if (micValue > signalMax)
            {
            signalMax = micValue;  // speichere den maximal gemessenen Wert
            }
        else if (micValue < signalMin)
            {
            signalMin = micValue;  // speichere den minimal gemessenen Wert
            }
        }
    }
 peakToPeak = signalMax - signalMin;  // max - min = Abstand von maximalem zu minimalem Amplitudenausschlag
 double volts = (peakToPeak * 5.0) / 1023;  // wandle in Volt um
 return volts;
}


