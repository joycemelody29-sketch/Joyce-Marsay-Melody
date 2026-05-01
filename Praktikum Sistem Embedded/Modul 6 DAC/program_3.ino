#include <Arduino.h>

#define SERVO_PIN 18
#define POT_PIN   34

// PWM config
#define PWM_FREQ 50      // 50 Hz (servo standard)
#define PWM_RES  16      // 16-bit resolution

// Duty range untuk servo (16-bit, 50Hz)
#define SERVO_MIN 1638   // ~0.5 ms
#define SERVO_MAX 8192   // ~2.5 ms

int readSmooth() {
    long total = 0;
    for (int i = 0; i < 10; i++) {
        total += analogRead(POT_PIN);
        delay(2);
    }
    return total / 10;
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    ledcSetup(0, PWM_FREQ, PWM_RES);
    ledcAttachPin(SERVO_PIN, 0);

    Serial.println("Servo + Potensiometer (STABIL)");
}

void loop() {
    int pot = readSmooth(); // smoothing ADC

    // pastikan tidak out of range
    pot = constrain(pot, 0, 4095);

    int duty = map(pot, 0, 4095, SERVO_MIN, SERVO_MAX);
    int angle = map(pot, 0, 4095, 0, 180);

    ledcWrite(0, duty);

    Serial.print("ADC: ");
    Serial.print(pot);
    Serial.print(" | Angle: ");
    Serial.print(angle);
    Serial.print(" | Duty: ");
    Serial.println(duty);

    delay(20); // sesuai 50Hz servo
}
