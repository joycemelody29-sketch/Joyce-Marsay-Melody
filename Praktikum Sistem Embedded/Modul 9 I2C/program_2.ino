#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

// ================= PIN SPI ESP32 =================
#define SD_SCK   18
#define SD_MISO  19
#define SD_MOSI  23
#define SD_CS     5

// =====================================================
// FUNCTION PRINT SIZE
// =====================================================
void printSize(uint64_t bytes) {

  float kb = bytes / 1024.0;
  float mb = kb / 1024.0;
  float gb = mb / 1024.0;

  if (gb >= 1.0) {

    Serial.print(gb, 2);
    Serial.println(" GB");

  } else if (mb >= 1.0) {

    Serial.print(mb, 2);
    Serial.println(" MB");

  } else if (kb >= 1.0) {

    Serial.print(kb, 2);
    Serial.println(" KB");

  } else {

    Serial.print(bytes);
    Serial.println(" Bytes");
  }
}

// =====================================================
// FUNCTION PRINT SD INFO
// =====================================================
void printSDInfo() {

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {

    Serial.println("SD Card tidak terdeteksi");
    return;
  }

  Serial.println("===== INFO SD CARD =====");

  Serial.print("Tipe SD Card : ");

  if (cardType == CARD_MMC) {

    Serial.println("MMC");

  } else if (cardType == CARD_SD) {

    Serial.println("SDSC");

  } else if (cardType == CARD_SDHC) {

    Serial.println("SDHC");

  } else {

    Serial.println("UNKNOWN");
  }

  Serial.print("Total Size : ");
  printSize(SD.cardSize());

  Serial.print("Used Size  : ");
  printSize(SD.usedBytes());

  Serial.print("Free Size  : ");
  printSize(SD.totalBytes() - SD.usedBytes());

  Serial.println("========================");
}

// =====================================================
// SETUP
// =====================================================
void setup() {

  Serial.begin(115200);

  // Inisialisasi SPI
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  // Inisialisasi SD Card
  if (!SD.begin(SD_CS)) {

    Serial.println("Gagal mengakses SD Card");
    return;
  }

  Serial.println("SD Card berhasil diakses");

  // Tampilkan informasi SD Card
  printSDInfo();
}

// =====================================================
// LOOP
// =====================================================
void loop() {

}
