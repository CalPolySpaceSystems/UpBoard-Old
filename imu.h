/*
 * imu.h
 * 
 * Combined support for the A3G4250D and LSM9DS1TR
 *
 * Written by Patrick Chizek
 */

#ifndef _IMU_H_
#define _IMU_H_

/*
union imu_Raw
{
  struct IMU_packet_raw {
    uint16_t rtc;
    uint16_t temp;
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

  int16_t imuRawArr[11];

} imuRaw;
*/
void initIMU(void);

uint8_t readRawIMU(int16_t out[]);
void readFloatIMU(float out[]);

#endif
