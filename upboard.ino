#include <Wire.h>
#include "lsm9ds1.h"
#include "barometer.h"
#include "telemetry.h"

void setup() {
	delay(2000);
	// USB Communication
	SerialUSB.begin(9600);
	// XBee module
	Serial1.begin(9600);
	// GPS
	Serial.begin(115200);
	Wire.begin();
	initLSM();
	initMS5611();
}

uint8_t GPSOutput[48];
int GPSOutputPos = -2;
struct GPSData gdata;

void loop() {
	while (Serial.available()) {
		// get the new byte:
		char inChar = (char)Serial.read();

		if(inChar == '$') {
			GPSOutputPos = -1;
		} else if(GPSOutputPos > -2) {
			GPSOutputPos++;
			GPSOutput[GPSOutputPos] = inChar;
			// End of message, process and reset to start
			if(GPSOutputPos > 46) {
					processGPS(GPSOutput, &gdata);
					GPSOutputPos = -2;
			}
		}
	}

	struct LSMData ldata;
	readLSM(&ldata);
	struct MS5611data mdata;
	primeTempMS5611();
	delay(10);
	readTempMS5611(&mdata);
	primePressureMS5611();
	delay(10);
	readPressureMS5611(&mdata);
	calcAltitudeMS5611(&mdata);

	char out[255] = "";
	createPacket(out, &ldata, &gdata, &mdata);
	Serial1.print(out);
	SerialUSB.println(out);
}
