/*
 * imu.h
 * 
 * Combined support for the A3G4250D and LSM9DS1TR
 *
 * Written by Patrick Chizek
 */

#ifndef _IMU_H_
#define _IMU_H_

struct IMU_packet {
  uint8_t id;
  uint16_t rtc;
  float accel_x;
  float accel_y;
  float accel_z;
  float gyro_x;
  float gyro_y;
  float gyro_z;
  float mag_x;
  float mag_y;
  float mag_z;
}__attribute__((packed));

struct IMU_packet_raw {
  uint8_t id;
  uint16_t rtc;
  int16_t  accel_x;
  int16_t  accel_y;
  int16_t  accel_z;
  int16_t  gyro_x;
  int16_t  gyro_y;
  int16_t  gyro_z;
  int16_t  mag_x;
  int16_t  mag_y;
  int16_t  mag_z;
}__attribute__((packed));

void initIMU(void);

uint8_t readRawIMU(struct IMU_packet_raw* out);
void readFloatIMU(struct IMU_packet* out);

#endif
