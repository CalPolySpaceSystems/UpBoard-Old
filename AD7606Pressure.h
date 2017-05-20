#ifndef _AD7606PRESSURE_H
#define _AD7606PRESSURE_H

#include <Arduino.h>

void initAD7606(int SENSOR_CS,SENSOR_CONVST,SENSOR_RESET);

float* readAD7Pressure(void); 

#endif