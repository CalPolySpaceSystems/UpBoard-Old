#ifndef _BARO_H
#define _BARO_H

#include <Arduino.h>

struct BaroData {
	float Temperature;
	float Pressure;
	float Altitude;
}

// Main methods to call
void initBarometer(void);
void updateBaromResults(struct BaroData * data);

// Fine tuning
void primeBarometer(uint8_t data_to_get);
uint32_t readBarometerRaw();

#endif