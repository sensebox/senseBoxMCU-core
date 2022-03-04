/**
 * Attributes for some code snippets go to senseBox library examples.
 */
#include <Plot.h> // Our plot library
#include <senseBoxIO.h> // Used to power I2C pins
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BME280.h>
#include <Adafruit_BME680.h>
#include <Adafruit_HDC1000.h>


// Allocate OLED display object
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
// Allocate plot object
Plot plot(&display);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// The timeframe displayed on the graph
const double TIMEFRAME = 15.0;
const int BUTTON_PIN = 0;


typedef struct addressed_sensor {
  byte addresses[5]; // Sensor can have up to 5 addresses
  unsigned int address_count;
  double (* measurement)();
  void (* setup_plot)();
} AddressedSensor;


byte currentAddress = 0;
AddressedSensor* currentSensor = 0;

void setup()   {                
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // I2C/Wire Pins anschalten
  senseBoxIO.powerI2C(true);
  delay(1000); // wait 1000ms

  // Sometimes address 0x3C is needed instead of 0x3D
  // You can use an I2C scanner to check which address is used by your display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  // Initialisierung fertig
  
  // Clear display buffer initially
  display.clearDisplay();
  display.display();
  delay(100);
  
  nextSensor();
}

void loop() {
  if (currentSensor == 0) {
    delay(5000);
    nextSensor();
    return;
  }
  plot.clear();
  
  double starttime = millis();
  // Current time since start of this timeframe
  double t = 0;
  // Count how many measurements have been made to calculate speed
  int measurements = 0;
  
  while (t <= TIMEFRAME) {
    if (isButtonPressed()) {
      nextSensor();
      break;
    }
    // Get current time -> x-coordinate
    t = (millis() - starttime) / 1000.0;
    // Measurement -> y-coordinate
    double d = currentSensor->measurement();
    measurements++;
    // Plot
    plot.addDataPoint(t, d);
  }
  
  Serial.print("Average time per measurement/display cycle: ");
  Serial.print(t/measurements, 2);
  Serial.println("s");
}

bool isButtonPressed() {
  int buttonState; // variable for reading the button status
  // read the state of the button
  buttonState = digitalRead(BUTTON_PIN);
  return buttonState == LOW; // state low = button pressed
}


// ============ Sensor Management =================
AddressedSensor sensors[] = {
  { {0x40, 0x41, 0x43, 0, 0}, 3, &check_sensor_hdc1000_temp, &setup_sensor_hdc1000_temp },
  { {0x40, 0x41, 0x43, 0, 0}, 3, &check_sensor_hdc1000_humid, &setup_sensor_hdc1000_humid },
  { {0x76, 0x77, 0, 0, 0}, 2, &check_sensor_bmxX80_pressure, &setup_sensor_bmxX80_pressure },
  { {0x76, 0x77, 0, 0, 0}, 2, &check_sensor_bmxX80_temp, &setup_sensor_bmxX80_temp }
};
AddressedSensor* lastSensor = sensors + 3;

void nextSensor() {
  find_next_sensor();
  if (currentSensor == 0) {
    // No sensor was found after the current sensor
    // -> try again from start
    find_next_sensor();
    if (currentSensor == 0) {
      // No sensor was found on any address
      Serial.println("No sensor found.");
      display.clearDisplay();
      display.display();
      delay(100);
      display.setCursor(0, 0);
      display.setTextColor(WHITE, BLACK);
      display.setTextSize(2);
      display.println("No sensor found!");
      display.display();
      return;
    }
  }
  currentSensor->setup_plot();
}


void find_next_sensor() {
  Serial.println("Scanning...");
  senseBoxIO.statusRed();
  // If no sensor is selected set search to start
  AddressedSensor *sensor;
  if (currentSensor == 0) {
    sensor = sensors;
  } else {
    sensor = currentSensor + 1;
  }
  for(; sensor <= lastSensor; sensor++) {
    // For each sensor starting from the current sensor
    // check all addresses if any is connected.
    for (byte *address = sensor->addresses; 
         address < sensor->addresses + sensor->address_count; 
         address++) {
      Wire.beginTransmission(*address);
      byte error = Wire.endTransmission();

      // If an active address is found: select that sensor
      // TODO: Extended testing if sensor is working 
      // (can also be used to differentiate between multiple 
      // sensors who possibly use the same address)
      if(error == 0)
      {
        Serial.print("\nDevice found at 0x");
        Serial.println(*address, HEX);
        currentSensor = sensor;
        currentAddress = *address;
        senseBoxIO.statusNone();
        return;
      }
      else if(error==4)
      {
        Serial.print("\nUnknow error at 0x");
        Serial.println(*address, HEX);
      }
    }
  }
  senseBoxIO.statusNone();
  currentSensor = 0;
  currentAddress = 0;
}

// ===========================================

// ================ HDC1000 ==================
Adafruit_HDC1000 hdc;
double check_sensor_hdc1000_temp() {
  hdc.begin(currentAddress);
  return hdc.readTemperature();
}
void setup_sensor_hdc1000_temp() {
  plot.setTitle("Temperature");
  plot.setXLabel("s");
  plot.setYLabel("*C");
  plot.setXRange(0, TIMEFRAME);
  plot.setYRange(20, 30);
  plot.setXTick(5);
  plot.setYTick(5);
  plot.setXPrecision(0);
  plot.setYPrecision(0);
}
double check_sensor_hdc1000_humid() {
  hdc.begin(currentAddress);
  return hdc.readHumidity();
}
void setup_sensor_hdc1000_humid() {
  plot.setTitle("Humidity");
  plot.setXLabel("s");
  plot.setYLabel("%");
  plot.setXRange(0, TIMEFRAME);
  plot.setYRange(0, 100);
  plot.setXTick(5);
  plot.setYTick(25);
  plot.setXPrecision(0);
  plot.setYPrecision(0);
}

// ============= BME280 BME680 BMP280 ==============
Adafruit_BMP280 bmp280;
Adafruit_BME280 bme280;
Adafruit_BME680 bme680;
double check_sensor_bmxX80_temp() {
  double t;
  if (bmp280.begin(currentAddress) != 0) {
    t = bmp280.readTemperature();
  } else if(bme280.begin(currentAddress) != 0) {
    t = bme280.readTemperature();
  } else if(bme680.begin(currentAddress) != 0) {
    bme680.performReading();
    t = bme680.temperature;
  }
  return t;
}
void setup_sensor_bmxX80_temp() {
  plot.setTitle("Temperature");
  plot.setXLabel("s");
  plot.setYLabel("*C");
  plot.setXRange(0, TIMEFRAME);
  plot.setYRange(20, 35);
  plot.setXTick(5);
  plot.setYTick(5);
  plot.setXPrecision(0);
  plot.setYPrecision(0);
}
double check_sensor_bmxX80_pressure() {
  double p;
  if (bmp280.begin(currentAddress) != 0) {
    p = bmp280.readPressure();
  } else if(bme280.begin(currentAddress) != 0) {
    p = bme280.readPressure();
  } else if(bme680.begin(currentAddress) != 0) {
    bme680.performReading();
    p = bme680.pressure;
  }
  return p / 100.0;
}
void setup_sensor_bmxX80_pressure() {
  plot.setTitle("Pressure");
  plot.setXLabel("s");
  plot.setYLabel("hPa");
  plot.setXRange(0, TIMEFRAME);
  plot.setYRange(950, 1050);
  plot.setXTick(5);
  plot.setYTick(50);
  plot.setXPrecision(0);
  plot.setYPrecision(0);
}





// TODO IMPLEMENT
// BMXX80 altitude and gas
//    case 0x29:  //TSL45315
//      Serial.println("TSL45315 not implemented");
//      break;
//    case 0x38:  //VEML6070
//    //case 0x39:
//      Serial.println("--- VEML6070 (0x38+0x39) not implemented");
//      break;
//    case 0x42:  //CAM-M8Q
//      Serial.println("CAM-M8Q not implemented");
//      break;
//    case 0x50:  //24LCxxx EEPROM
//      Serial.println("24LCxxx not implemented");
//      break;
//    case 0x60:  //ATECCx08
//      Serial.println("ATECCx08 not implemented");
//      break;
//    case 0x68:  //RV8523
//      Serial.println("RV8523 not implemented");
//      break;

