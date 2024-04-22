#include <ESP32Servo.h>

Servo myServo;
int servoPin = 4;
int DOOR_OPEN = 30;
int DOOR_CLOSE = 0;

void setup() {
  myServo.attach(servoPin);
}

void loop() {
  myServo.write(DOOR_OPEN);
  delay(1000);
  myServo.write(DOOR_CLOSE);
  delay(1000);
}
