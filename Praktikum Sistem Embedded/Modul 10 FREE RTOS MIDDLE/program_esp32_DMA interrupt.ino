#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SPI.h>
#include <SD.h>

// =======================
// PIN CONFIG SESUAI WIRING
// =======================
#define DHT_GPIO 4
#define DHT_TYPE DHT22
// Kalau pakai DHT11, ubah jadi:
// #define DHT_TYPE DHT11

#define SD_SPI_CS_GPIO    5
#define SD_SPI_MOSI_GPIO  23
#define SD_SPI_MISO_GPIO  19
#define SD_SPI_SCLK_GPIO  18

#define I2C_SDA_GPIO 21
#define I2C_SCL_GPIO 22

// =======================
// OBJECT
// =======================
DHT dht(DHT_GPIO, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
SPIClass sdSPI(VSPI);

// =======================
// GLOBAL DATA
// =======================
float suhu = 0.0;
float kelembapan = 0.0;

bool dhtValid = false;
bool sdReady = false;

SemaphoreHandle_t dataMutex;

// =======================
// TASK HANDLE
// =======================
TaskHandle_t lcdTaskHandle;
TaskHandle_t dhtTaskHandle;
TaskHandle_t sdTaskHandle;

// =======================
// FUNGSI KONVERSI BYTE KE MB / GB
// =======================
float bytesToMB(uint64_t bytes) {
  return bytes / (1024.0 * 1024.0);
}

float bytesToGB(uint64_t bytes) {
  return bytes / (1024.0 * 1024.0 * 1024.0);
}

// =======================
// TASK 1: LCD PRIORITY UTAMA
// =======================
void taskLCD(void *parameter) {
  while (true) {
    float localSuhu = 0.0;
    float localKelembapan = 0.0;
    bool localDhtValid = false;

    if (xSemaphoreTake(dataMutex, portMAX_DELAY)) {
      localSuhu = suhu;
      localKelembapan = kelembapan;
      localDhtValid = dhtValid;
      xSemaphoreGive(dataMutex);
    }

    lcd.clear();

    if (localDhtValid) {
      lcd.setCursor(0, 0);
      lcd.print("Suhu: ");
      lcd.print(localSuhu, 1);
      lcd.print((char)223);
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("Hum : ");
      lcd.print(localKelembapan, 1);
      lcd.print(" %");
    } else {
      lcd.setCursor(0, 0);
      lcd.print("DHT Error");
      lcd.setCursor(0, 1);
      lcd.print("Cek Sensor");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// =======================
// TASK 2: DHT PRIORITY KEDUA
// =======================
void taskDHT(void *parameter) {
  while (true) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (xSemaphoreTake(dataMutex, portMAX_DELAY)) {
      if (isnan(h) || isnan(t)) {
        dhtValid = false;
      } else {
        suhu = t;
        kelembapan = h;
        dhtValid = true;
      }

      xSemaphoreGive(dataMutex);
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

// =======================
// TASK 3: SD CARD BACA PENYIMPANAN
// =======================
void taskSDCard(void *parameter) {
  while (true) {
    Serial.println("===== INFO SD CARD =====");

    if (sdReady) {
      uint64_t cardSize = SD.cardSize();
      uint64_t totalBytes = SD.totalBytes();
      uint64_t usedBytes = SD.usedBytes();
      uint64_t freeBytes = totalBytes - usedBytes;

      Serial.print("Ukuran Kartu : ");
      Serial.print(bytesToGB(cardSize), 2);
      Serial.println(" GB");

      Serial.print("Total Space  : ");
      Serial.print(bytesToGB(totalBytes), 2);
      Serial.println(" GB");

      Serial.print("Terpakai     : ");
      Serial.print(bytesToMB(usedBytes), 2);
      Serial.println(" MB");

      Serial.print("Sisa         : ");
      Serial.print(bytesToGB(freeBytes), 2);
      Serial.println(" GB");

      Serial.print("Sisa         : ");
      Serial.print(bytesToMB(freeBytes), 2);
      Serial.println(" MB");

      Serial.println("Status SD    : Terbaca");
    } else {
      Serial.println("Status SD    : Tidak terbaca");
      Serial.println("Cek wiring, format FAT32, atau tegangan modul SD");
    }

    Serial.println("========================");
    Serial.println();

    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

// =======================
// SETUP
// =======================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32 FreeRTOS Task Priority");
  Serial.println("LCD Priority 3");
  Serial.println("DHT Priority 2");
  Serial.println("SD Card Priority 1");

  dataMutex = xSemaphoreCreateMutex();

  if (dataMutex == NULL) {
    Serial.println("Gagal membuat mutex!");
    while (true);
  }

  // =======================
  // INIT LCD I2C
  // =======================
  Wire.begin(I2C_SDA_GPIO, I2C_SCL_GPIO);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("ESP32 Start");
  lcd.setCursor(0, 1);
  lcd.print("SD Capacity");
  delay(1500);
  lcd.clear();

  // =======================
  // INIT DHT
  // =======================
  dht.begin();

  // =======================
  // INIT SD CARD SPI
  // =======================
  sdSPI.begin(
    SD_SPI_SCLK_GPIO,
    SD_SPI_MISO_GPIO,
    SD_SPI_MOSI_GPIO,
    SD_SPI_CS_GPIO
  );

  if (SD.begin(SD_SPI_CS_GPIO, sdSPI)) {
    sdReady = true;
    Serial.println("SD Card berhasil terbaca");
  } else {
    sdReady = false;
    Serial.println("SD Card gagal terbaca");
  }

  // =======================
  // CREATE TASK PRIORITY
  // =======================
  xTaskCreatePinnedToCore(
    taskLCD,
    "LCD Task",
    4096,
    NULL,
    3,
    &lcdTaskHandle,
    1
  );

  xTaskCreatePinnedToCore(
    taskDHT,
    "DHT Task",
    4096,
    NULL,
    2,
    &dhtTaskHandle,
    1
  );

  xTaskCreatePinnedToCore(
    taskSDCard,
    "SD Card Task",
    4096,
    NULL,
    1,
    &sdTaskHandle,
    1
  );
}

void loop() {
  // Kosong karena semua proses dijalankan oleh task FreeRTOS
}
