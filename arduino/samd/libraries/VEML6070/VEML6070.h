// VEML6070 library

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class VEML6070 {
public:
  VEML6070();
  void begin();
  float getUV();
};
