/*	
	CPSS 3DOF Library
	Written by Patrick Chizek
	5/2017
*/
#include <Arduino.h>
#include <math.h>

//CTRL Reg Addresses
#define A3G_CTRL_REG1		0x20
#define A3G_CTRL_REG2		0x21
#define A3G_CTRL_REG3		0x22
#define A3G_CTRL_REG4		0x23
#define A3G_CTRL_REG5		0x24
#define FIFO_CTRL_REG		0x2E

// Other registers
#define A3G_OUT_TEMP		0x26
#define A3G_STATUS_REG		0x27
#define A3G_OUT_START		0x28

// Registers to find
#define STAT_ZYXOR			0x80
#define STAT_ZYXDA			0x08

void readRawA3G(int16_t *rawA3G);{
	
	for (byte i=0;i<3;i++){

		Wire.beginTransmission(A3G_DEVICE_ADD);
		Wire.write(A3G_OUT_START + i);
		Wire.endTransmission(false);
		
		Wire.requestFrom(A3G_DEVICE_ADD,1,true);
		while (Wire.available() == 0);
		
		rawA3G[i] = (Wire.read()<< 8);

		Wire.beginTransmission(A3G_DEVICE_ADD);
		Wire.write(A3G_OUT_START + 1 + i);
		Wire.endTransmission(false);
				
		Wire.requestFrom(A3G_DEVICE_ADD,1,true);
		while (Wire.available() == 0);
				
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
		outA3G[i] = round(raw[i] * 0.7477);
	}

}

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
	while(Wire.available() == 0);
	
	byte regValue = Wire.read();

	if ((regValue & expectedValue) > 0) {
		return true;
	}

	else{
		return false;
	}
	
}
