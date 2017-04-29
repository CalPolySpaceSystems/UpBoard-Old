#ifndef _TELEMETRY_H_
#define _TELEMETRY_H_

#include <Arduino.h>
#include "gps.h"
#include "lsm9ds1.h"
#include "barometer.h"

void createPacket(char* out, struct LSMData *ldata, struct GPSData *gdata, struct MS5611data *mdata);
#endif

