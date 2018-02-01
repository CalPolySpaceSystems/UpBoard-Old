#ifndef _BARO_H
#define _BARO_H

#include <Arduino.h>

/* NOTE: all data reads must be primed an allow for a 10 ms
 *       delay after priming before reading.
 */

struct MS5611data {
  float temperature;
  float pressure;
  float altitude;
};

struct BAROMETER_packet {
    uint8_t id;
    uint16_t rtc;
    float temp;
    float pressure;
    float altitude;	
}__attribute__((packed));



void initMS5611(void);

// Temperature functions (must call primePressure before readTemp)
void primeTempMS5611(void);
void readTempMS5611(struct BAROMETER_packet *data);

// Pressure functions (must call primePressure before readPressure)
void primePressureMS5611(void);
void readPressureMS5611(struct BAROMETER_packet *data);

// Calculate altitude using data from MS5611data struct
void calcAltitudeMS5611(struct BAROMETER_packet *data);

// Convert MS5611 struct data to string for output
String ms5611ToString(struct BAROMETER_packet *data);
void buildPacketMS5611(struct BAROMETER_packet *data);

/* Reads the raw values based on the previous priming method (Temp or Pressure)
 * ONLY CALL THIS IF YOU MUST!
 * IT IS ALREADY IMPLEMENTED IN readTempMS6511 and readPressureMS5611
*/
uint32_t readRawMS5611();


#endif
