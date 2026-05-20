#include <Arduino.h>
#include <ESP32Servo.h>

// ================= PIN =================
#define SENSOR_PIN   34
#define SERVO_PIN    18
#define LED_PIN      2

Servo myServo;

// Variabel global
int sensorValue = 0;
int servoAngle = 0;

// =====================================================
// TASK MEMBACA SENSOR
// =====================================================
void TaskSensor(void *pvParameters) {

  while (1) {

    sensorValue = analogRead(SENSOR_PIN);

    // Mapping sensor ke sudut servo
    servoAngle = map(sensorValue, 0, 4095, 0, 180);

    Serial.print("Sensor : ");
    Serial.print(sensorValue);

    Serial.print(" | Servo : ");
    Serial.println(servoAngle);

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// =====================================================
// TASK SERVO
// =====================================================
void TaskServo(void *pvParameters) {

  while (1) {

    myServo.write(servoAngle);

    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

// =====================================================
// TASK LED
// =====================================================
void TaskLED(void *pvParameters) {

  while (1) {

    if (sensorValue > 2000) {

      digitalWrite(LED_PIN, HIGH);
    }
    else {

      digitalWrite(LED_PIN, LOW);
    }

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

// =====================================================
// SETUP
// =====================================================
void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  // Setup Servo
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400);

  // Membuat Task FreeRTOS
  xTaskCreate(
    TaskSensor,
    "Sensor",
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
