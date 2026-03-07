/**
 * @file main.cpp
 * @brief Internal Timer Interrupt + OLED SSD1306 (2 Detik)
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED_PIN 2

// OLED config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Timer
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t interruptCount = 0;
volatile bool timerFlag = false;

// ISR Timer
void IRAM_ATTR onTimer() {
    portENTER_CRITICAL_ISR(&timerMux);
    interruptCount++;
    timerFlag = true;
    portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("Internal Timer Interrupt + OLED");

    pinMode(LED_PIN, OUTPUT);

    // Inisialisasi OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED gagal start");
        while (true);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Timer 0, prescaler 80 → 1 tick = 1 µs
    timer = timerBegin(0, 80, true);

    // 2.000.000 µs = 2 detik
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 2000000, true);
    timerAlarmEnable(timer);

    Serial.println("Timer interrupt setiap 2 detik aktif");
}

void loop() {
    if (timerFlag) {
        portENTER_CRITICAL(&timerMux);
        timerFlag = false;
        uint32_t count = interruptCount;
        portEXIT_CRITICAL(&timerMux);

        // Toggle LED
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));

        // Serial
        Serial.printf("Interrupt ke-%lu\n", count);

        // Update OLED
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Internal Interrupt");
        display.println("------------------");
        display.print("Count : ");
        display.println(count);
        display.print("LED   : ");
        display.println(digitalRead(LED_PIN) ? "ON" : "OFF");
        display.display();
    }
}
