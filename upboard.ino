#include <Wire.h>
#include "lsm9ds1.h"

void setup() {
  SerialUSB.begin(9600);
	Wire.begin();
	initLSM();
}
void loop() {
	struct LSMData data;
	readLSM(&data);
	SerialUSB.print(lsmToString(&data));
}
