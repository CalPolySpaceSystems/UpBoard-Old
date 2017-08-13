// I2C Functions for STM A3G4250D
// Patrick Chizek
// Written for CPSS, 6/2017

#include <Arduino.h>
#include <math.h>
#include <Wire.h>

//Device Address
#define A3G_DEVICE_ADD (0b1101001)

//CTRL Reg Addresses
#define A3G_CTRL_REG1		0x20 // Toggle axes, data rates
#define A3G_CTRL_REG2		0x21 // HP filter settings
#define A3G_CTRL_REG3		0x22 // Interrupts,DRDY
#define A3G_CTRL_REG4		0x23 // Big/Little Endian,Self_test,SPI Mode
#define A3G_CTRL_REG5		0x24 // HP Filter Enable, FIFO enable, Reboot
#define FIFO_CTRL_REG		0x2E // FIFO Settings

// Status Register
#define A3G_STATUS_REG	0x27

// Output Registers
#define A3G_OUT_START		0x28
#define A3G_OUT_TEMP    0x26

// Registers to find
#define STAT_ZYXOR			0x80
#define STAT_ZYXDA			0x08

void writeReg(byte deviceAddress, byte targetRegister, byte newValue){
  Wire.beginTransmission(deviceAddress);
  Wire.write(targetRegister);
  Wire.write(newValue);
  Wire.endTransmission();
}

void initA3G(){
  writeReg(A3G_CTRL_REG1,0b00001111,A3G_DEVICE_ADD); // Enable all Gyro axes, Set Bandwidth
  writeReg(A3G_CTRL_REG2,0b00000011,A3G_DEVICE_ADD); // High Pass filter settings
  writeReg(A3G_CTRL_REG5,0b00010000,A3G_DEVICE_ADD); // Enable HP Filter
}

void readRawA3G(int16_t *rawA3G){
	
	for (byte i=0;i<3;i++){

		Wire.beginTransmission(A3G_DEVICE_ADD);
		Wire.write(A3G_OUT_START + 2*i + 1);
		Wire.endTransmission(false);
		
		Wire.requestFrom(A3G_DEVICE_ADD,1,true);
		
		rawA3G[i] = (Wire.read()<< 8);

		Wire.beginTransmission(A3G_DEVICE_ADD);
		Wire.write(A3G_OUT_START +  2*i);
		Wire.endTransmission(false);
				
		Wire.requestFrom(A3G_DEVICE_ADD,1,true);
				
		rawA3G[i] |= (Wire.read());
	}
	
}

void readA3G(float *outA3G){

	int16_t raw[3];
	readRawA3G(raw);

	for (int i=0;i<3;i++){
		outA3G[i] = (raw[i] * 0.007477);
	}
	
}

void readFixIntA3G(int16_t *fpA3G){

	int16_t raw[3];
	readRawA3G(raw);

	for (int i=0;i<3;i++){
		fpA3G[i] = round(raw[i] * 0.7477);
	}

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
