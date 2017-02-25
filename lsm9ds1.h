#ifndef _LSM9DS1_H_
#define _LSM9DS1_H_

#include <Arduino.h>


//device addresses


struct LSMData {
	//accelerometer data
	float acc[3];
	//gyroscope data
	float gyr[3];
	//magnetometer data
	float mag[3];
	//temperature data
	float temp;
}

void LSMRead(struct LSMData *out);
void LSMSetup();
