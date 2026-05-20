#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// ================= PIN TFT =================
#define TFT_CS    5
#define TFT_RST   4
#define TFT_DC    2

// Membuat object TFT
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {

  // Serial Monitor
  Serial.begin(115200);

  // ================= INISIALISASI TFT =================
  tft.initR(INITR_BLACKTAB);

  // Rotasi layar
  tft.setRotation(1);

  // Background merah
  tft.fillScreen(ST77XX_RED);

  // ================= PENGATURAN TEKS =================
  tft.setTextColor(ST77XX_BLACK);

  // Ukuran teks
  tft.setTextSize(2);

  // Posisi cursor
  tft.setCursor(20, 30);

  // Tampilkan tulisan
  tft.println("HELLO");

  tft.setCursor(20, 60);
  tft.println("ESP32");

  tft.setCursor(20, 90);
  tft.println("SPI LCD");
}

void loop() {

}
