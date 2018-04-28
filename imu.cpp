#include <Arduino.h>
#include <math.h>
#include <Wire.h>

#include "priv/imu_priv.h"
#include "imu.h"

//byte devAdd[3] = {LSM_AG_ADR,LSM_AG_ADR,LSM_MAG_ADR};
//byte startReg[3] = {LSM_ACC_START,LSM_GYRO_START,LSM_MAG_START};

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
  Wire.endTransmission(false);


  // request chuck of data and continue reading after start register.
  Wire.requestFrom(dev, len,true);

  // wait for data
  //while(Wire.available() == 0){};
  for(int i = 0; i < len; i++) {
    data[i] = Wire.read();
  }
}

void initIMU(){
  writeReg(A3G_DEVICE_ADD,A3G_CTRL_REG1,0b00001111); // Enable all Gyro axes, Set Bandwidth
  writeReg(A3G_DEVICE_ADD,A3G_CTRL_REG2,0b00000011); // High Pass filter settings
  writeReg(A3G_DEVICE_ADD,A3G_CTRL_REG5,0b00000000); // Enable HP Filter

  delay(50);
  
  writeReg(LSM_AG_ADD, LSM_CTRL_REG4, 0b00111000);       //enable gyro
  writeReg(LSM_AG_ADD, LSM_CTRL_REG5_XL,0b00111000);     //enable accelerometer
  writeReg(LSM_AG_ADD, LSM_CTRL_REG6_XL, 0b00001000);    //set accelerometer scale to 16Gs
  writeReg(LSM_AG_ADD, LSM_CTRL_REG1_G, 0b01000000);     //gyro/accel odr and bw

  delay(50);
  writeReg(LSM_MAG_ADD, LSM_CTRL_REG1_M, 0b10110000);
  writeReg(LSM_MAG_ADD, LSM_CTRL_REG2_M, 0b00100000); //set mag sensitivity to 8 gauss
  writeReg(LSM_MAG_ADD, LSM_CTRL_REG3_M, 0b00000000); //enable mag continuous
  writeReg(LSM_MAG_ADD, LSM_CTRL_REG4_M, 0b00000100); 
}

/* LSM9DS1 */
 
void readRawLSM(int16_t out[]){

  int16_t reading;

  /* Read temperature */
  
   Wire.beginTransmission(LSM_AG_ADD);
   Wire.write(LSM_TS_START + 1);
   Wire.endTransmission(false);
    
   Wire.requestFrom(LSM_AG_ADD,1,true);
    
   reading = (Wire.read()<< 8);

   Wire.beginTransmission(LSM_AG_ADD);
   Wire.write(LSM_TS_START);
   Wire.endTransmission(false);
       
   Wire.requestFrom(LSM_AG_ADD,1,true);
       
   reading |= (Wire.read());
    
   out[0] = reading;

  /* Read Accelerometer & Gyro */
  for (uint8_t i=0;i<6;i++){
  
    // Grab the data in two reads
    Wire.beginTransmission(LSM_AG_ADD);
    Wire.write(LSM_ACC_START + 2*i + 1);
    Wire.endTransmission(false);
    
    Wire.requestFrom(LSM_AG_ADD,1,true);
    
    reading = (Wire.read()<< 8);

    Wire.beginTransmission(LSM_AG_ADD);
    Wire.write(LSM_ACC_START +  2*i);
    Wire.endTransmission(false);
        
    Wire.requestFrom(LSM_AG_ADD,1,true);
        
    reading |= (Wire.read());
    
    out[i+1] = reading;
  }

  /* Read Magnetometer */
  for (uint8_t j=0;j<3;j++){
  
    // Grab the data in two reads
    Wire.beginTransmission(LSM_MAG_ADD);
    Wire.write(LSM_MAG_START + 2*j + 1);
    Wire.endTransmission(false);
    
    Wire.requestFrom(LSM_MAG_ADD,1,true);
    
    reading = (Wire.read()<< 8);

    Wire.beginTransmission(LSM_MAG_ADD);
    Wire.write(LSM_MAG_START +  2*j);
    Wire.endTransmission(false);
        
    Wire.requestFrom(LSM_MAG_ADD,1,true);
        
    reading |= (Wire.read());
    
    out[j+7] = reading;
  }
}

/* A3G4250D */

uint8_t readRawA3G(int16_t out[]){

  int16_t reading;
  uint8_t rc = 0;
  
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
    ///Serial1.println(String(reading));
    // Add to struct only if is not saturated
    if (abs(reading) < 32600){
      out[4+i] = reading;
      rc |= (1<<i);
    }
  }
  
  return rc;
}


uint8_t readRawIMU(int16_t out[]){
	
	readRawLSM(out);
	int rc = readRawA3G(out);
  return rc;

}

void readFloatIMU(float out[]){
	
	int16_t rawIMU[10];

  uint8_t switchGyro = readRawIMU(rawIMU);

  // Print arbitrary temperature 
  out[0] = (rawIMU[0]*LSM_TS_FACT)+22.5;

	for (int i=1;i<4;i++){
		out[i] = rawIMU[i]*LSM_ACC_FACT; 
	}
	
	for (int i=4;i<7;i++){
		if (1<<(i-3)& switchGyro){
			out[i] = -rawIMU[i]*A3G_GYRO_FACT;
		}
		else{
			out[i] = rawIMU[i]*LSM_GYRO_FACT;
		}
	}
	
	for (int i=7;i<10;i++){
		out[i] = rawIMU[i]*LSM_MAG_FACT; 
	}
	
} 
 


