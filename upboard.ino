#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "lsm9ds1.h"
#include "barometer.h"
#include "telemetry.h"

#define BUZZER 3
#define LED 4

uint8_t GPSOutput[48];
int GPSOutputPos = -2;
struct GPSData gdata;
struct MS5611data mdata;

void beep(float hz, float dur){  //beep frequency in Hz for duration in ms (not us)
  int halfT = 500000/hz;
  for (int i = 0; i<(500*dur/halfT) ;i++){
    digitalWrite(BUZZER,HIGH);
    delayMicroseconds(halfT);
    digitalWrite(BUZZER,LOW);
    delayMicroseconds(halfT);
  }
}

void blink(int time) {
  digitalWrite(LED,HIGH);
  delay(time);
  digitalWrite(LED,LOW);
}

void setup() {
 
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  blink(500);
  
	// USB Communication
	SerialUSB.begin(9600);
	// XBee module
	Serial1.begin(9600);
	// GPS
	Serial.begin(115200);

	Wire.begin();
  // see if the card is present and can be initialized:
  if (!SD.begin(8)) {
    SerialUSB.println("SD ERROR");
    Serial1.println("SD ERROR");
    beep(400,2000);
  }
	initLSM();
	initMS5611();

    
  primeTempMS5611();
  delay(10);
  readTempMS5611(&mdata);

  blink(100);
  delay(100);
  blink(100);
  delay(100);
  digitalWrite(LED,HIGH);
  beep(660,400);
  beep(770,400);
}

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
	primePressureMS5611();
	delay(10);
	readPressureMS5611(&mdata);
	calcAltitudeMS5611(&mdata);

	char out[255] = "";
	createPacket(out, &ldata, &gdata, &mdata);
  
	Serial1.print(out);
	SerialUSB.println(out);

  File log = SD.open("uplog.txt", FILE_WRITE);
  if(!log) {
    SerialUSB.println("FILE ERROR");
    Serial1.println("FILE ERROR");
  }

  log.println(out);
  log.close();
}

