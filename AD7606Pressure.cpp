/* Pressure Board Cpp File
	
*/
#include <arduino.h>
#include <SPI.h>
#include "AD7606Pressure.h"



void initAD7606(int csPin,convstPin,resetPin) {

	#define SENSOR_CS     csPin
	#define SENSOR_CONVST convstPin
	#define SENSOR_RESET  resetPin

	pinMode(SENSOR_CS,OUTPUT);
	pinMode(SENSOR_CONVST,OUTPUT);
	pinMode(SENSOR_RESET, OUTPUT);
	digitalWrite(SENSOR_CS,HIGH);
	digitalWrite(SENSOR_CONVST,HIGH);
	digitalWrite(SENSOR_RESET,HIGH);
	delay(1);
	digitalWrite(SENSOR_RESET,LOW);

}



float* readAD7Pressure(){
	uint16_t rawADC[12];
	float pressureOut[6];

	// Start the cycle of
	digitalWrite(SENSOR_CONVST,LOW);
	delayMicroseconds(2);
	digitalWrite(SENSOR_CONVST,HIGH);
	delayMicroseconds(12);

	SPI.beginTransaction(SPISettings(8000000,MSBFIRST,SPI_MODE3));
	digitalWrite(SENSOR_CS,LOW);  

	for (int i = 0;i<12;i++){
		rawADC[i] = (uint16_t)SPI.transfer(0x00);
	}
	digitalWrite(SENSOR_CS,HIGH);
	SPI.endTransaction();

	// Convert readings to KPa, store in array
	for (int j=0, i = 0;j<12;j += 2, i++){
		pressureOut[i] = (((5.0*((rawADC[j] << 8) | (rawADC[j+1])))/(32768.0))+0.0421)/(0.012105);
	}

	return pressureOut;
}



