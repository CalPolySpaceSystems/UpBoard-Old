#include <barometer.h>
#include <Wire.h>
#include <math.h>


// calibration data
uint16_t C[7] = {0};

// Initialize barometer from unknown state
void initBarometer(void) {
	// reset the barometer
	Wire.beginTransmission(BARO_ADDRESS);
	Wire.write(BARO_RESET);
	Wire.endTransmission();
	
	for(uint8_t i = 0; i < 6; i++) {
		// write into PROM data
		Wire.beginTransmission(BARO_ADDRESS);
		Wire.write(BARO_PROM_READ + (i << 1));
		Wire.endTransmission();
		
		// read calibration data
		Wire.requestFrom(BARO_ADDRESS, (uint8_t) 2);
		while(Wire.available() == 0)
			;
		C[i+1] = (Wire.read() << 8) | Wire.read();
	}
	return;
}

// Primes barometer to organize relevant data for reading
void primeBarometer(uint8_t data_to_get) {
	Wire.beginTransmission(BARO_ADDRESS);
	Wire.write(data_to_get);
	Wire.endTransmission();
	return;
}


// Reads the raw values based on the previous priming method (Temp or Pressure)
uint32_t readBarometerRaw() {
	Wire.beginTransmission(BARO_ADDRESS);
	Wire.write(0);
	Wire.endTransmission();
	
	Wire.requestFrom(BARO_ADDRESS, 3);
	while(Wire.available() == 0)
		;
	return (((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 8) | (uint32_t)Wire.read());
}

// Updates barometer struct with new readings
// uses datasheet methods to obtain pressure, temperature, and alititude readings
// tons of fun going on here with magic numbers from datasheet
void updateBaromResults(struct BaroData * data) {
	uint32_t raw_pressure;
	uint32_t raw_temp;
	int32_t dT;
	int32_t TEMP;
	int64_t OFF;
	int64_t SENS;
	int32_t P;
	
	primeBarometer(BARO_PRESSURE);
	raw_pressure = readBarometerRaw();
	
	primeBarometer(BARO_TEMP);
	raw_temp = readBarometerRaw();
	
	dT = int32_t(raw_temp - (((uint32_t)C[5]) << 8));
	off = ((uint64_t)C[2] << 16) + (((int64_t)dT * (uint64_t)C[4]) >> 7);
	sens = ((uint64_t)C[1] << 15) + (((int64_t)dT * (uint64_t)C[3]) >> 8);
	
	temp = ((int64_t)dT * (uint64_t)C[6]) >> 23;
	temp = temp + 2000;
	
	P = (int32_t)(((((uint64_t)raw_pressure * sens) >> 21) - off) >> 15);
	data->Temperature = temp / 100.0;
	data->Pressure = P / 100.0;
	data->Altitude = ((pow((1013.25 / Pressure), 1/5.257) - 1.0) * (Temperature + 273.15)) / 0.0065;
	return;
}
