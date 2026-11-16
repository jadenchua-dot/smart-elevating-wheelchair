// Smart Elevating Wheelchair - Arduino Uno
//
// C++ version of the Tinkercad block program (docs/images/tinkercad-blocks.jpg).
// It does exactly what the blocks do:
//
//   Button on D2 pressed  -> servo on D9 turns to 90 deg (seat up), wait 1 s
//   Button on D3 pressed  -> servo on D9 turns to 0 deg  (seat down), wait 1 s
//   Ultrasonic (trig D10, echo D12) sees something closer than 25 cm
//                          -> D6 on, D7 beeps (on 200 ms, off 200 ms, on 200 ms)
//   Otherwise              -> D6 and D7 off
//
// The servo turns a Lego gear that drives the scissor lift under the seat.
// The ultrasonic sensor faces backwards, to warn about obstacles behind the
// wheelchair while the seat is raised.
//
// Wiring: both buttons use pull-down resistors, so a press reads HIGH.

#include <Servo.h>

// Pins (same as the Tinkercad circuit)
const int BTN_UP_PIN   = 2;    // raise the seat
const int BTN_DOWN_PIN = 3;    // lower the seat
const int LED_PIN      = 6;    // stays on while an obstacle is close
const int BUZZER_PIN   = 7;    // beeps while an obstacle is close
const int SERVO_PIN    = 9;
const int TRIG_PIN     = 10;
const int ECHO_PIN     = 12;

// Settings
const int  SEAT_UP_DEG   = 90;
const int  SEAT_DOWN_DEG = 0;
const long WARN_CM       = 25;   // the "software filter": only react to obstacles closer than this

Servo seatServo;

// Distance in cm from the HC-SR04, or -1 if no echo came back.
long readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long us = pulseIn(ECHO_PIN, HIGH, 30000);   // time out after 30 ms (about 5 m)
  if (us == 0) return -1;
  return us * 0.01723;                        // same conversion Tinkercad uses
}

void setup() {
  pinMode(BTN_UP_PIN, INPUT);
  pinMode(BTN_DOWN_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  seatServo.attach(SERVO_PIN);

  // "on start" blocks: warning off
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  // Seat up
  if (digitalRead(BTN_UP_PIN) == HIGH) {
    seatServo.write(SEAT_UP_DEG);
    delay(1000);
  }

  // Seat down
  if (digitalRead(BTN_DOWN_PIN) == HIGH) {
    seatServo.write(SEAT_DOWN_DEG);
    delay(1000);
  }

  // Obstacle warning
  long cm = readDistanceCm();
  if (cm >= 0 && cm < WARN_CM) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
}
