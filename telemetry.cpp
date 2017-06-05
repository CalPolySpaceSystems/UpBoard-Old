#include <Arduino.h>
#include <string.h>
#include "telemetry.h"

#define QUEUE_LEN 255
#define MSG_LEN 255

#define FIXED_PT_4_12 4096
#define FIXED_PT_6_10 1024
#define FIXED_PT_8_8 256
#define FIXED_PT_12_4 16
#define FIXED_PT_14_2 4

static char queue[QUEUE_LEN][MSG_LEN];
static uint16_t len = 0;
static uint16_t head = 0;

// C's % is remainder, so this adds modulo support
static int mod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}


// packet enqueue returns a pointer to a spot in the queue to store data
char* packetEnqueue() {
  uint8_t pos = head;
  head++;
  head %= QUEUE_LEN;

  if(len < QUEUE_LEN)
    len++;

  return queue[pos];
}

char* packetDequeue() {
  if(len == 0) {
    return NULL;
  }

  char* out = queue[mod(head-len, QUEUE_LEN)];
  len--;
  return out;
}


void sendTelemetry(Stream *s, int amount) {
  static struct {
    int pos;
    int len;
    char msg[MSG_LEN];
  } pack;

  while(amount > 0) {
    // if the current packet is empty, load a new one
    if(pack.pos >= pack.len) {
      const char* newPacket = packetDequeue();
      if(newPacket == NULL)
        break;
      strcpy(pack.msg, newPacket);
      pack.pos = 0;
      pack.len = MSG_LEN; // TODO: Add smarter length detection
    }

    s->write(pack.msg[pack.pos++]);
    amount--;
  }
}

// createPacket generates a telemetry packet. See documentation for the format.
int createPacket(char* out, struct LSMData *ldata, struct GPSData *gdata, struct MS5611data *mdata) {
  int pos = 0;
  // Header
  out[pos++] = 'C';

  // RTC
  union {
    uint16_t i;
    char c[2];
  } rtc;
  rtc.i = 1234;
  for(int i = sizeof(rtc.c)-1; i > -1; i--)
    out[pos++] = rtc.c[i];

  // Time
  union {
    uint32_t i;
    char c[4];
  } time;
  time.i = (uint32_t) (gdata->time*100+0.5);
  for(int i = sizeof(time.c)-1; i > -1; i--)
    out[pos++] = time.c[i];

  // Latitude
  union {
    uint32_t i;
    char c[4];
  } lat;
  lat.i = (uint32_t) (fabs(gdata->lat)*100000+0.5);
  for(int i = sizeof(lat.c)-1; i > -1; i--)
    out[pos++] = lat.c[i];

  // Longitude
  union {
    uint32_t i;
    char c[4];
  } lng;
  lng.i = (uint32_t) (fabs(gdata->lng)*100000+0.5);
  for(int i = sizeof(lng.c)-1; i > -1; i--)
    out[pos++] = lng.c[i];

  // Temperature
  union {
    uint16_t i;
    char c[2];
  } temp;
  temp.i = (uint16_t) (ldata->temp * FIXED_PT_12_4);
  for(int i = sizeof(temp.c)-1; i > -1; i--)
    out[pos++] = temp.c[i];

  // Accelerometer
  for(int i = 0; i < 3; i++) {
    union {
      uint16_t i;
      char c[2];
    } acc;
    acc.i = (uint16_t) (ldata->acc[i] * FIXED_PT_6_10);
    for(int j = sizeof(acc.c)-1; j > -1; j--)
      out[pos++] = acc.c[j];
  }

  // Gyro
  for(int i = 0; i < 3; i++) {
    union {
      uint16_t i;
      char c[2];
    } gyr;
    gyr.i = (uint16_t) (ldata->gyr[i] * FIXED_PT_12_4);
    for(int j = sizeof(gyr.c)-1; j > -1; j--)
      out[pos++] = gyr.c[j];
  }

  // Magnetometer
  for(int i = 0; i < 3; i++) {
    union {
      uint16_t i;
      char c[2];
    } mag;
    mag.i = (uint16_t) (ldata->mag[i] * FIXED_PT_4_12);
    for(int j = sizeof(mag.c)-1; j > -1; j--)
      out[pos++] = mag.c[j];
  }

  // Pressure
  union {
    uint16_t i;
    char c[2];
  } pres;
  pres.i = (uint16_t) (mdata->pressure * FIXED_PT_8_8);
  for(int i = sizeof(pres.c)-1; i > -1; i--)
    out[pos++] = pres.c[i];

  // Altitude
  union {
    uint16_t i;
    char c[2];
  } alt;
  alt.i = (uint16_t) (mdata->altitude * FIXED_PT_14_2);
  for(int i = sizeof(alt.c)-1; i > -1; i--)
    out[pos++] = alt.c[i];

  // Placeholder for pressure tap data
  for(int i = 0; i < 12; i++)
    out[pos++] = '\0';

  out[pos++] = '*'; // Footer

  char checksum = 0;
  // Calculate checksum
  for(int i = 0; i < pos; i++) {
    checksum ^= out[i];
  }

  out[pos++] = checksum;

  // NOTE(Joshua): IF YOU CHANGE THIS, YOU MUST UPDATE MSG_LEN
  // Telemetry packets currently are hard coded to this length. It needs to be redesigned in the future.
  // Since binary packets can have \0 in them, we can no longer simply use strlen to determine length.

  return pos;
}
