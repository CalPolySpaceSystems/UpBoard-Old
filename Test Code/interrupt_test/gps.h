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

void processGPS(uint8_t *in, struct GPSData *out);


/* Returns a string intended for debug logging GPS data */
String gpsToString(struct GPSData *data);

/* Write out configuration messages to the GPS serial line */
int setupGPSNMEAStrings(Stream *gps_port, Stream *debug_port);

/* Flush GPS input buffer */
void flushGPS(Stream *gps_port);
