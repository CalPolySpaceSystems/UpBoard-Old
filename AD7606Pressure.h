/*	
	CPSS Pressure Board Library
	Written by Patrick Chizek
	5/2017
*/
#ifndef _AD7606PRESSURE_H
#define _AD7606PRESSURE_H

#include <Arduino.h>

void initAD7606(int csPin, int convstPin, int resetPin);

uint16_t readAD7Raw(uint16_t rawADC[12]);

float readAD7Pressure(float pressureOut[6]);

#endif