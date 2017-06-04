#ifndef _GPS_H_
#define _GPS_H_

#include <Arduino.h>

struct GPSData {
  // Valid (check first)
  boolean valid;
  // Latitude (+N, -S)
  float lat;
  // Longitude (+E, -W)
  float lng;
  // Time of position (UTC)
  float time;
};

/* Setup GPS message configuration, modify in gps.cpp */
int setupGPSNMEAStrings(Stream *gps_port, Stream *debug_port);
void flushGPS(Stream *gps_port);
int processGPS(char *in, struct GPSData *out);
String gpsToString(struct GPSData *data);
#endif
