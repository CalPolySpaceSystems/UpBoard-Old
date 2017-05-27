#ifndef _AD7606PRESSURE_H
#define _AD7606PRESSURE_H

#include <Arduino.h>

void initAD7606(int csPin; int convstPin; int resetPin);

uint16_t* readAD7Raw(void);

float* readAD7Pressure(void); 

#endif