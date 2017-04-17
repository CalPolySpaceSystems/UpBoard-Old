#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <Arduino.h>
#include <Servo.h>


struct state
{
  float angle[3];
  bool launched;
  float dt;
};

void InitServo(Servo s);
#endif
