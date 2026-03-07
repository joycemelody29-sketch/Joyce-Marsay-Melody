/**
 * @file main.cpp
 * @brief Program 01: GPIO Interrupt + OLED SSD1306 - ESP32
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUTTON_PIN  4
#define LED_PIN     2

// OLED config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

volatile uint32_t risingCount = 0;
volatile uint32_t fallingCount = 0;
volatile bool stateChanged = false;

void IRAM_ATTR buttonISR() {
    if (digitalRead(BUTTON_PIN) == LOW) {
        fallingCount++;
    } else {
        risingCount++;
    }
    stateChanged = true;
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, CHANGE);

    // Inisialisasi OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED gagal start");
        while (true);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("GPIO Interrupt");
    display.println("ESP32 + OLED");
    display.display();

    Serial.println("System Ready");
}

void loop() {
    if (stateChanged) {
        stateChanged = false;

        Serial.printf("Rising: %lu, Falling: %lu\n", risingCount, fallingCount);

        digitalWrite(LED_PIN, !digitalRead(LED_PIN));

        // Update OLED
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Interrupt Counter");
        display.println("-----------------");
        display.print("Rising : ");
        display.println(risingCount);
        display.print("Falling: ");
        display.println(fallingCount);
        display.display();
    }
}
