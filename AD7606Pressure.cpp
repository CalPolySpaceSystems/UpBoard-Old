
#include <Arduino.h>
#include <SPI.h>
#include "AD7606Pressure.h"

int ad7_CS;
int ad7_CVS;
int ad7_RESET;

void initAD7606(int csPin, int convstPin, int resetPin) {

	ad7_CS = csPin;
	ad7_CVS = convstPin;
	ad7_RESET = resetPin;

	pinMode(ad7_CS,OUTPUT);
	pinMode(ad7_CVS,OUTPUT);
	pinMode(ad7_RESET, OUTPUT);
	digitalWrite(ad7_CS,HIGH);
	digitalWrite(ad7_CVS,HIGH);
	digitalWrite(ad7_RESET,HIGH);
	delay(1);
	digitalWrite(ad7_RESET,LOW);

}


void readAD7Raw(uint16_t *rawADC){
	
	// Start the cycle of
	digitalWrite(ad7_CVS,LOW);
	delayMicroseconds(2);
	digitalWrite(ad7_CVS,HIGH);
	delayMicroseconds(12);

	SPI.beginTransaction(SPISettings(8000000,MSBFIRST,SPI_MODE3));
	digitalWrite(ad7_CS,LOW);
 
	for (int i = 0;i<12;i++){
		rawADC[i] = (uint16_t)SPI.transfer(0x00);
	}
 
	digitalWrite(ad7_CS,HIGH);
	SPI.endTransaction();

	
}


void readAD7Pressure(float *pressureOut){

	uint16_t rawADC[12];
	readAD7Raw(rawADC);
	
	// Convert readings to KPa, store in array
	for (int j=0, i = 0;j<12;j += 2, i++){
		pressureOut[i] = (((5.0*((rawADC[j] << 8) | (rawADC[j+1])))/(32768.0))+0.0421)/(0.012105);
	}
}

