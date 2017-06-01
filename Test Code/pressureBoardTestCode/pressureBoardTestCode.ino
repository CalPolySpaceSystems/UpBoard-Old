#include <SPI.h>
#include "AD7606Pressure.h"

uint16_t  sensorRaw[12]; 
float pressure[6];
int16_t adcRead;
String dataString;

void setup() {
  // put your setup code here, to run once:

  
  initAD7606(4,11,9);
  SPI.begin();
  
  SerialUSB.begin(9600);
  
}


void loop() {
  // put your main code here, to run repeatedly:
  
  readAD7Pressure(pressure);
  dataString = (String(pressure[0])+','+String(pressure[1])+','+String(pressure[2])+','+String(pressure[3])+','+String(pressure[4])+','+String(pressure[5]));
  SerialUSB.println(dataString);
    
  delay(1); 
}
