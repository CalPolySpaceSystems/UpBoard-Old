//#include "telemetry.h"

#define SerialGPS Serial

void setup() {
  // put your setup code here, to run once:
  SerialUSB.begin(9600);
  SerialGPS.begin(9600); 

}

void loop() {
  // put your main code here, to run repeatedly:
  if (SerialGPS.available()){
    SerialUSB.write(SerialGPS.read());
  }
}
