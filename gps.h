#ifndef _GPS_H_
#define _GPS_H_

#include <Arduino.h>

struct GPS_packet {
  // Valid (check first)
  uint8_t valid;
  // Time of position (UTC)
  float time;
  // Latitude (+N, -S)
  float lat;
  // Longitude (+E, -W)
  float lng;
}__attribute__((packed));

/* Setup GPS message configuration, modify in gps.cpp */
int setupGPSNMEAStrings(USARTClass *gps_port, USARTClass *debug_port);
void flushGPS(USARTClass *gps_port);
int processGPS(struct GPS_packet *out);
uint8_t readGPS(USARTClass *gps_port);
String gpsToString(struct GPS_packet *data);

#endif

