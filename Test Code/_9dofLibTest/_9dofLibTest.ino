#include <Wire.h>
#include "lsm9ds1.h"

#define LSM_AG_ADD        (0B1101011)
#define LSM_MAG_ADD       (0B0011110)

#define LSM_CTRL_REG4     (0b00011110)
#define LSM_CTRL_REG5_XL  (0b00011111)
#define LSM_CTRL_REG6_XL  (0b00100000)
#define LSM_CTRL_REG1_G   (0b00010000)
#define LSM_CTRL_REG2_M   (0b00100001)
#define LSM_CTRL_REG3_M   (0b00100010)
#define LSM_WHO_AM_I      (0b00001111)

#define LSM_ACC_START     (0b00101000)
#define LSM_GYRO_START    (0b00011000)
#define LSM_MAG_START     (0b00101000)
#define LSM_TS_START      (0b00010101)

#define LSM_GYRO_FACT     (0.00875f)
#define LSM_ACC_FACT      (0.000732f)
#define LSM_MAG_FACT      (0.00029f)

int16_t lsmData[10];

void setup() {
  // put your setup code here, to run once:
  SerialUSB.begin(9600);
  Wire.begin();
  initLSM();
}

void loop() {
  // put your main code here, to run repeatedly:
  struct LSMData ldata;

  readLSM(&ldata);
  

  SerialUSB.println(dataString);
    
}

void readLSMRaw(byte *lsmRaw){

  /*
  if sizeof(){
    return;
  }
  */
  
  //Get Gyro/Accelerometer Readings
  for (byte i=0; i<2, i++){

    Wire.beginTransmission(LSM_ADR);
    Wire.write(LSM_ACC_START + 6*i);
    Wire.endTransmission();
  
    Wire.requestFrom(LSM_ADR, 6);

    while(Wire.available() == 0){};

    for(int j=0;j<6;j++) {
      lsmRaw[6*i + j] = Wire.read();
    }
  
  }

  // Get Magnetometer Readings
  Wire.beginTransmission(LSM_ADR);
  Wire.write(LSM_MAG_START);
  Wire.endTransmission();

  Wire.requestFrom(LSM_M_ADR, 6);

  while(Wire.available() == 0){};

  for(int j=0;j<6;j++) {
    lsmRaw[j + 12] = Wire.read();
  }  

  // Get Temperature Readings
  Wire.beginTransmission(LSM_ADR);
  Wire.write(LSM_TS_START);
  Wire.endTransmission();

  Wire.requestFrom(LSM_ADR, 2);

  while(Wire.available() == 0){};

  for(int j=0;j<2;j++) {
    lsmRaw[j + 18] = Wire.read(); 
  }  

}


