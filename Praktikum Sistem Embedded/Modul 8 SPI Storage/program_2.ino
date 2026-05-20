#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= LCD =================
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

#define LCD_SDA_PIN PB11
#define LCD_SCL_PIN PB10

// ================= I2C SLAVE DARI ESP32 =================
#define STM32_SLAVE_ADDR 0x08

#define SLAVE_SDA_PIN PB7
#define SLAVE_SCL_PIN PB6

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// I2C khusus untuk menerima data dari ESP32
TwoWire WireSlave(SLAVE_SDA_PIN, SLAVE_SCL_PIN);

// ================= VARIABEL =================
volatile bool dataMasuk = false;
volatile uint8_t panjangData = 0;

volatile char bufferISR[17];
char textLCD[17] = "WAIT ESP32";

uint32_t rxCounter = 0;
unsigned long lastLCDUpdate = 0;

// ================= CALLBACK I2C RECEIVE =================
void onReceiveData(int jumlahByte) {
  int i = 0;
  // Tambahkan pengecekan batas buffer agar tidak overflow
  while (WireSlave.available() && i < (int)(sizeof(bufferISR) - 1)) {
    char c = WireSlave.read();
    if (c >= 32 && c <= 126) {
      bufferISR[i] = c;
      i++;
    }
  }
  bufferISR[i] = '\0';
  dataMasuk = true;
}

// ================= UPDATE LCD =================
void tampilLCD() {
  static char lastText[17] = ""; 
  char line1[17];

  // Gunakan format yang tetap lebarnya agar tidak perlu clear screen (mencegah kedip)
  snprintf(line1, sizeof(line1), "Count: %-9lu", rxCounter);

  lcd.setCursor(0, 0);
  lcd.print(line1);

  // Hanya update baris 2 jika teks berubah
  if (strcmp(textLCD, lastText) != 0) {
    char paddedText[17];
    snprintf(paddedText, sizeof(paddedText), "%-16s", textLCD); // Pad dengan spasi
    lcd.setCursor(0, 1);
    lcd.print(paddedText);
    strncpy(lastText, textLCD, 16);
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(1000);

  // ---------- LCD ----------
  Wire.setSDA(LCD_SDA_PIN);   // PB11 = SDA
  Wire.setSCL(LCD_SCL_PIN);   // PB10 = SCL
  Wire.begin();
  Wire.setClock(100000);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("STM32 SLAVE");
  lcd.setCursor(0, 1);
  lcd.print("LCD READY");
  delay(1000);

  // ---------- I2C SLAVE ----------
  WireSlave.begin(STM32_SLAVE_ADDR);
  WireSlave.onReceive(onReceiveData);

  lcd.clear();
  tampilLCD();
}

// ================= LOOP =================
void loop() {
  if (dataMasuk) {
    // Salin data dengan aman (Critical Section)
    noInterrupts();
    strcpy(textLCD, (char*)bufferISR);
    dataMasuk = false;
    interrupts();

    // Tampilkan ke Serial Monitor
    Serial.print("[I2C RX] Data: ");
    Serial.print(textLCD);
    Serial.printf(" | Total RX: %lu\n", rxCounter + 1);

    rxCounter++;
    tampilLCD();
  }
  
  // Update counter setiap 1 detik
  if (millis() - lastLCDUpdate >= 1000) {
    lastLCDUpdate = millis();
    tampilLCD();
  }
}
