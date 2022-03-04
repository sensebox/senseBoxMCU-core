#include "BMX055.h"

BMX055::BMX055(){}

uint8_t BMX055::beginAcc(char range)
{

	char _range = range; //2g Range 0x03

	switch (range)
	{
	case 0x03:
		accRange = (2.0 / 2048.0);
		break;
	case 0x05:
		accRange = (4.0 / 2048.0);
		break;
	case 0x08:
		accRange = (8.0 / 2048.0);
		break;
	case 0x0C:
		accRange = (16.0 / 2048.0);
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

uint8_t BMX055::beginGyro()
{

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

uint8_t BMX055::beginMagn()
{

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

void BMX055::getAcceleration(float *x, float *y, float *z, float *accTotal)
{

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
		if (Wire1.available() == 1)
			_data[i] = Wire1.read();
	}

	// Convert the data to 12-bits
	int xAccl = ((_data[1] * 256) + (_data[0] & 0xF0)) / 16;
	if (xAccl > 2047)
		xAccl -= 4096;
	*x = xAccl * accRange;

	int yAccl = ((_data[3] * 256) + (_data[2] & 0xF0)) / 16;
	if (yAccl > 2047)
		yAccl -= 4096;
	*y = yAccl * accRange;

	int zAccl = ((_data[5] * 256) + (_data[4] & 0xF0)) / 16;
	if (zAccl > 2047)
		zAccl -= 4096;
	*z = zAccl * accRange;

	*accTotal = 9.81 * sqrt((sq(*x) + sq(*y) + sq(*z)));
}

float BMX055::getAccelerationX()
{
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
		if (Wire1.available() == 1)
			_data[i] = Wire1.read();
	}

	// Convert the data to 12-bits
	int xAccl = ((_data[1] * 256) + (_data[0] & 0xF0)) / 16;
	if (xAccl > 2047)
		xAccl -= 4096;
	float x = xAccl * accRange;
	return x;
}

float BMX055::getAccelerationY()
{
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
		if (Wire1.available() == 1)
			_data[i] = Wire1.read();
	}

	int yAccl = ((_data[3] * 256) + (_data[2] & 0xF0)) / 16;
	if (yAccl > 2047)
		yAccl -= 4096;
	float y = yAccl * accRange;
	return y;
}

float BMX055::getAccelerationZ()
{
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
		if (Wire1.available() == 1)
			_data[i] = Wire1.read();
	}

	int zAccl = ((_data[5] * 256) + (_data[4] & 0xF0)) / 16;
	if (zAccl > 2047)
		zAccl -= 4096;
	float z = zAccl * accRange;
	return z;
}

float BMX055::getAccelerationTotal()
{

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
		if (Wire1.available() == 1)
			_data[i] = Wire1.read();
	}

	// Convert the data to 12-bits
	int xAccl = ((_data[1] * 256) + (_data[0] & 0xF0)) / 16;
	if (xAccl > 2047)
		xAccl -= 4096;
	float x = xAccl * accRange;

	int yAccl = ((_data[3] * 256) + (_data[2] & 0xF0)) / 16;
	if (yAccl > 2047)
		yAccl -= 4096;
	float y = yAccl * accRange;

	int zAccl = ((_data[5] * 256) + (_data[4] & 0xF0)) / 16;
	if (zAccl > 2047)
		zAccl -= 4096;
	float z = zAccl * accRange;

	float accTotal = 9.81 * sqrt((sq(x) + sq(y) + sq(z)));
	return accTotal;
}

void BMX055::getMagnet(int *x, int *y, int *z)
{

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
		if (Wire1.available() == 1)
			_data[i] = Wire1.read();
	}

	// Convert the data
	int xMag = ((_data[1] * 256) + (_data[0] & 0xF8)) / 8;
	if (xMag > 4095)
		xMag -= 8192;
	*x = xMag;

	int yMag = ((_data[3] * 256) + (_data[2] & 0xF8)) / 8;
	if (yMag > 4095)
		yMag -= 8192;
	*y = yMag;

	int zMag = ((_data[5] * 256) + (_data[4] & 0xFE)) / 2;
	if (zMag > 16383)
		zMag -= 32768;
	*z = zMag;
}
void BMX055::getRotation(int *x, int *y, int *z)
{

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
		if (Wire1.available() == 1)
			_data[i] = Wire1.read();
	}

	// Convert the data
	int xGyro = (_data[1] * 256) + _data[0];
	if (xGyro > 32767)
		xGyro -= 65536;
	*x = xGyro;

	int yGyro = (_data[3] * 256) + _data[2];
	if (yGyro > 32767)
		yGyro -= 65536;
	*y = yGyro;

	int zGyro = (_data[5] * 256) + _data[4];
	if (zGyro > 32767)
		zGyro -= 65536;
	*z = zGyro;
}