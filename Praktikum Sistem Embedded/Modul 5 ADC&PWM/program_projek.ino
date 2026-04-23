#include <Arduino.h>
#include <Keypad.h>

#define POT_PIN PA0
#define LED_PIN PA1
#define BTN_PIN PA3

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {PB0, PB1, PB10, PB11};
byte colPins[COLS] = {PB12, PB13, PB14, PB15};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int mode = 1;

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
}

void loop() {

  // =========================
  // KEY PAD MODE CONTROL
  // =========================
  char key = keypad.getKey();

  if (key) {
    if (key == '1') mode = 1;
    if (key == '2') mode = 2;
    if (key == '3') mode = 3;

    Serial.printf("MODE = %d\n", mode);
  }

  int pot = analogRead(POT_PIN);
  bool btn = digitalRead(BTN_PIN);

  int led = 0;

  // =========================
  // MODE 1 = PUSH BUTTON
  // =========================
  if (mode == 1) {
    if (btn == LOW) {
      led = 255;   // ditekan → nyala
    } else {
      led = 0;     // tidak ditekan → mati
    }
  }

  // =========================
  // MODE 2 = POTENSIOMETER
  // =========================
  else if (mode == 2) {
    led = map(pot, 0, 4095, 0, 255);
  }

  // =========================
  // MODE 3 = ON TERUS
  // =========================
  else if (mode == 3) {
    led = 255;
  }

  analogWrite(LED_PIN, led);
}




PROGRAM PROJEK
