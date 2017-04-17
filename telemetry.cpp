#include <Arduino.h>
#include <string.h>
#include "telemetry.h"

// fToA converts a float to a char array
void fToA(char* out, float in) {
  char temp[32]; // TODO: figure out right length
  int32_t whole = (int32_t) in;    //truncate whole numbers
  int16_t frac =  (int16_t) ((float)(in - (float) whole)*10000); //remove whole part of flt and shift 5 places over
  if(frac < 0) {
  frac *= -1;
  }

  itoa(whole, temp, 10);
  strcat(out, temp);
  strcat(out, ".");
  itoa(frac, temp, 10);
  strcat(out, temp);
}

// createPacket generates a telemetry packet.
// LEN includes every charecter up to the start of length.
// Format $CPSS,TIME,MILLI,LAT,LNG,GPSVALID,ALT,PRESSURE,MS5_TEMP,ACC_X,ACC_Y,ACC_Z,GYR_X,GYR_Y,GYR_Z,MAG_X,MAG_Y,MAG_Z,LMS_TEMP;LEN,CHECKSUM*
// Example packet: $1928.3746,1234.5677,9876.5429,0,5678.9013,50.6780,10.5677,-4.0,5.6700,6.2300,34.0,3.4500,67.8899,54.0,3.7500,53.8899,-110.9000;128,84
void createPacket(char* out, struct LSMData *ldata, struct GPSData *gdata, struct MS5611data *mdata, struct state *state) {
  // set header and initial null byte
  out[0] = '$';
  out[1] = '\0';

  strcat(out, "CPSS,");

  // time
  char tmp[32] = "";
  fToA(tmp, gdata->time);
  strcat(out, tmp);
  strcat(out, ",");

  // milliseconds since startup
  tmp[0] = '\0';
  itoa(millis(), tmp, 10);
  strcat(out, tmp);
  strcat(out, ",");

  // lat
  tmp[0] = '\0';
  fToA(tmp, gdata->lat);
  strcat(out, tmp);
  strcat(out, ",");

  // lng
  tmp[0] = '\0';
  fToA(tmp, gdata->lng);
  strcat(out, tmp);
  strcat(out, ",");

  // gps valid
  if(gdata->valid)
  strcat(out, "1");
  else
  strcat(out, "0");
  strcat(out, ",");

  // alt
  tmp[0] = '\0';
  fToA(tmp, mdata->altitude);
  strcat(out, tmp);
  strcat(out, ",");

  // pressure
  tmp[0] = '\0';
  fToA(tmp, mdata->pressure);
  strcat(out, tmp);
  strcat(out, ",");

  // barometer temperature
  tmp[0] = '\0';
  fToA(tmp, mdata->temperature);
  strcat(out, tmp);
  strcat(out, ",");

  // accelerometer x
  tmp[0] = '\0';
  fToA(tmp, ldata->acc[0]);
  strcat(out, tmp);
  strcat(out, ",");

  // accelerometer y
  tmp[0] = '\0';
  fToA(tmp, ldata->acc[1]);
  strcat(out, tmp);
  strcat(out, ",");

  // accelerometer z
  tmp[0] = '\0';
  fToA(tmp, ldata->acc[2]);
  strcat(out, tmp);
  strcat(out, ",");

  // gyro x
  tmp[0] = '\0';
  fToA(tmp, ldata->gyr[0]);
  strcat(out, tmp);
  strcat(out, ",");

  // gyro y
  tmp[0] = '\0';
  fToA(tmp, ldata->gyr[1]);
  strcat(out, tmp);
  strcat(out, ",");

  // gyro z
  tmp[0] = '\0';
  fToA(tmp, ldata->gyr[2]);
  strcat(out, tmp);
  strcat(out, ",");

  // magnetometer x
  tmp[0] = '\0';
  fToA(tmp, ldata->mag[0]);
  strcat(out, tmp);
  strcat(out, ",");

  // magnetometer y
  tmp[0] = '\0';
  fToA(tmp, ldata->mag[1]);
  strcat(out, tmp);
  strcat(out, ",");

  // magnetometer z
  tmp[0] = '\0';
  fToA(tmp, ldata->mag[2]);
  strcat(out, tmp);
  strcat(out, ",");

  // 9-axis tempertare
  //tmp[0] = '\0';
  //fToA(tmp, ldata->temp);
  //strcat(out, tmp);
  //strcat(out, ";");

    // Time State
    tmp[0] = '\0';
    fToA(tmp, state->dt);
    strcat(out, tmp);
    strcat(out, ",");

    // Angle x
    tmp[0] = '\0';
    fToA(tmp, state->angle[0]);
    strcat(out, tmp);
    strcat(out, ",");

    // Angle y
    tmp[0] = '\0';
    fToA(tmp, state->angle[1]);
    strcat(out, tmp);
    strcat(out, ",");

    // Angle z
  //  tmp[0] = '\0';
    //fToA(tmp, state->angle[2]);
    //strcat(out, tmp);
    //strcat(out, ",");

    // Launched
    tmp[0] = '\0';
    fToA(tmp, char(state->launched));
    strcat(out, tmp);
    strcat(out, ",");

  strcat(out, ";");

  // set message length
  tmp[0] = '\0';
  itoa(strlen(out), tmp, 10);
  strcat(out, tmp);
  strcat(out, ",");

  // calculate checksum
  int cs = 0;
  uint8_t pos = 0;
  while(out[pos] != '\0') {
  cs ^= (unsigned char) out[pos];
  pos++;
  }

  tmp[0] = '\0';
  itoa(cs, tmp, 16);
  strcat(out, tmp);
  strcat(out, "*");
}
