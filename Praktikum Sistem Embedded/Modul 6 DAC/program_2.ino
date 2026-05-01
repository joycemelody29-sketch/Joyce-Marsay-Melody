#include <Arduino.h>
#include <Servo.h>

Servo myServo;

#define SERVO_PIN PA0
#define POT_PIN   PA1

// smoothing ADC
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

    pinMode(POT_PIN, INPUT);
    myServo.attach(SERVO_PIN, 500, 2400); // penting untuk stabilitas servo

    Serial.println("Servo Control Ready");
}

void loop() {
    int potValue = readSmooth();                  // baca potensio (0–4095)
    int angle = map(potValue, 0, 4095, 0, 180);   // ubah ke sudut

    // dead zone biar tidak jitter
    static int lastAngle = -1;
    if (abs(angle - lastAngle) > 2) {
        myServo.write(angle);
        lastAngle = angle;
    }

    Serial.print("ADC: ");
    Serial.print(potValue);
    Serial.print(" | Angle: ");
    Serial.println(angle);

    delay(20); // sesuai frekuensi servo (~50Hz)
}
