#include <Arduino.h>
#include <math.h>
#include <Wire.h>

#include "priv/imu_priv.h"

byte devAdd[3] = {LSM_AG_ADR,LSM_AG_ADR,LSM_MAG_ADR};

byte startReg[3] = {LSM_ACC_START,LSM_GYRO_START,LSM_MAG_START};

void writeReg(byte deviceAddress, byte targetRegister, byte newValue){
	Wire.beginTransmission(deviceAddress);
	Wire.write(targetRegister);
	Wire.write(newValue);
	Wire.endTransmission();
}

bool checkReg(byte deviceAddress, byte targetRegister, byte expectedValue){

	Wire.beginTransmission(deviceAddress);
	Wire.write(targetRegister);
	Wire.endTransmission(false);

	Wire.requestFrom(deviceAddress,1,true);
	
	byte regValue = Wire.read();

	if ((regValue & expectedValue) > 0) {
		return true;
	}

	else{
		return false;
	}
	
}

void readReg(uint8_t dev, uint8_t startReg, uint8_t len, byte *data) {
  Wire.beginTransmission(dev);
  Wire.write(startReg);
  Wire.endTransmission();

  // request chuck of data and continue reading after start register.
  Wire.requestFrom(dev, len);

  // wait for data
  while(Wire.available() == 0){};

  for(int i = 0; i < len; i++) {
  data[i] = Wire.read();
  }
}

void initIMU(){
  writeReg(A3G_CTRL_REG1,0b00001111,A3G_DEVICE_ADD); // Enable all Gyro axes, Set Bandwidth
  writeReg(A3G_CTRL_REG2,0b00000011,A3G_DEVICE_ADD); // High Pass filter settings
  writeReg(A3G_CTRL_REG5,0b00010000,A3G_DEVICE_ADD); // Enable HP Filter
  
  writeReg(LSM_AG_ADR, LSM_CTRL_REG4, 0b00111000);       //enable gyro
  writeReg(LSM_AG_ADR, LSM_CTRL_REG5_XL,0b00111000);     //enable accelerometer
  writeReg(LSM_AG_ADR, LSM_CTRL_REG6_XL, 0b00001000);    //set accelerometer scale to 16Gs
  writeReg(LSM_AG_ADR, LSM_CTRL_REG1_G, 0b01000000);     //gyro/accel odr and bw
  writeReg(LSM_MAG_ADR, LSM_CTRL_REG2_M, 0b00100000); //set mag sensitivity to 8 gauss
  writeReg(LSM_MAG_ADR, LSM_CTRL_REG3_M, 0b00000000); //enable mag continuous
}

/* LSM9DS1 */
 
void readRawLSM(struct IMU_packet_raw** out){

  int16_t * ind = &(out->accel_x);
  uint8_t read[6];
  
  for (int i = 0; i<9; i+=3){
    
    readReg(startReg [i/3], devAdd [i/3], 6, read);
    
    for (int j = 0; j < 3; i++) {
    out[i+j] = (read[2*i+1]<<8) | (read[2*i]);
    }
    
  }
 
}


/* A3G4250D */

uint8_t readRawA3G(struct IMU_packet_raw** out){
  
  int16_t * ind = &(out->gyro_x);
  int16_t reading;
  uint8_t rc;
  
  for (uint8_t i=0;i<3;i++){
  
    // Grab the data in two reads
    Wire.beginTransmission(A3G_DEVICE_ADD);
    Wire.write(A3G_OUT_START + 2*i + 1);
    Wire.endTransmission(false);
    
    Wire.requestFrom(A3G_DEVICE_ADD,1,true);
    
    reading = (Wire.read()<< 8);

    Wire.beginTransmission(A3G_DEVICE_ADD);
    Wire.write(A3G_OUT_START +  2*i);
    Wire.endTransmission(false);
        
    Wire.requestFrom(A3G_DEVICE_ADD,1,true);
        
    reading |= (Wire.read());
    
    // Add to struct only if is not saturated
    if (abs(reading) < 32600){
      ind[i] = reading;
      rc |= (1<<i);
    }
  }
  
  return rc;
}


uint8_t readRawIMU(struct IMU_packet_raw** out){
	
	readRawLSM(out);
	return readRawA3G(out);

}

void readFloatIMU(struct IMU_packet** out){
	
	struct IMU_packet_raw rawIMU;
	
	int16_t * indRaw = &(rawIMU->accel_x);
	int16_t * indOut = &(out->accel_x);
	
	uint8_t switchGyro = readRawIMU(&rawIMU);
	
	for (i=0;i<3;i++){
		indOut[i] = indRaw[i]*LSM_ACC_FACT; 
	}
	
	for (i=3;i<6;i++){
		if (1<<(i-3)& switchGyro){
			indOut[i] = indRaw[i]*A3G_GYRO_FACT;
		}
		else{
			indOut[i] = indRaw[i]*LSM_GYRO_FACT;
		}
		indOut[i] = indRaw[i]*floatFact[i]; 
	}
	
	for (i=6;i<9;i++){
		indOut[i] = indRaw[i]*LSM_MAG_FACT; 
	}
	
} 
 

