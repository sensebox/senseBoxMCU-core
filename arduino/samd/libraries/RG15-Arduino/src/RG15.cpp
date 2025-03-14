
#include "RG15.hpp"

#include "Arduino.h"

/**
 * @brief Arduino library for robust communication with the RG15 rain gauge
 * sensor, written in C++.
 * @author Paul Reichmuth (@PaulReichmuth) and Björn Luig (@BjoernLuig) with
 * inspiration from Jan-Patrick Bollow(@JBollow)
 * @version 1.0.0
 * @date 2024-05-22
 * @note See official documentation of the RG15:
 * https://rainsensors.com/wp-content/uploads/sites/3/2020/07/rg-15_instructions_sw_1.000.pdf#page=2
 */

bool RG15::_changeSettings(char settingCode) {
  settingCode = tolower(settingCode);  // necessary to match response
  _errorCode = 0;                      // reset error
  if (!_checkSerial()) return false;   // fail

  // do multiple attemps
  for (_attempts = 0; _attempts < _maxAttempts; _attempts++) {
    if (_attempts) _cleanSerial();               // skip clean on first try
    if (!_sendCommand(settingCode)) continue;    // failed attempt
    if (!_collectResponse()) continue;           // failed attempt
    if (!_matchResponse(settingCode)) continue;  // failed attempt
    return true;                                 // success
  }
  return false;  // all attempts failed
}

bool RG15::_checkSerial() {
  if (_serial == nullptr) {
    _errorCode = 1;  // serial does not exist
    return false;    // fail
  } else {
    return true;  // success
  }
}

void RG15::_cleanSerial() {
  unsigned long cleanTime = millis();
  while (millis() - cleanTime < _cleanTimeout) {
    if (_serial->available()) _serial->read();  // clean serial stream
  }
}

bool RG15::_sendCommand(char command) {
  if (_serial->write(command) == 1 && _serial->write('\n') == 1) {
    return true;  // success
  } else {
    _errorCode = 2;  // serial could not write
    return false;    // fail
  }
}

bool RG15::_sendCommand(char* command) {
  if (_serial->write(command) == strlen(command) && _serial->write('\n') == 1) {
    return true;  // success
  } else {
    _errorCode = 2;  // serial could not write
    return false;    // fail
  }
}

bool RG15::_collectResponse() {
  unsigned long responseTime = millis();
  int index = 0;

  // read characters and ensure timeout
  while (millis() - responseTime < _responseTimeout) {
    if (_serial->available()) {
      char c = _serial->read();

      // check for line break (end of response)
      if (c == '\n') {
        _responseBuffer[index] = '\0';  // ensure null termination
        return true;                    // success
      }

      // write valid ascii
      if (c < 32 || c > 126) continue;  // skip non valid ascii characters
      _responseBuffer[index++] = c;

      // check for size limit of response buffer
      if (index == sizeof(_responseBuffer) - 1) {
        _responseBuffer[index] = '\0';  // ensure null termination
        _errorCode = 3;                 // response is invalid
        return false;                   // fail
      }
    }
  }
  _responseBuffer[index] = '\0';  // ensure null termination
  _errorCode = 4;                 // response timed out
  return false;                   // fail
}

bool RG15::_matchResponse(char expectedResponse) {
  if (strlen(_responseBuffer) > 0 && _responseBuffer[0] == expectedResponse) {
    return true;  // success
  } else {
    _errorCode = 3;  // response is invalid
    return false;    // fail
  }
}

bool RG15::_matchResponse(char* expectedResponse) {
  if (strcmp(_responseBuffer, expectedResponse) == 0) {
    return true;  // success
  } else {
    _errorCode = 3;  // response is invalid
    return false;    // fail
  }
}

int RG15::_getBaudCode(unsigned int baudRate) {
  for (int baudCode = 0; baudCode < sizeof(_baudRates); baudCode++)
    if (_baudRates[baudCode] == baudRate) return baudCode;  // success
  _errorCode = 5;  // unsupported baud rate
  return -1;       // unvalid baud rate
}

RG15::RG15(HardwareSerial& serial, unsigned int cleanTimeout,
           unsigned int responseTimeout, unsigned int maxAttempts)
    : _serial(&serial),
      _cleanTimeout(cleanTimeout),
      _responseTimeout(responseTimeout),
      _maxAttempts(maxAttempts) {}

RG15::RG15(HardwareSerial& serial)
    : RG15(serial, 500, 1000, 5) {}  // recommended defaults

bool RG15::begin(int baudRate, bool highResolution, char unit) {
  _errorCode = 0;                                  // reset error
  if (_getBaudCode(baudRate) == -1) return false;  // fail

  // start serial
  if (!_checkSerial()) return false;  // fail
  _serial->begin(baudRate);
  while (!_serial);

  // sensor settings
  bool success = setPollingMode();  // library only supports polling mode
  if (highResolution) {
    success &= setHighResolution();
  } else {
    success &= setLowResolution();
  }
  success &= setUnit(unit);
  return success;  // fail or success
}

bool RG15::begin() { return begin(9600, true, 'm'); }  // recommende defaults

bool RG15::poll() {
  _errorCode = 0;                     // reset error
  if (!_checkSerial()) return false;  // fail

  // strings for float conversion since sscanf does not support this in arduino
  char accBuffer[9];  // -> max "xxxxx.xx" mm -> approx 30 years of rain
  char eventAccBuffer[9];
  char totalAccBuffer[9];
  char rIntBuffer[9];
  char unitBuffer[4];  // -> "mm," or "in,"

  // do multiple attemps
  for (_attempts = 0; _attempts < _maxAttempts; _attempts++) {
    _cleanSerial();                     // skip clean on first try
    if (!_sendCommand('r')) continue;   // failed attempt
    if (!_collectResponse()) continue;  // failed attempt

    // parse poll response
    int parsed = sscanf(_responseBuffer,
                        "Acc %8s %s EventAcc %8s %*s TotalAcc %8s %*s RInt %8s",
                        accBuffer, unitBuffer, eventAccBuffer, totalAccBuffer,
                        rIntBuffer);
    if (parsed == 5) {  // parsed everything
      _acc = atof(accBuffer);
      _eventAcc = atof(eventAccBuffer);
      _totalAcc = atof(totalAccBuffer);
      _rInt = atof(rIntBuffer);

      // check unit
      if (unitBuffer[0] == _unit) {  // 'm' or 'i'
        return true;                 // success
      } else {
        _errorCode = 7;  // unit mismatch
        continue;        // failed attempt
      }
    } else {  // parsing failed
      _acc = -1;
      _eventAcc = -1;
      _totalAcc = -1;
      _rInt = -1;
      _errorCode = 6;  // parsing failed
      continue;        // failed attempt
    }
  }
  return false;  // failed all attempts
}

bool RG15::restart() {
  _errorCode = 0;                     // reset error
  if (!_checkSerial()) return false;  // fail

  // do multiple attemps
  for (_attempts = 0; _attempts < _maxAttempts; _attempts++) {
    if (_attempts) _cleanSerial();
    if (!_sendCommand('k')) continue;  // failed attempt
    delay(_responseTimeout);           // additional delay for restart
    _cleanSerial();
    return true;  // success
  }
  return false;  // failed all attempts
}

bool RG15::changeBaudRate(unsigned int baudRate) {
  int baudCode = _getBaudCode(baudRate);  // get baud code for baud rate
  char baudCommand[11];                   // maximal 'Baud 57600' as response
  if (baudCode == -1) return false;       // fail
  _errorCode = 0;                         // reset error
  if (!_checkSerial()) return false;      // fail

  // do multiple attemps
  for (_attempts = 0; _attempts < _maxAttempts; _attempts++) {
    // send baud command (here baud code)
    sprintf(baudCommand, "b %i", baudCode);
    if (_attempts) _cleanSerial();             // skip clean on first try
    if (!_sendCommand(baudCommand)) continue;  // failed attempt

    // check response (here baud rate)
    sprintf(baudCommand, "Baud %i", baudRate);
    if (!_collectResponse()) continue;           // failed attempt
    if (!_matchResponse(baudCommand)) continue;  // failed attempt

    // change own baud rate
    _serial->begin(baudRate);
    while (!_serial);
    return true;  // success
  }
  return false;  // failed all attempts
}

bool RG15::setPollingMode() {
  return _changeSettings('p');  // fail or success
}

bool RG15::setHighResolution() {
  return _changeSettings('h');  // fail or success
}

bool RG15::setLowResolution() {
  return _changeSettings('l');  // fail or success
}

bool RG15::setUnit(char unit) {
  _errorCode = 0;  // reset error
  if (unit == 'm' || unit == 'i') {
    if (_changeSettings(unit)) {
      _unit = unit;
      return true;  // success
    } else {
      return false;  // fail
    }
  } else {
    _errorCode = 7;  // unit mismatch
    return false;    // fail
  }
}

char RG15::getUnit() { return _unit; }

bool RG15::resetAccumulation() {
  _errorCode = 0;                     // reset error
  if (!_checkSerial()) return false;  // fail

  // do multiple attemps
  for (_attempts = 0; _attempts < _maxAttempts; _attempts++) {
    if (_attempts) _cleanSerial();       // skip clean on first try
    if (_sendCommand('o')) return true;  // success
    // there is no response for this command
  }
  return false;  // all attempts failed
}

float RG15::getAccumulation() { return _acc; }

float RG15::getEventAccumulation() { return _eventAcc; }

float RG15::getTotalAccumulation() { return _totalAcc; }

float RG15::getRainfallIntensity() { return _rInt; }

int RG15::getErrorCode() { return _errorCode; }

int RG15::getAttempts() { return _attempts; }

char* RG15::getResponseBuffer() { return _responseBuffer; }
