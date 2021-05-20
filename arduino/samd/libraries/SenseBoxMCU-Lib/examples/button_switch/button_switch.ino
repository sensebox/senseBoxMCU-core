#include <SenseBoxMCU.h>

Button button(0);

void setup(){
    button.begin();
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    Serial.begin(9600);
}

void loop(){
    int state = button.getSwitch();
    Serial.println(state);

    if (state == HIGH){
        digitalWrite(8,HIGH);
        digitalWrite(7, LOW);
    }
    else {
        digitalWrite(7,HIGH);
        digitalWrite(8,LOW);
    }
}