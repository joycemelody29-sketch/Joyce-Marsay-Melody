#include <Arduino.h>

unsigned long lastHelloMs = 0;

void setup() {
    Serial.begin(115200);

    // Tunggu hingga Serial Monitor terhubung (maksimal 5 detik)
    // agar pesan awal tidak hilang
    while (!Serial && millis() < 5000);

    delay(2000);

    Serial.println("Program 01: UART Echo - ESP32");
    Serial.println("HELLOW WORLD");
}

void loop() {

    if (millis() - lastHelloMs >= 1000) {
        lastHelloMs = millis();
        Serial.println("HELLOW WORLD");
    }

    if (Serial.available()) {
        Serial.write(Serial.read());
    }

}
