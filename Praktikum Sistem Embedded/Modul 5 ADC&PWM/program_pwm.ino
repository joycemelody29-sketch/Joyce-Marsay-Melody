#include <Arduino.h>

#define LDR_PIN PA2
#define POT_PIN PA0
#define LED_PIN PA1

void setup() {
    Serial.begin(115200);
    delay(2000);
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    int ldr = analogRead(LDR_PIN);
    int pot = analogRead(POT_PIN);

    int threshold = pot;

    // 🔥 BALIK LOGIKA BIAR JELAS:
    // Gelap = LED nyala
    int led;

    if (ldr < threshold) {
        led = 255;   // nyala saat gelap (DITUTUPI)
    } else {
        led = 0;     // mati saat terang
    }

    analogWrite(LED_PIN, led);

    Serial.printf("LDR:%4d | POT:%4d | LED:%d\n", ldr, pot, led);

    delay(200);
}
