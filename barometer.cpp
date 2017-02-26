#include "barometer.h"
#include <Wire.h>
#include <math.h>

// Barometer Configuration
#define BARO_PRESSURE 	(0x48)
#define BARO_TEMP 		(0x58)
#define BARO_ADDRESS 	(0x77)
#define BARO_RESET 		(0x1E)
#define BARO_PROM_READ 	(0xA2)

// calibration data
uint16_t C[7] = {0};
int32_t dT;

// Initialize barometer from unknown state
void initMS5611(void) {
	// reset the barometer
	Wire.beginTransmission(BARO_ADDRESS);
	Wire.write(BARO_RESET);
	Wire.endTransmission();
	delay(10);
 
	for(uint8_t i = 0; i < 6; i++) {
		// write into PROM data
		Wire.beginTransmission(BARO_ADDRESS);
		Wire.write(BARO_PROM_READ + (i * 2));
		Wire.endTransmission();
		
		// read calibration data
		Wire.requestFrom(BARO_ADDRESS, (uint8_t) 2);
		while(Wire.available() == 0);
   
		C[i+1] = (Wire.read() << 8) | Wire.read();
    SerialUSB.print(C[i+1]);
	}
 delay(500);
	return;
}

// Primes MS5611 to organize temperature data for reading
void primeTempMS5611(void) {
	Wire.beginTransmission(BARO_ADDRESS);
	Wire.write(BARO_TEMP);
	Wire.endTransmission();
	return;
}

// Primes MS5611 to orgainze pressure data for reading
void primePressureMS5611(void) {
    Wire.beginTransmission(BARO_ADDRESS);
	Wire.write(BARO_PRESSURE);
	Wire.endTransmission();
	return;
}

// Reads the raw values based on the previous priming method (Temp or Pressure)
uint32_t readRawMS5611(void) {
	Wire.beginTransmission(BARO_ADDRESS);
	Wire.write(0);
	Wire.endTransmission();
	
	Wire.requestFrom(BARO_ADDRESS, 3);
	while(Wire.available() == 0)
		;
	return (((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 8) | (uint32_t)Wire.read());
}

/* Reads MS5611 temperature data into MS5611data struct
 * Assumptions:
 *     MS5611 primed for temperature
 *     10 ms delay after priming
 */
void readTempMS5611(struct MS5611data *data) {
	uint32_t raw_temp;
	int32_t temp;
	///int32_t dT;
 	
	raw_temp = readRawMS5611();
	
	dT = int32_t(raw_temp - (((uint32_t)C[5]) << 8));
	
	data->temperature = ((((int64_t)dT * (uint64_t)C[6]) >> 23) + 2000) / 100.0;

    return;
}

/* Reads MS5611 pressure data into MS5611data struct
 * Assumptions:
 *    MS5611 primed for pressure
 *    10 ms delay after priming
*/
void readPressureMS5611(struct MS5611data *data) {
    uint32_t raw_pressure;
    int64_t off;
    int64_t sens;
    //int32_t dT;

    //dT = ((data->temperature * 100.0) - 2000) / ((uint64_t)C[6] >> 23)

    raw_pressure = readRawMS5611();

    off  = ((uint64_t)C[2] << 16) + (((int64_t)dT * (uint64_t)C[4]) >> 7);
    sens = ((uint64_t)C[1] << 15) + (((int64_t)dT * (uint64_t)C[3]) >> 8);
 
    data->pressure = (int32_t)(((((uint64_t)raw_pressure * sens) >> 21) - off) >> 15);
  

    //off = ((uint64_t)C[2] << 16) + (((int64_t)dT * (uint64_t)C[4]) >> 7);
	//sens = ((uint64_t)C[1] << 15) + (((int64_t)dT * (uint64_t)C[3]) >> 8);
	
    //data->pressure = ((int32_t)(((((uint64_t)raw_pressure * sens) >> 21) - off) >> 15) / 100.0);
    data->pressure = data->pressure / 100.0;
    return;
}

// Calculate altitude using data from MS5611data struct
void calcAltitudeMS5611(struct MS5611data *data) {
    data->altitude = ((pow((1013.25 / data->pressure), 1/5.257) - 1.0) * (data->temperature + 273.15)) / 0.0065;
}

// Convert MS5611 struct data to string for output
String ms5611ToString(struct MS5611data *data) {
    String out = "Pressure = " + String(data->pressure);
    out += "\nTemperature = " + String(data->temperature);
    out += "\nAltitude = " + String(data->altitude);
    return out;
}
