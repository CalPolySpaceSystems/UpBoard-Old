#ifndef _BARO_H
#define _BARO_H

#include <Arduino.h>

// Barometer Configuration
#define BARO_PRESSURE 	(0x48)
#define BARO_TEMP 		(0x58)
#define BARO_ADDRESS 	(0x77)
#define BARO_RESET 		(0x1E)
#define BARO_PROM_READ 	(0xA2)

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
