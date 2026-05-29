#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ======================
// PIN
// ======================
#define DHTPIN 4
#define DHTTYPE DHT22

#define BUTTON_PIN 18
#define LED_PIN 2

#define SDA_PIN 19
#define SCL_PIN 22

// ======================
// OBJECT
// ======================
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ======================
// GLOBAL VARIABLE
// ======================
float suhu = 0;
float hum = 0;

volatile bool buttonPressed = false;

// ======================
// MUTEX
// ======================
SemaphoreHandle_t dataMutex;
SemaphoreHandle_t lcdMutex;

// ======================
// INTERRUPT FUNCTION
// ======================
void IRAM_ATTR buttonISR() {

  buttonPressed = true;
}

// ======================
// TASK SENSOR
// PRIORITY 3
// ======================
void TaskSensor(void *pvParameters) {

  while (1) {

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h)) {

      xSemaphoreTake(dataMutex, portMAX_DELAY);

      suhu = t;
      hum = h;

      xSemaphoreGive(dataMutex);

      Serial.println("Sensor Updated");
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// ======================
// TASK LCD
// PRIORITY 2
// ======================
void TaskLCD(void *pvParameters) {

  while (1) {

    xSemaphoreTake(dataMutex, portMAX_DELAY);

    float t = suhu;
    float h = hum;

    xSemaphoreGive(dataMutex);

    if (xSemaphoreTake(lcdMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp:");
      lcd.print(t);
      lcd.print((char)223);
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("Hum :");
      lcd.print(h);
      lcd.print("%");
      xSemaphoreGive(lcdMutex);
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// ======================
// TASK LED + INTERRUPT
// PRIORITY 1
// ======================
void TaskLED(void *pvParameters) {

  while (1) {

    if (buttonPressed == true) {

      buttonPressed = false;

      Serial.println("BUTTON PRESSED");

      digitalWrite(LED_PIN, HIGH);

      if (xSemaphoreTake(lcdMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("BUTTON PRESSED");
        xSemaphoreGive(lcdMutex);
      }

      vTaskDelay(pdMS_TO_TICKS(1000));

      digitalWrite(LED_PIN, LOW);
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// ======================
// SETUP
// ======================
void setup() {

  Serial.begin(115200);

  // ======================
  // DHT START
  // ======================
  dht.begin();

  // ======================
  // LCD START
  // ======================
  Wire.begin(SDA_PIN, SCL_PIN);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("SYSTEM START");

  // ======================
  // LED
  // ======================
  pinMode(LED_PIN, OUTPUT);

  // ======================
  // BUTTON
  // ======================
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(
    digitalPinToInterrupt(BUTTON_PIN),
    buttonISR,
    FALLING
  );

  // ======================
  // MUTEX
  // ======================
  dataMutex = xSemaphoreCreateMutex();
  lcdMutex = xSemaphoreCreateMutex();

  // ======================
  // CREATE TASK
  // ======================

  xTaskCreatePinnedToCore(
    TaskSensor,
    "SensorTask",
    2048,
    NULL,
    3,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    TaskLCD,
    "LCDTask",
    2048,
    NULL,
    2,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    TaskLED,
    "LEDTask",
    2048,
    NULL,
    1,
    NULL,
    0
  );
}

// ======================
// LOOP
// ======================
void loop() {

}
