#include <Arduino.h>
#include <Servo.h>
#include "control.h"
#include "lsm9ds1.h"

#define SERVO_MIDDLE 1500
#define SERVO_DELAY 500
#define LAUNCH_ACCELERATION 1.5 // TODO: Determine what is a reasonable value for this

// InitServo moves the servo around the middle to help it center
void initServo(Servo s)
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

// Offsets input gyro data by offset
void applyGyroOffsets(struct LSMData *ldata, struct LSMData *offset)
{
  ldata->acc[0] = ldata->acc[0] - offset->acc[0];
  ldata->acc[1] = ldata->acc[1] - offset->acc[1];
  ldata->acc[2] = ldata->acc[2] - offset->acc[2];
  ldata->gyr[0] = ldata->gyr[0] - offset->gyr[0];
  ldata->gyr[1] = ldata->gyr[1] - offset->gyr[1];
  ldata->gyr[2] = ldata->gyr[2] - offset->gyr[2];
}


float compFilter(float oldAngle, float gyroData, float accAng, float dt)
{
  return 0.99 * (oldAngle + gyroData * dt) + 0.01 * accAng;
}

bool detectedLaunch(struct LSMData *data)
{
  if (data->acc[2] > LAUNCH_ACCELERATION) {
    return true;
  }
  return false;
}
