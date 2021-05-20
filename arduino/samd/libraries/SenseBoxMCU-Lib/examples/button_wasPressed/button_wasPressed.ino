#include <SenseBoxMCU.h>

Button button(0);
int counter = 0;

void setup(){
    button.begin();
    Serial.begin(9600);
}

void loop(){
    int pressed = button.wasPressed();
    Serial.println(pressed);
    if (pressed == HIGH){
        counter++;
    }
    Serial.println(counter);
}