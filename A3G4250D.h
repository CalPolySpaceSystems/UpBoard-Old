/*	
	CPSS 3DOF Library
	Written by Patrick Chizek
	5/2017
*/
#ifndef _A3G4250D_H
#define _A3G4250D_H

#include <Arduino.h>
#include <math.h>

void readRawA3G(int16_t *rawA3G);

void readA3G(float *outA3G);

void readFixIntA3G(int16_t *fpA3G);

void writeReg(byte deviceAddress, byte targetRegister, byte newValue);

bool checkReg(byte deviceAddress, byte targetRegister, byte expectedValue);

#endif
