
#include <SenseBoxMCU.h>

Button button(0);

void setup(){
    button.begin();
    Serial.begin(9600);
}

void loop(){
    Serial.println(button.isPressed());
}