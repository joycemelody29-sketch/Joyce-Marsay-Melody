#include <Arduino.h>

unsigned long lastHelloMs = 0;

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("Program 01: UART Echo - STM32");
    Serial.println("Hello World");
}

void loop() {
    if (millis() - lastHelloMs >= 1000) {
        lastHelloMs = millis();
        Serial.println("Hello World");
    }

    if (Serial.available()) {
        Serial.write(Serial.read());
    }
}

program stmuarts
