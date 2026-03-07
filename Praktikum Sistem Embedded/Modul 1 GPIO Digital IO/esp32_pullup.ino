/**
 * @file main.cpp
 * @brief Program 04: Button Debounce State Machine - STM32
 */

#include <Arduino.h>

// ==================== KONFIGURASI ====================
#define BUTTON_PIN      PA2    // Push button (INPUT_PULLUP)
#define LED_PIN         PA1     // External LED (active HIGH)
#define DEBOUNCE_MS     50
#define LED_SELF_TEST_BLINKS 6

#define BUTTON_INPUT_MODE     INPUT_PULLUP
#define BUTTON_ACTIVE_STATE   LOW
#define BUTTON_INACTIVE_STATE HIGH

// ==================== STATE MACHINE ====================
typedef enum {
    BTN_IDLE,
    BTN_DEBOUNCE,
    BTN_PRESSED,
    BTN_RELEASED
} ButtonState_t;

// ==================== VARIABEL ====================
ButtonState_t buttonState = BTN_IDLE;
bool lastButtonRead = BUTTON_INACTIVE_STATE;
unsigned long debounceStartTime = 0;
unsigned long pressStartTime = 0;
uint32_t pressCount = 0;
bool ledState = false;

// ==================== SETUP ====================
void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n========================================");
    Serial.println("Program 04: Button Debounce - STM32");
    Serial.println("========================================\n");
    
    pinMode(BUTTON_PIN, BUTTON_INPUT_MODE);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);   // LED OFF (active HIGH)

    Serial.printf("Button: PB0 (INPUT_PULLUP), LED: PB1 (active HIGH)\n");
    Serial.println("Button mode: INTERNAL PULL-UP (ACTIVE LOW)");
    Serial.println("Startup LED self-test on PB1...");

    for (int i = 0; i < LED_SELF_TEST_BLINKS; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(200);
    }

    Serial.println("Self-test done. Press button to toggle LED\n");
}

// ==================== LOOP ====================
void loop() {
    bool currentRead = digitalRead(BUTTON_PIN);
    
    switch (buttonState) {
        case BTN_IDLE:
            if (currentRead == BUTTON_ACTIVE_STATE &&
                lastButtonRead == BUTTON_INACTIVE_STATE) {
                buttonState = BTN_DEBOUNCE;
                debounceStartTime = millis();
            }
            break;
            
        case BTN_DEBOUNCE:
            if (millis() - debounceStartTime >= DEBOUNCE_MS) {
                if (currentRead == BUTTON_ACTIVE_STATE) {
                    buttonState = BTN_PRESSED;
                    pressStartTime = millis();
                    
                    // Toggle LED (active HIGH)
                    pressCount++;
                    ledState = !ledState;
                    digitalWrite(LED_PIN, ledState);
                    
                    Serial.printf("Button PRESSED #%lu - LED %s\n",
                                 pressCount, ledState ? "ON" : "OFF");
                } else {
                    buttonState = BTN_IDLE;
                }
            }
            break;
            
        case BTN_PRESSED:
            if (currentRead == BUTTON_INACTIVE_STATE) {
                buttonState = BTN_RELEASED;
                debounceStartTime = millis();
            }
            break;
            
        case BTN_RELEASED:
            if (millis() - debounceStartTime >= DEBOUNCE_MS) {
                if (currentRead == BUTTON_INACTIVE_STATE) {
                    unsigned long duration = millis() - pressStartTime;
                    Serial.printf("Button RELEASED - Duration: %lu ms\n\n", duration);
                    buttonState = BTN_IDLE;
                } else {
                    buttonState = BTN_PRESSED;
                }
            }
            break;
    }
    
    lastButtonRead = currentRead;
}

/**
 * WIRING BUTTON (INTERNAL PULL-UP / ACTIVE LOW):
 *   PB0 ----[Button]---- GND
 *   (pinMode PB0 = INPUT_PULLUP)
 *
 * WIRING LED EKSTERNAL (ACTIVE HIGH):
 *   PB1 ----[220Ω..1kΩ]----|>|---- GND
 *   (Anoda LED ke PB1 via resistor, katoda ke GND)
 */
