#include <Plot.h> // Our plot library
#include <senseBoxIO.h> // Used to power I2C pins
#include <Ultrasonic.h>

// Allocate OLED display object
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
// Allocate sensor objects
Ultrasonic Ultrasonic(1,2);
// Allocate plot object
Plot ultrasonicPlot(&display);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// The timeframe displayed on the graph
const double TIMEFRAME = 15.0;

void setup()   {                
  Serial.begin(9600);

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

  // Config plot to have right labels and range
  ultrasonicPlot.setTitle("Distance");
  ultrasonicPlot.setYLabel("cm");
  ultrasonicPlot.setXRange(0, TIMEFRAME);
  // Increase if you want to display higher distances
  ultrasonicPlot.setYRange(0, 100);
  ultrasonicPlot.setXTick(5);
  ultrasonicPlot.setYTick(20);
  ultrasonicPlot.setYPrecision(0);
  ultrasonicPlot.setXPrecision(0);
  ultrasonicPlot.setXLabel("s");
}

void loop() {
  ultrasonicPlot.clear();
  
  double starttime = millis();
  // Current time since start of this timeframe
  double t = 0;
  // Count how many measurements have been made to calculate speed
  int measurements = 0;
  
  while (t <= TIMEFRAME) {
    // Get current time -> x-coordinate
    t = (millis() - starttime) / 1000.0;
    // Measurement -> y-coordinate
    double d = Ultrasonic.getDistance();
    measurements++;
    // Plot
    ultrasonicPlot.addDataPoint(t, d);
  }
  
  Serial.print("Average time per measurement/display cycle: ");
  Serial.print(t/measurements, 2);
  Serial.println("s");
}
