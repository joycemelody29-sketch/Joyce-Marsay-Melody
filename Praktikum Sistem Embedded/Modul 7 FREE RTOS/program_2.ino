#include <Arduino.h>

// ================= PIN =================
#define LED1_PIN     2
#define LED2_PIN     4
#define BUZZER_PIN   5
#define SENSOR_PIN   34

// Variabel global
int sensorValue = 0;

// =====================================================
// TASK MEMBACA SENSOR
// =====================================================
void TaskSensor(void *pvParameters) {

  while (1) {

    sensorValue = analogRead(SENSOR_PIN);

    Serial.print("Sensor Value : ");
    Serial.println(sensorValue);

    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

// =====================================================
// TASK LED 1
// =====================================================
void TaskLED1(void *pvParameters) {

  while (1) {

    digitalWrite(LED1_PIN, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);

    digitalWrite(LED1_PIN, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// =====================================================
// TASK LED 2
// =====================================================
void TaskLED2(void *pvParameters) {

  while (1) {

    int delayTime = map(sensorValue, 0, 4095, 100, 1000);

    digitalWrite(LED2_PIN, HIGH);
    vTaskDelay(delayTime / portTICK_PERIOD_MS);

    digitalWrite(LED2_PIN, LOW);
    vTaskDelay(delayTime / portTICK_PERIOD_MS);
  }
}

// =====================================================
// TASK BUZZER
// =====================================================
void TaskBuzzer(void *pvParameters) {

  while (1) {

    if (sensorValue > 2000) {

      tone(BUZZER_PIN, 1000);
    }
    else {

      noTone(BUZZER_PIN);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// =====================================================
// SETUP
// =====================================================
void setup() {

  Serial.begin(115200);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Membuat task FreeRTOS
  xTaskCreate(
    TaskSensor,
    "Sensor",
    2048,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    TaskLED1,
    "LED1",
    2048,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    TaskLED2,
    "LED2",
    2048,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    TaskBuzzer,
    "Buzzer",
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
