/*
   UpBoardX Legacy Code
*/

#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include "AD7606Pressure.h"
#include "barometer.h"
#include "imu.h"
#include "digital_io.h"
#include "gps.h"

// GPS NMEA Decoder note gpsPort is Serial1
#include "minmea.h"

// Functionality switches - Comment out to disable
//#define SD_CARD
//#define USB_TELEMETRY
#define XBEE_TELEMETRY

#define GPS_ID 2
#define IMU_ID 3
#define BARO_ID 4
#define TAP_ID 5

// Define digial pins
#define BUZZER 9
#define LED LED_BUILTIN
//#define SDSELECT 8
#define AD7606_CS 23
#define AD7606_CONVST 24
#define AD7606_RESET 25

#define ENDING 0x4350

// Give sane names to serial outputs
#define SerialXbee Serial
#define SerialGPS Serial1

// Flash Chip 
#define FLASH_CS 7

// Data packet structs
struct BAROMETER_packet barometer;
struct GPS_packet gps;
float imuData[10];
uint16_t pressTap[6];

uint8_t baroCount = 0;
int lastBaroRead = 0;

// GPS standards
struct minmea_sentence_gll gpsFrame;

/* Write the packet data out to the Xbee */
void writePacket(uint8_t id, byte * data, size_t data_size) {
  uint16_t rtc = (uint16_t)(millis());
  SerialXbee.write(id);
  SerialXbee.write(rtc);
  SerialXbee.write(rtc >> 8);
  SerialXbee.write(data, data_size);
  SerialXbee.write(ENDING >> 8);
  SerialXbee.write(ENDING); 
}

void setup() {

  SerialXbee.begin(19200);
  SerialGPS.begin(9600);

  // Initialize the u-blox GPS to GPGLL strings only 
  setupGPSNMEAStrings(&SerialGPS);

  digitalWrite(LED, HIGH);
  //beep(BUZZER, 660, 400);
  //beep(BUZZER, 770, 400);
  
  Wire.begin();

  // Initialize both IMUs
  initIMU();

  // Initialize pressure taps
  //initAD7606(AD7606_CS, AD7606_CONVST, AD7606_RESET);
  //pinMode(AD7606_RESET,OUTPUT);

  // Set the flash chip CS HIGH
  digitalWrite(FLASH_CS, HIGH);

  SPI.begin();

  // Initialize barometer and get initial values
  initMS5607();
}

void loop() {
 	/* Barometer Read Process */
	primePressureMS5607();
	delay(50);
	readPressureMS5607(&barometer);
	delay(50);

	primeTempMS5607();
	delay(50);
	readTempMS5607(&barometer);
	delay(50);
	calcAltitudeMS5607(&barometer);
	writePacket(BARO_ID, (byte *)&barometer, sizeof(barometer));

  /* IMU read process */
  readFloatIMU(imuData);
  writePacket(IMU_ID, (byte *)imuData, sizeof(imuData));

  
  /* GPS Read Process */
  if (readGPS(&SerialGPS)) {
		if (minmea_parse_gll(&gpsFrame, (char *) getGPSPacket())) {
			 gps.valid = gpsFrame.status;
       gps.time = millis();
       gps.lat = gpsFrame.latitude.value;
       gps.lng = gpsFrame.longitude.value;
       writePacket(GPS_ID, (byte *)&gps, sizeof(gps));
		 }
  }

}

