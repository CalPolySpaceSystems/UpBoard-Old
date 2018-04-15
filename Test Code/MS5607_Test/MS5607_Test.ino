#include <Wire.h>
#include "barometer.h"

struct BAROMETER_packet  baroData;
String dataString;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Wire.begin();

  initMS56XX();

}

void loop() {

  Serial.println("Start");
  // Prime Temp
  primeTempMS56XX();
  delay(10);
  Serial.println("Primed Temp");
  
  // Read Temp
  readTempMS56XX(&baroData);
  Serial.println("Read Temp");
  
  //Prime Pressure
  primePressureMS56XX();
  delay(10);
  Serial.println("Primed Pressure");
  
  // Read Pressure
  readPressureMS56XX(&baroData);
  Serial.println("Read Pressure");

  // Calc Altitiude
  calcAltitudeMS56XX(&baroData);
  Serial.println("Calced Alt");

  // Send to serial
  dataString = MS56XXToString(&baroData);
  //Serial.print("S: ");
  Serial.println(dataString);

  // Delay 1 second  
  delay(1000);
}
