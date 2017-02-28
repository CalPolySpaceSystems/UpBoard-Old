#include <Wire.h>
#include "lsm9ds1.h"
#include "barometer.h"
#include "telemetry.h"

void setup() {
	Serial.begin(9600);
	//Wire.begin();
	//initLSM();

	struct LSMData ldata;
	ldata.temp = -110.90;
	ldata.acc[0] = -4.00;
	ldata.acc[1] = 5.67;
	ldata.acc[2] = 6.23;
	ldata.gyr[0] = 34;
	ldata.gyr[1] = 3.45;
	ldata.gyr[2] = 67.89;
	ldata.mag[0] = 54;
	ldata.mag[1] = 3.75;
	ldata.mag[2] = 53.89;
	struct GPSData gdata;
	gdata.lat = 1234.5678;
	gdata.lng = 9876.5432;
	gdata.time = 1928.3746;
	struct MS5611data mdata;
	mdata.altitude = 5678.9012;
	mdata.pressure = 50.678;
	mdata.temperature = 10.5677;

	char out[255] = "";
	createPacket(out, &ldata, &gdata, &mdata);
	Serial.println(out);
}
void loop() {
	//struct LSMData data;
	//readLSM(&data);
	//SerialUSB.print(lsmToString(&data));
}
