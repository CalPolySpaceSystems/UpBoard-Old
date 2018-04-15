#ifndef _BARO_H
#define _BARO_H

#include <Arduino.h>

/* NOTE: all data reads must be primed an allow for a 10 ms
 *       delay after priming before reading.
 */

struct MS56XXdata {
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



void initMS56XX(void);

// Temperature functions (must call primePressure before readTemp)
void primeTempMS56XX(void);
void readTempMS56XX(struct BAROMETER_packet *data);

// Pressure functions (must call primePressure before readPressure)
void primePressureMS56XX(void);
void readPressureMS56XX(struct BAROMETER_packet *data);

// Calculate altitude using data from MS56XXdata struct
void calcAltitudeMS56XX(struct BAROMETER_packet *data);

// Convert MS56XX struct data to string for output
String MS56XXToString(struct BAROMETER_packet *data);
void buildPacketMS56XX(struct BAROMETER_packet *data);

/* Reads the raw values based on the previous priming method (Temp or Pressure)
 * ONLY CALL THIS IF YOU MUST!
 * IT IS ALREADY IMPLEMENTED IN readTempMS6511 and readPressureMS56XX
*/
uint32_t readRawMS56XX();


#endif

