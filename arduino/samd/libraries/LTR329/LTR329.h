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

#ifndef LTR329_h
#define LTR329_h

#include "Arduino.h"

#define LTR329_ADDR   0x29 // default address

// LTR329 register addresses
#define LTR329_CONTR         0x80
#define LTR329_MEAS_RATE     0x85
#define LTR329_PART_ID       0x86
#define LTR329_MANUFAC_ID    0x87
#define LTR329_DATA_CH1_0    0x88
#define LTR329_DATA_CH1_1    0x89
#define LTR329_DATA_CH0_0    0x8A
#define LTR329_DATA_CH0_1    0x8B
#define LTR329_STATUS		 0x8C

class LTR329 {
	public:
		LTR329(void);
			// LTR329 object
			
		boolean begin(void);
			// Initialize LTR329 library with default address (0x29)
			// Always returns true
		
		boolean setPowerUp(void);
			// Turn on LTR329, begin integration
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() below)

		boolean setPowerDown(void);
			// Turn off LTR329
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() below)

		boolean setControl(byte gain, boolean reset, boolean mode);
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
			
		boolean getControl(byte &gain, boolean reset, boolean mode);
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
			
			
		boolean setMeasurementRate(byte integrationTime, byte measurementRate);
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
			
		boolean getMeasurementRate(byte &integrationTime, byte &measurementRate);
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

		boolean getPartID(byte &partID);
			// Gets the part number ID and revision ID of the chip
			// Default value is 0x0A
			// part number ID = 0x0A (default)
			// Revision ID = 0x00
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() below)
		
		boolean getManufacID(byte &manufacID);
			// Gets the Manufacturers ID
			// Default value is 0x05
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() below)
		
		boolean getData(unsigned int &CH0, unsigned int &CH1);
			// Gets the 16-bit channel 0 and channel 1 data
			// Default value of both channels is 0x00
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() below)
		
		boolean getStatus(boolean valid, byte &gain, boolean intrStatus, boolean dataStatus);
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
					
		boolean getLux(byte gain, byte integrationTime, unsigned int CH0, unsigned int CH1, uint &lux);
			// Convert raw data to lux
			// gain: 0 (1X) or 7 (96X), see getControl()
			// integrationTime: integration time in ms, from getMeasurementRate()
			// CH0, CH1: results from getData()
			// lux will be set to resulting lux calculation
			// returns true (1) if calculation was successful
			// returns false (0) AND lux = 0.0 IF EITHER SENSOR WAS SATURATED (0XFFFF)
		
		byte getError(void);
			// If any library command fails, you can retrieve an extended
			// error code using this command. Errors are from the wire library: 
			// 0 = Success
			// 1 = Data too long to fit in transmit buffer
			// 2 = Received NACK on transmit of address
			// 3 = Received NACK on transmit of data
			// 4 = Other error

	private:

		boolean readByte(byte address, byte &value);
			// Reads a byte from a LTR329 address
			// Address: LTR329 address (0 to 15)
			// Value will be set to stored byte
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() above)
	
		boolean writeByte(byte address, byte value);
			// Write a byte to a LTR329 address
			// Address: LTR329 address (0 to 15)
			// Value: byte to write to address
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() above)

		boolean readUInt(byte address, unsigned int &value);
			// Reads an unsigned integer (16 bits) from a LTR329 address (low byte first)
			// Address: LTR329 address (0 to 15), low byte first
			// Value will be set to stored unsigned integer
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() above)

		boolean writeUInt(byte address, unsigned int value);
			// Write an unsigned integer (16 bits) to a LTR329 address (low byte first)
			// Address: LTR329 address (0 to 15), low byte first
			// Value: unsigned int to write to address
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() above)
			
		byte _i2c_address;
		
		byte _error;
};

#endif