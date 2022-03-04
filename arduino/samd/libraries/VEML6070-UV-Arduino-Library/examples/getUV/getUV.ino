#include <senseBoxIO.h>
#include <VEML6070.h>

VEML6070 veml;

void setup()
{
    Serial.begin(9600);
    veml.begin();
}

void loop()
{
    Serial.print("UV: ");
    Serial.println(veml.getUV());
}
