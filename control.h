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

void initServo(Servo s);
void applyGyroOffsets(struct LSMData *ldata, struct LSMData *offset);
float compFilter(float oldAngle, float gyroData, float accAng, float dt);
bool detectedLaunch(struct LSMData *data);
#endif
