#include "Arduino.h"

/**
 * @brief Arduino library for robust communication with the RG15 rain gauge
 * sensor, written in C++.
 * @author Paul Reichmuth (@PaulReichmuth) and Björn Luig (@BjoernLuig), with
 * inspiration from Jan-Patrick Bollow (@JBollow).
 * @version 1.0.0
 * @date 2024-05-22
 * @note See the official documentation for the RG15:
 * https://rainsensors.com/wp-content/uploads/sites/3/2020/07/rg-15_instructions_sw_1.000.pdf#page=2
 */

class RG15 {
 private:
  HardwareSerial* _serial;  // Pointer to the sensor's serial port
  const unsigned int _baudRates[7] = {
      1200,  2400,  4800, 9600,
      19200, 38400, 57600};  // Valid baud rates indexed by baud codes (0-6)
  char _responseBuffer[90] = {0};  // Buffer to store the last sensor response
  unsigned long _cleanTimeout;     // Timeout for cleaning the serial stream
  unsigned long _responseTimeout;  // Timeout for sensor responses
  unsigned int _attempts = 0;      // Number of attempted sensor communications
  unsigned int _maxAttempts;       // Maximum allowed attempts for communication
  char _unit = 'm';  // Unit: 'm' for metric (mm) and 'i' for imperial (inches)
                     // if unknown
  float _acc = -1;   // Accumulated rainfall since the last poll
  float _eventAcc = -1;  // Accumulated rainfall in the current event
  float _totalAcc = -1;  // Total accumulated rainfall since the last reset
  float _rInt = -1;      // Rainfall intensity
  int _errorCode = 0;    // Code of the latest error for debugging

  bool _changeSettings(char setting);
  bool _checkSerial();
  void _cleanSerial();
  bool _sendCommand(char command);
  bool _sendCommand(char* command);
  bool _collectResponse();
  bool _matchResponse(char expectedResponse);
  bool _matchResponse(char* expectedResponse);
  int _getBaudCode(unsigned int baudRate);

 public:
  /**
   * @brief Construct a new RG15 sensor instance.
   *
   * @param serial Reference to the serial port to use (e.g., Serial1 or Serial2
   * on the senseBox MCU and Serial0 on the senseBoc MCU-S2).
   * @param cleanTimeout Timeout for cleaning the serial communication stream in
   * ms (should be at least 500 ms).
   * @param responseTimeout Timeout for response in sensor communication in ms
   * (should be at least 1000 ms).
   * @param maxAttempts Maximum number of attempts for sensor communication.
   */
  RG15(HardwareSerial& serial, unsigned int cleanTimeout,
       unsigned int responseTimeout, unsigned int maxAttempts);

 public:
  /**
   * @brief Construct a new RG15 sensor instance with default communication
   * settings (cleanTimeout = 500 ms, responseTimeout = 1000 ms, maxAttempts =
   * 5).
   *
   * @param serial Reference to the serial port to use (e.g., Serial1 or Serial2
   * on the senseBox).
   */
  RG15(HardwareSerial& serial);

  /**
   * @brief Initialize the sensor and communication.
   *
   * @param baudRate The baud rate for serial sensor communication.
   * @param highResolution True for high resolution, false for low resolution.
   * @param unit 'm' for metric, 'i' for imperial units.
   * @return true if the sensor was initialized successfully, false otherwise.
   */
  bool begin(int baudRate, bool highResolution, char unit);

  /**
   * @brief Initialize the sensor and communication with default parameters:
   * 9600 baud rate, high resolution, and metric units.
   *
   * @return true if the sensor was initialized successfully, false otherwise.
   */
  bool begin();

  /**
   * @brief Poll all rain gauge data in polling mode and store it in the sensor
   * object.
   *
   * @return true if the data was successfully retrieved, false otherwise.
   */
  bool poll();

  /**
   * @brief Restart the sensor.
   *
   * @return true if the sensor was restarted successfully, false otherwise.
   */
  bool restart();

  /**
   * @brief Change the sensor's baud rate and reestablish communication. It is
   * advised to not use this and keep the baud rate at 9600.
   *
   * @param baudRate The baud rate to change to. Only 1200, 2400, 4800, 9600,
   * 19200, 38400, and 57600 are valid.
   * @return true if the baud rate was changed correctly, false otherwise.
   */
  bool changeBaudRate(unsigned int baudRate);

  /**
   * @brief Set the sensor to polling mode (the only mode supported by this
   * library). It can also be used as a ping test.
   *
   * @return true if polling mode was set and acknowledged by the sensor, false
   * otherwise.
   */
  bool setPollingMode();

  /**
   * @brief Enable high resolution mode.
   *
   * @return true if high resolution was set and acknowledged by the sensor,
   * false otherwise.
   */
  bool setHighResolution();

  /**
   * @brief Enable low resolution mode.
   *
   * @return true if low resolution was set and acknowledged by the sensor,
   * false otherwise.
   */
  bool setLowResolution();

  /**
   * @brief Set the sensor's unit to metric or imperial.
   *
   * @param unit 'm' for metric, 'i' for imperial.
   * @return true if the unit was set and acknowledged by the sensor, false
   * otherwise.
   */
  bool setUnit(char unit);

  /**
   * @brief Get the sensor's unit.
   *
   * @return 'm' for metric, 'i' for imperial.
   * */
  char getUnit();

  /**
   * @brief Reset the accumulated rainfall. This is only temporary until the
   * next restart of the sensor.
   *
   * @return true if the reset was successful, false otherwise.
   */
  bool resetAccumulation();

  /**
   * @brief Get the accumulated rainfall since the last poll.
   *
   * @return float The accumulated rainfall since the last poll.
   */
  float getAccumulation();

  /**
   * @brief Get the accumulated rainfall for the current event.
   *
   * @return float The accumulated rainfall for the current event.
   */
  float getEventAccumulation();

  /**
   * @brief Get the total accumulated rainfall since the last reset.
   *
   * @return float The total accumulated rainfall since the last reset.
   */
  float getTotalAccumulation();

  /**
   * @brief Get the current rainfall intensity.
   *
   * @return float The current rainfall intensity.
   */
  float getRainfallIntensity();

  /**
   * @brief Get the latest error code for debugging purposes.
   *
   * @return 0 if no error occurred,
   * @return 1 if the serial connection does not exist,
   * @return 2 if the serial connection could not write,
   * @return 3 if the response is invalid,
   * @return 4 if the response timed out,
   * @return 5 if the baud rate is not supported,
   * @return 6 if parsing failed,
   * @return 7 if the unit does not match.
   */
  int getErrorCode();

  /**
   * @brief Get the number of attempted communications.
   *
   * @return int The number of attempts.
   */
  int getAttempts();

  /**
   * @brief Get the response buffer for debugging purposes.
   *
   * @return char* The response buffer as a C-string.
   */
  char* getResponseBuffer();
};

/**
 * TODO:
 * List of unimplemented features that have not been necessary so far:
 * - Proper usage of continuous mode.
 * - Reading accumulation only.
 * - Using the hardware switches.
 * - Features related to the tipping bucket.
 */
