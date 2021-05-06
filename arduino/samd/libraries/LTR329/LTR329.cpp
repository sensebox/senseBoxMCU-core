/*
	LTR329 sensor lib
	Based on LTR303 illumination sensor library for Arduino
	by Lovelesh, thingTronics
	
The MIT License (MIT)

Copyright (c) 2015 thingTronics Limited

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

version 0.1
*/

#include <LTR329.h>
#include <Wire.h>

LTR329::LTR329(void) {
	// LTR329 object
}

boolean LTR329::begin(void) {
	// Initialize LTR329 library with default address (0x39)
	// Always returns true

	_i2c_address = LTR329_ADDR;
	Wire.begin();
	return(true);
}

boolean LTR329::setPowerUp(void) {
	// Turn on LTR329, begin integrations
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)

	// Write 0x03 (reset = 1 & mode = 1) to command byte (power on)
	return(writeByte(LTR329_CONTR,0x01));
}

boolean LTR329::setPowerDown(void) {
	// Turn off LTR329
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)

	// Clear command byte (reset = 0 & mode = 0)(power off)
	return(writeByte(LTR329_CONTR,0x00));
}

boolean LTR329::setControl(byte gain, boolean reset = false, boolean mode = false) {
	// Sets the gain, SW reset and mode of LTR329
	// Default value is 0x00
	// If gain = 0, device is set to 1X gain (default)
	// If gain = 1, device is set to 2X gain
	// If gain = 2, device is set to 4X gain
	// If gain = 3, device is set to 8X gain
	// If gain = 4, invalid
	// If gain = 5, invalid
	// If gain = 6, device is set to 48X gain
	// If gain = 7, device is set to 96X gain
	//----------------------------------------
	// If reset = false(0), initial start-up procedure not started (default)
	// If reset = true(1), initial start-up procedure started
	//----------------------------------------
	// If mode = false(0), stand-by mode (default)
	// If mode = true(1), active mode
	
	byte control = 0x00;
	
	// sanity check for gain
	if (gain > 3 && gain < 6) {
		gain = 0x00;
	}
	else if(gain >= 7) {
		gain = 0x00;
	}
	
	// control byte logic
	control |= gain << 2;
	if(reset) {
		control |= 0x02;
	}
	
	if(mode) {
		control |= 0x01;
	}
	
	return(writeByte(LTR329_CONTR,control));
}			
			
boolean LTR329::getControl(byte &gain, boolean reset, boolean mode) {
	// Gets the control register values
	// Default value is 0x00
	// If gain = 0, device is set to 1X gain (default)
	// If gain = 1, device is set to 2X gain
	// If gain = 2, device is set to 4X gain
	// If gain = 3, device is set to 8X gain
	// If gain = 4, invalid
	// If gain = 5, invalid
	// If gain = 6, device is set to 48X gain
	// If gain = 7, device is set to 96X gain
	//----------------------------------------
	// If reset = false(0), initial start-up procedure not started (default)
	// If reset = true(1), initial start-up procedure started
	//----------------------------------------
	// If mode = false(0), stand-by mode (default)
	// If mode = true(1), active mode
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)			
	
	byte control;
	
	// Reading the control byte
	if(readByte(LTR329_CONTR, control)) {
		// Extract gain
		gain = (control & 0x1C) >> 2;
		
		// Extract reset
		reset = (control & 0x02) ? true : false; 
		
		// Extract mode
		mode = (control & 0x01) ? true : false;
		
		// return if successful
		return(true);
	}
	return(false);
}

boolean LTR329::setMeasurementRate(byte integrationTime, byte measurementRate = 3) {
	// Sets the integration time and measurement rate of the sensor
	// integrationTime is the measurement time for each ALs cycle
	// measurementRate is the interval between DATA_REGISTERS update
	// measurementRate must be set to be equal or greater than integrationTime
	// Default value is 0x03
	// If integrationTime = 0, integrationTime will be 100ms (default)
	// If integrationTime = 1, integrationTime will be 50ms
	// If integrationTime = 2, integrationTime will be 200ms
	// If integrationTime = 3, integrationTime will be 400ms
	// If integrationTime = 4, integrationTime will be 150ms
	// If integrationTime = 5, integrationTime will be 250ms
	// If integrationTime = 6, integrationTime will be 300ms
	// If integrationTime = 7, integrationTime will be 350ms
	//------------------------------------------------------
	// If measurementRate = 0, measurementRate will be 50ms
	// If measurementRate = 1, measurementRate will be 100ms
	// If measurementRate = 2, measurementRate will be 200ms
	// If measurementRate = 3, measurementRate will be 500ms (default)
	// If measurementRate = 4, measurementRate will be 1000ms
	// If measurementRate = 5, measurementRate will be 2000ms
	// If measurementRate = 6, measurementRate will be 2000ms
	// If measurementRate = 7, measurementRate will be 2000ms
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	byte measurement = 0x00;
	
	// Perform sanity checks
	if(integrationTime >= 0x07) {
		integrationTime = 0x00;
	}
	
	if(measurementRate >= 0x07) {
		measurementRate = 0x00;
	}
	
	measurement |= integrationTime << 3;
	measurement |= measurementRate;
	
	return(writeByte(LTR329_MEAS_RATE, measurement));
}

boolean LTR329::getMeasurementRate(byte &integrationTime, byte &measurementRate) {
	// Gets the value of Measurement Rate register
	// Default value is 0x03
	// If integrationTime = 0, integrationTime will be 100ms (default)
	// If integrationTime = 1, integrationTime will be 50ms
	// If integrationTime = 2, integrationTime will be 200ms
	// If integrationTime = 3, integrationTime will be 400ms
	// If integrationTime = 4, integrationTime will be 150ms
	// If integrationTime = 5, integrationTime will be 250ms
	// If integrationTime = 6, integrationTime will be 300ms
	// If integrationTime = 7, integrationTime will be 350ms
	//------------------------------------------------------
	// If measurementRate = 0, measurementRate will be 50ms
	// If measurementRate = 1, measurementRate will be 100ms
	// If measurementRate = 2, measurementRate will be 200ms
	// If measurementRate = 3, measurementRate will be 500ms (default)
	// If measurementRate = 4, measurementRate will be 1000ms
	// If measurementRate = 5, measurementRate will be 2000ms
	// If measurementRate = 6, measurementRate will be 2000ms
	// If measurementRate = 7, measurementRate will be 2000ms
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
			
	byte measurement = 0x00;
	
	// Reading the measurement byte
	if(readByte(LTR329_MEAS_RATE, measurement)) {
		// Extract integration Time
		integrationTime = (measurement & 0x38) >> 3;
		
		// Extract measurement Rate
		measurementRate = measurement & 0x07; 
		
		// return if successful
		return(true);
	}
	return(false);		
}

boolean LTR329::getPartID(byte &partID) {
	// Gets the part number ID and revision ID of the chip
	// Default value is 0x0A
	// part number ID = 0x0A (default)
	// Revision ID = 0x00
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
			
	return(readByte(LTR329_PART_ID, partID));
}

boolean LTR329::getManufacID(byte &manufacID) {
	// Gets the Manufacturers ID
	// Default value is 0x05
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	return(readByte(LTR329_MANUFAC_ID, manufacID));
}

boolean LTR329::getData(unsigned int &CH0, unsigned int &CH1) {
	// Gets the 16-bit channel 0 and channel 1 data
	// Default value of both channels is 0x00
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	return(readUInt(LTR329_DATA_CH0_0,CH0) && readUInt(LTR329_DATA_CH1_0,CH1));
}

boolean LTR329::getStatus(boolean valid, byte &gain, boolean intrStatus, boolean dataStatus) {
	// Gets the status information of LTR329
	// Default value is 0x00
	// If valid = false(0), Sensor data is valid (default)
	// If valid = true(1), Sensor data is invalid
	//--------------------------------------------
	// If gain = 0, device is set to 1X gain (default)
	// If gain = 1, device is set to 2X gain
	// If gain = 2, device is set to 4X gain
	// If gain = 3, device is set to 8X gain
	// If gain = 4, invalid
	// If gain = 5, invalid
	// If gain = 6, device is set to 48X gain
	// If gain = 7, device is set to 96X gain
	//---------------------------------------------
	// If intrStatus = false(0), INTR in inactive (default)
	// If intrStatus = true(1), INTR in active
	//---------------------------------------------
	// If dataStatus = false(0), OLD data (already read) (default)
	// If dataStatus = true(1), NEW data
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	byte status = 0x00;
	
	// Reading the status byte
	if(readByte(LTR329_STATUS, status)) {
		// Extract validity
		valid = (status & 0x80) ? true : false;
	
		// Extract gain
		gain = (status & 0x70) >> 4;
	
		// Extract interrupt status
		intrStatus = (status & 0x08) ? true : false;
	
		// Extract data status
		dataStatus = (status & 0x04) ? true : false;
		
		// return if successful
		return(true);
	}
	return(false);
}

// Get the right lux algorithm
boolean LTR329::getLux(byte gain, byte integrationTime, unsigned int CH0, unsigned int CH1, uint &lux) {
	// Convert raw data to lux
	// gain: 0 (1X) or 7 (96X), see getControl()
	// integrationTime: integration time in ms, from getMeasurementRate()
	// CH0, CH1: results from getData()
	// lux will be set to resulting lux calculation
	// returns true (1) if calculation was successful
	// returns false (0) AND lux = 0 IF EITHER SENSOR WAS SATURATED (0XFFFF)

	double ratio, d0, d1;
	uint ALS_GAIN[8] = {1,2,4,8,1,1,48,96};
	float ALS_INT[8] = {1.0,0.5,2.0,4.0,1.5,2.5,3.0,3.5};
	// Determine if either sensor saturated (0xFFFF)
	// If so, abandon ship (calculation will not be accurate)
	if ((CH0 == 0xFFFF) || (CH1 == 0xFFFF)) {
		lux = 0.0;
		return(false);
	}

	// Convert from unsigned integer to floating point
	d0 = CH0; d1 = CH1;

	// We will need the ratio for subsequent calculations
	ratio = d1 / (d0+d1);

	// Determine lux per datasheet equations:
	if (ratio < 0.45) {
		lux = ((1.7743 * d0) + (1.1059 * d1)) / ALS_GAIN[gain] / ALS_INT[integrationTime];
		return(true);
	}

	else if (ratio < 0.64) {
		lux = ((4.2785 * d0) - (1.9548 * d1)) / ALS_GAIN[gain] / ALS_INT[integrationTime];
		return(true);
	}

	else if (ratio < 0.85) {
		lux = ((0.5926 * d0) + (0.1185 * d1)) / ALS_GAIN[gain] / ALS_INT[integrationTime];
		return(true);
	}

	else lux = 0;

	return(true);
}

byte LTR329::getError(void) {
	// If any library command fails, you can retrieve an extended
	// error code using this command. Errors are from the wire library: 
	// 0 = Success
	// 1 = Data too long to fit in transmit buffer
	// 2 = Received NACK on transmit of address
	// 3 = Received NACK on transmit of data
	// 4 = Other error

	return(_error);
}

// Private functions:

boolean LTR329::readByte(byte address, byte &value) {
	// Reads a byte from a LTR329 address
	// Address: LTR329 address (0 to 15)
	// Value will be set to stored byte
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)

	// Check if sensor present for read
	Wire.beginTransmission(_i2c_address);
	Wire.write(address);
	_error = Wire.endTransmission();

	// Read requested byte
	if (_error == 0)
	{
		Wire.requestFrom(_i2c_address,1);
		if (Wire.available() == 1)
		{
			value = Wire.read();
			return(true);
		}
	}
	return(false);
}

boolean LTR329::writeByte(byte address, byte value) {
	// Write a byte to a LTR329 address
	// Address: LTR329 address (0 to 15)
	// Value: byte to write to address
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)

	Wire.beginTransmission(_i2c_address);
	Wire.write(address);
	// Write byte
	Wire.write(value);
	_error = Wire.endTransmission();
	if (_error == 0)
		return(true);

	return(false);
}

boolean LTR329::readUInt(byte address, unsigned int &value) {
	// Reads an unsigned integer (16 bits) from a LTR329 address (low byte first)
	// Address: LTR329 address (0 to 15), low byte first
	// Value will be set to stored unsigned integer
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)

	byte high, low;
	
	// Check if sensor present for read
	Wire.beginTransmission(_i2c_address);
	Wire.write(address);
	_error = Wire.endTransmission();

	// Read two bytes (low and high)
	if (_error == 0)
	{
		Wire.requestFrom(_i2c_address,2);
		if (Wire.available() == 2)
		{
			low = Wire.read();
			high = Wire.read();
			// Combine bytes into unsigned int
			value = word(high,low);
			return(true);
		}
	}	
	return(false);
}

boolean LTR329::writeUInt(byte address, unsigned int value) {
	// Write an unsigned integer (16 bits) to a LTR329 address (low byte first)
	// Address: LTR329 address (0 to 15), low byte first
	// Value: unsigned int to write to address
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)

	// Split int into lower and upper bytes, write each byte
	if (writeByte(address,lowByte(value)) 
		&& writeByte(address + 1,highByte(value)))
		return(true);

	return(false);
}