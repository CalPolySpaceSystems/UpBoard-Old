#include <SPI.h>

#define SENSOR_CS     4
#define SENSOR_CONVST 9
#define SENSOR_RESET  11

uint16_t  sensorRaw[12]; 
float pressureOut[6];
int16_t adcRead;
String dataString;

void setup() {
  // put your setup code here, to run once:

  
  
  pinMode(SENSOR_CS,OUTPUT);
  pinMode(SENSOR_CONVST,OUTPUT);
  pinMode(SENSOR_RESET, OUTPUT);
  digitalWrite(SENSOR_CS,HIGH);
  digitalWrite(SENSOR_CONVST,HIGH);
  digitalWrite(SENSOR_RESET,HIGH);
  delay(1);
  digitalWrite(SENSOR_RESET,LOW);

  SPI.begin();
  
  SerialUSB.begin(9600);
  
}


void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(SENSOR_CONVST,LOW);
  delayMicroseconds(2);
  digitalWrite(SENSOR_CONVST,HIGH);
  delayMicroseconds(12);

  SPI.beginTransaction(SPISettings(8000000,MSBFIRST,SPI_MODE3));
  digitalWrite(SENSOR_CS,LOW);  
  for (int i = 0;i<12;i++){
    sensorRaw[i] = (uint16_t)SPI.transfer(0x00);
/*      SerialUSB.print(String(i));
      SerialUSB.print(' ');
      SerialUSB.println(String(sensorRaw[i]));
  */
  }
  digitalWrite(SENSOR_CS,HIGH);
  SPI.endTransaction();

  for (int j=0, i = 0;j<12;j += 2, i++){
    adcRead = (sensorRaw[j] << 8) | (sensorRaw[j+1]);
    pressureOut[i] = (((5.0*(adcRead))/(32768.0))+0.0421)/(0.012105);

  }
  dataString = (String(pressureOut[0])+','+String(pressureOut[1])+','+String(pressureOut[2])+','+String(pressureOut[3])+','+String(pressureOut[4])+','+String(pressureOut[5]));
  SerialUSB.println(dataString);
    
  delay(1); 
}
