#include <Arduino.h>
#include "gps.h"

// processGPS converts a GPGLL  sentance into GPSData struct. Example sentance:
// GPGLL,0000.0000,N,00000.0000,E,193602.000,V,N*4E
// Only works with 48 charecter long GPGLL sentances for now
void processGPS(uint8_t *in, struct GPSData *out) {
  char lat[10];
  char lng[10];
  char time[11];

  if((char) in[42] != 'A') {
    out->valid = false;
  } else {
    out->valid = true;
  }

  for(int i = 0; i < 9; i++) {
    lat[i] = in[6+i];
  }
  // null terminate string
  lat[9] = '\0';

  for(int i = 0; i < 9; i++) {
    lng[i] = in[18+i];
  }
  // null terminate string
  lng[9] = '\0';

  for(int i = 0; i < 10; i++) {
    time[i] = in[31+i];
  }
  // null terminate string
  time[10] = '\0';

  out->lat = (float) atof(lat);
  out->lng = (float) atof(lng);
  out->time = (float) atof(time);

  if((char) in[16] != 'N') {
    out->lat *= -1;
  }

  if((char) in[29] != 'E') {
    out->lng *= -1;
  }
}

// Returns a string intended for debug logging GPS data;
String gpsToString(struct GPSData *data) {
  String out = "Time";
  out += String(data->time);
  out += "\nLat: ";
  out += String(data->lat);
  out += "\nLng: ";
  out += String(data->lng);
  out += "Valid: ";
  out += data->valid;
  out += "\n";
  return out;
}
