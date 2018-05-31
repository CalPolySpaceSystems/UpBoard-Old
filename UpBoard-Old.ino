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
#define IMU
#define BAROMETER
#define GPS
//#define PRESSURE_TAP

#define GPS_ID 132
#define IMU_ID 133
#define BARO_ID 134
#define TAP_ID 135

// Define digial pins
#define BUZZER 9
#define LED1 11 
#define LED2 12
#define LED3 13

#define SD_CS 10
#define FLASH_CS 7

#define AD7606_CS 23
#define AD7606_CONVST 24
#define AD7606_RESET 25

#define ENDING 0x4350

// Give sane names to serial outputs
#define SerialXbee Serial
#define SerialGPS Serial1

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

  pinMode(BUZZER,OUTPUT);
  
  digitalWrite(LED3, HIGH);
  
  // C4
  beep(BUZZER, 262, 400);

  SerialXbee.begin(19200);
  SerialGPS.begin(9600);

  // D4
  beep(BUZZER, 294, 400);

  // Initialize the u-blox GPS to GPGLL strings only 
  setupGPSNMEAStrings(&SerialGPS);

  // E4
  beep(BUZZER, 330, 400);
  
  Wire.begin();

  // F4
  beep(BUZZER, 349, 400);

  // Initialize both IMUs
  initIMU();

  // G4
  beep(BUZZER, 392, 400);
  
  // Initialize pressure taps
  initAD7606(AD7606_CS, AD7606_CONVST, AD7606_RESET);
  pinMode(AD7606_RESET,OUTPUT);

  // Set the flash chip CS HIGH
  digitalWrite(FLASH_CS, HIGH);

  // A4
  beep(BUZZER, 440, 400);

  SPI.begin();

  // B4
  beep(BUZZER, 494, 400);

  // Initialize barometer and get initial values
  initMS5607();

  // C5
  beep(BUZZER, 523, 400);
  
  digitalWrite(LED3, LOW);
  
}

void loop() {
  
 	/* Barometer Read Process */
  #ifdef BAROMETER

	primePressureMS5607();
	delay(50);
	readPressureMS5607(&barometer);
	delay(50);

	primeTempMS5607();
	delay(50);
	readTempMS5607(&barometer);
	delay(50);
	calcAltitudeMS5607(&barometer);
  delay(1);
	writePacket(BARO_ID, (byte *)&barometer, sizeof(barometer));
  delay(1);

  #endif

  /* IMU read process */
  #ifdef IMU
  
  readFloatIMU(imuData);
  delay(1);
  writePacket(IMU_ID, (byte *)imuData, sizeof(imuData));
  delay(1);

  #endif

  /* GPS Read Process */
  #ifdef GPS

  if (readGPS(&SerialGPS)) {
		if (minmea_parse_gll(&gpsFrame, (char *) getGPSPacket())) {
			 gps.valid = gpsFrame.status;
       gps.time = millis();
       gps.lat = gpsFrame.latitude.value;
       gps.lng = gpsFrame.longitude.value;
       writePacket(GPS_ID, (byte *)&gps, sizeof(gps));
		 }
  }
  
  #endif

  /* Pressure Tap Read Process */
  #ifdef PRESSURE_TAP 
 
  readAD7Raw(pressTap);
  writePacket(TAP_ID, (byte *)&pressTap, sizeof(pressTap));

  #endif
  
  delay(50);
}

