#include <Arduino.h>
#include <SPI.h>

// ================= PIN SPI =================
#define STM32_CS   5

#define DATA_LEN   16

void setup() {

  Serial.begin(115200);

  // SPI ESP32
  SPI.begin();

  pinMode(STM32_CS, OUTPUT);
  digitalWrite(STM32_CS, HIGH);
}

void loop() {

  uint8_t txData[DATA_LEN] = {0};

  // Data yang dikirim ke STM32
  snprintf((char *)txData, DATA_LEN, "Hello STM32");

  SPI.beginTransaction(
      SPISettings(5000, MSBFIRST, SPI_MODE0)
  );

  digitalWrite(STM32_CS, LOW);

  delayMicroseconds(20);

  for (int i = 0; i < DATA_LEN; i++) {

    SPI.transfer(txData[i]);
  }

  digitalWrite(STM32_CS, HIGH);

  SPI.endTransaction();

  Serial.println((char *)txData);

  delay(1000);
}
