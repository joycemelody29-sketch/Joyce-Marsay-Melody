#include <Arduino.h>
#include <ESP32Servo.h>

// ================= PIN =================
#define POT_PIN     34
#define SERVO_PIN   18
#define LED_PIN     2

Servo myServo;

// Variabel global
int potValue = 0;
int servoAngle = 0;

// =====================================================
// TASK MEMBACA POTENSIOMETER
// =====================================================
void TaskReadPot(void *pvParameters) {

  while (1) {

    // Membaca nilai ADC
    potValue = analogRead(POT_PIN);

    // Konversi ke sudut servo
    servoAngle = map(potValue, 0, 4095, 0, 180);

    Serial.print("Potensio : ");
    Serial.print(potValue);

    Serial.print(" | Sudut Servo : ");
    Serial.println(servoAngle);

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// =====================================================
// TASK MENGGERAKKAN SERVO
// =====================================================
void TaskServo(void *pvParameters) {

  while (1) {

    myServo.write(servoAngle);

    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

// =====================================================
// TASK LED BLINK
// =====================================================
void TaskLED(void *pvParameters) {

  while (1) {

    int delayTime = map(potValue, 0, 4095, 100, 1000);

    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(delayTime / portTICK_PERIOD_MS);

    digitalWrite(LED_PIN, LOW);
    vTaskDelay(delayTime / portTICK_PERIOD_MS);
  }
}

// =====================================================
// SETUP
// =====================================================
void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  // Setup servo
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400);

  // Membuat task FreeRTOS
  xTaskCreate(
    TaskReadPot,
    "ReadPot",
    2048,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    TaskServo,
    "Servo",
    2048,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    TaskLED,
    "LED",
    2048,
    NULL,
    1,
    NULL
  );
}

// =====================================================
// LOOP
// =====================================================
void loop() {

}
