#include <Arduino.h>
#include "config.h"

void setup()
{
    Serial.begin(SERIAL_BAUD);

    pinMode(LED_BUILTIN_PIN, OUTPUT);
    pinMode(LED_EXTERNAL, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Matikan LED saat awal
    digitalWrite(LED_BUILTIN_PIN, HIGH); 
    digitalWrite(LED_EXTERNAL, LOW);
}

void loop()
{
    int buttonState = digitalRead(BUTTON_PIN);

    // INPUT_PULLUP:
    // Tekan = LOW
    // Lepas = HIGH

    if (buttonState == LOW)
    {
        digitalWrite(LED_BUILTIN_PIN, LOW);   // ON (active LOW)
        digitalWrite(LED_EXTERNAL, HIGH);     // ON
    }
    else
    {
        digitalWrite(LED_BUILTIN_PIN, HIGH);  // OFF
        digitalWrite(LED_EXTERNAL, LOW);      // OFF
    }
}
