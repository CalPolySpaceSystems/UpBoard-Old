#include <Arduino.h>
#include <Servo.h>
#include "control.h"

#define SERVO_MIDDLE 1500
#define SERVO_DELAY 500

// InitServo moves the servo around the middle to help it center
void InitServo(Servo s)
{
  s.writeMicroseconds(SERVO_MIDDLE);
  delay(SERVO_DELAY);
  s.writeMicroseconds(SERVO_MIDDLE-45*10);
  delay(SERVO_DELAY);
  s.writeMicroseconds(SERVO_MIDDLE+45*10);
  delay(SERVO_DELAY*2); // TODO: Any reason for this delay being longer
  s.writeMicroseconds(SERVO_MIDDLE);
  delay(SERVO_DELAY);
}
