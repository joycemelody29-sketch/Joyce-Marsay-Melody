#include <Arduino.h>
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// =====================================================
// TFT ST7735 di SPI2 STM32
// =====================================================
#define TFT_CS    PB12
#define TFT_DC    PB10
#define TFT_RST   PB11

// SPI2 STM32F103
// PB13 = SCK
// PB14 = MISO
// PB15 = MOSI

SPIClass SPI_TFT(PB15, PB14, PB13);

Adafruit_ST7735 tft =
    Adafruit_ST7735(&SPI_TFT, TFT_CS, TFT_DC, TFT_RST);

// =====================================================
// SPI1 STM32 sebagai SLAVE dari ESP32
// =====================================================

// PA4 = NSS / CS dari ESP32
// PA5 = SCK dari ESP32
// PA6 = MISO ke ESP32
// PA7 = MOSI dari ESP32

#define SPI_SLAVE_CS   PA4

#define DATA_LEN 16

uint8_t rxBuffer[DATA_LEN];

// =====================================================
// CLEAR SPI BUFFER
// =====================================================
void clearSPIBuffer() {

  volatile uint8_t dummy;

  while (SPI1->SR & SPI_SR_RXNE) {

    dummy = SPI1->DR;
  }

  dummy = SPI1->SR;
  dummy = SPI1->DR;
}

// =====================================================
// RECEIVE DATA FROM ESP32
// =====================================================
bool receiveFromESP32(
    uint8_t *buffer,
    uint8_t length,
    uint32_t timeout
) {

  uint32_t startTime = millis();

  uint8_t index = 0;

  clearSPIBuffer();

  // Tunggu CS dari ESP32 LOW
  while (digitalRead(SPI_SLAVE_CS) == HIGH) {

    if (millis() - startTime > timeout) {

      return false;
    }
  }

  while (index < length) {

    if (SPI1->SR & SPI_SR_RXNE) {

      buffer[index++] = SPI1->DR;
    }
  }

  return true;
}

// =====================================================
// SETUP
// =====================================================
void setup() {

  Serial.begin(115200);

  // TFT
  SPI_TFT.begin();

  tft.initR(INITR_BLACKTAB);

  tft.setRotation(1);

  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_GREEN);

  tft.setTextSize(2);

  // SPI SLAVE
  pinMode(SPI_SLAVE_CS, INPUT_PULLUP);
}

// =====================================================
// LOOP
// =====================================================
void loop() {

  if (receiveFromESP32(rxBuffer, DATA_LEN, 1000)) {

    Serial.println((char *)rxBuffer);

    tft.fillScreen(ST77XX_BLACK);

    tft.setCursor(10, 40);

    tft.println((char *)rxBuffer);
  }
}
