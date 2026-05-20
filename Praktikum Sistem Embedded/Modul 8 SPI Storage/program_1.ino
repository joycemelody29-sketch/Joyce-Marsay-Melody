#include <Arduino.h>
#include <Wire.h>

#define STM32_ADDR 0x08

#define ESP32_SDA 21
#define ESP32_SCL 22

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(ESP32_SDA, ESP32_SCL);
  Wire.setClock(100000);

  Serial.println("ESP32 MASTER READY");
}

void loop() {
  const char pesan[] = "HELLO WORLD";

  Wire.beginTransmission(STM32_ADDR);
  Wire.write((const uint8_t *)pesan, strlen(pesan));
  byte error = Wire.endTransmission();

  if (error == 0) {
    Serial.println("Terkirim: HELLO WORLD");
  } else {
    Serial.print("Gagal kirim, error: ");
    Serial.println(error);
  }

  delay(1000);
}
