#include <Arduino.h>
#include "gps.h"

/* define NMEA configuration arrays */
static uint8_t GGA_OFF[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xFF, 0x23};
static uint8_t GSA_OFF[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x01, 0x31};
static uint8_t GSV_OFF[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x02, 0x38};
static uint8_t RMC_OFF[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x03, 0x3F};
static uint8_t VTG_OFF[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x04, 0x46};
static uint8_t GRS_OFF[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x06, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x05, 0x4D};
static uint8_t TRS_10[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 
   0x00, 0x7A, 0x12, 0x00, 0x00};

static uint8_t *config = {GGA_OFF, GSA_OFF, GSV_OFF, RMC_OFF, VTG_OFF, GRS_OFF, TRS_10, NULL};


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
  out += "\nValid: ";
  out += data->valid;
  out += "\n";
  return out;
}

int setupGPSNMEAStrings(HardwareSerial *gps_port, HardwareSerial *debug_port) {
   /* wait for port to respond */
   uint8_t **ptr = config;

   debug_port->print("Waiting for GPS..."); 
   while (gps_port->available() < 1);

   while ((*ptr)) {
      gps_port->write(*ptr, 16);    /* iteratively write out config bytes */
      delay(10);                    /* delay because we can */ 
      ptr++;                        /* prepare next configuration array */
   }
   return 1;
}
