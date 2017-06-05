#include <Arduino.h>
#include "gps.h"

#define NEMA_SEPERATOR ","
#define NMEA_MSG "GNGLL"

/* define NMEA configuration arrays */
/* first bit is length of array */
static uint8_t GGA_OFF[] = {16, 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xFF, 0x23};
static uint8_t GSA_OFF[] = {16, 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x01, 0x31};
static uint8_t GSV_OFF[] = {16, 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x02, 0x38};
static uint8_t RMC_OFF[] = {16, 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x03, 0x3F};
static uint8_t VTG_OFF[] = {16, 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x04, 0x46};
static uint8_t GRS_OFF[] = {16, 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x06, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x05, 0x4D};
static uint8_t TRS_10[] = {14, 0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01,
   0x00, 0x7A, 0x12};

static uint8_t *config[8] = {GGA_OFF, GSA_OFF, GSV_OFF, RMC_OFF, VTG_OFF, GRS_OFF, TRS_10, NULL};

// strtok_single behaves like strtok but doesn't treat multiple tokens as one delimiter
static char * strtok_single (char * str, char const * delims) {
  static char  * src = NULL;
  char  *  p,  * ret = 0;

  if (str != NULL)
    src = str;

  if (src == NULL)
    return NULL;

  if ((p = strpbrk (src, delims)) != NULL) {
    *p  = 0;
    ret = src;
    src = ++p;

  } else if (*src) {
    ret = src;
    src = NULL;
  }

  return ret;
}

// Process GPS parses a GPS Sentence to extract location information
// Example sentences (with starting $ and * stripped out by preparser)
// GNGLL,,,,,224439.00,V,N
// GNGLL,3518.33426,N,12039.89241,W,224513.00,A,A
int processGPS(char *in, struct GPSData *out) {
  char *token = NULL;

  token = strtok_single(in, NEMA_SEPERATOR);
  if(token == NULL)
    return 1;

  // Discard unwanted NEMA messages
  if(token != NULL && strcmp(token, NMEA_MSG))
    return 1;

  // Parse latitude
  token = strtok_single(NULL, NEMA_SEPERATOR);
  out->lat = (float) atof(token);

  // Parse hemisphere of message, if not N, assume southern hemisphere
  token = strtok_single(NULL, NEMA_SEPERATOR);
  if(token != NULL && strcmp(token, "N"))
    out->lat *= -1;

  // Parse longitude
  token = strtok_single(NULL, NEMA_SEPERATOR);
  out->lng = (float) atof(token);

  // Parse hemisphere of message, if not E, assume western hemisphere
  token = strtok_single(NULL, NEMA_SEPERATOR);
  if(token != NULL && strcmp(token, "E"))
    out->lng *= -1;

  // Parse time
  token = strtok_single(NULL, NEMA_SEPERATOR);
  out->time = (float) atof(token);

  // Parse fix status
  out->valid = false;
  token = strtok_single(NULL, NEMA_SEPERATOR);
  if(token != NULL && !strcmp(token, "A"))
    out->valid = true;

  return 0;
}

/* Returns a string intended for debug logging GPS data */
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

/* Write out configuration messages to the GPS serial line */
int setupGPSNMEAStrings(Stream *gps_port, Stream *debug_port) {
   /* wait for port to respond */
   int i = 0, j = 0;
   int arr_length;
   debug_port->print("Waiting for GPS...");
   while (gps_port->available() < 1);

   while ((config[i])) {
      arr_length = config[i][0];
      for(j = 1; j <= arr_length; j++) {
        gps_port->write(config[i][j]);
      }
      delay(10);                    /* delay because we can */
      i++;
   }
   flushGPS(gps_port);
   debug_port->print("done\n");
   return 1;
}

/* Flush GPS input buffer */
void flushGPS(Stream *gps_port) {
   char c;
   while(gps_port->available()) {
      c = gps_port->read();
   }
   return;
}
