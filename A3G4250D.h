// I2C Library for STM A3G4250D
// Patrick Chizek
// Written for CPSS, 6/2017

#ifndef _A3G4250D_H
#define _A3G4250D_H

#include <Arduino.h>
#include <math.h>

void initA3G(void); // Sets up the gyro to nominal settings

void readRawA3G(int16_t *rawA3G); // Returns raw readings as array of 16-bit signed integers

void readA3G(float *outA3G); // Returns readings in floating point degrees per second

void readFixIntA3G(int16_t *fpA3G); // Returns a fixed point representation of reading in DPS

#endif
