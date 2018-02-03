/*
   UpBoardX Legacy Code
*/

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
//#include "lsm9ds1.h"
//#include "A3G4250D.h"
//#include "AD7606Pressure.h"
#include "barometer.h"
//#include "telemetry.h"
#include "imu.h"
#include "digital_io.h"


// Functionality switches - Comment out to disable
//#define SD_CARD
//#define USB_TELEMETRY
#define XBEE_TELEMETRY

#define IMU_ID 3

// Define digial pins
#define BUZZER 12
#define LED LED_BUILTIN
//#define SDSELECT 8
#define AD7606_CS 4
#define AD7606_CONVST 11
#define AD7606_RESET 9

#define ENDING 0x4350

// Give sane names to serial outputs
#define SerialXbee Serial1
#define SerialGPS Serial3

// Data packet structs
struct BAROMETER_packet   barometer;

float imuData[10];

uint8_t baroCount = 0;
uint8_t lastBaroRead;

/* Write the packet data out to the Xbee */
void write_packet(uint8_t id, byte * data, size_t data_size) {
  uint16_t rtc = (uint16_t)(millis()/1000);
  SerialXbee.write(id);
  SerialXbee.write(rtc);
  SerialXbee.write(rtc >> 8);
  SerialXbee.write(data, data_size);
  SerialXbee.write(ENDING >> 8);
  SerialXbee.write(ENDING); 
}

void setup() {
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);

  blink(LED, 1, 500);

  Wire.begin();

  SerialXbee.begin(19200);
  //SerialGPS.begin(9600);

  digitalWrite(LED, HIGH);
  beep(BUZZER, 660, 400);
  beep(BUZZER, 770, 400);

  // Initialize both IMUs
  initIMU();
  /*
  // Initialize barometer and get initial values
  initMS5611();

  primePressureMS5611();
  delay(10);
  readPressureMS5611(&barometer);
  primeTempMS5611();
  lastBaroRead = millis();
  delay(10);
  */
}

/*
  void readGPS() {
  // Read from the GPS if it has data available.
  while (SerialGPS.available()) {
    // get the new byte:
    char inChar = SerialGPS.read();

    if (inChar == '$') {
      GPSOutputPos = 0;
    } else if (GPSOutputPos > -1) {
      // End of message, process and reset to start
      if(inChar == '*') {
        processGPS(GPSOutput, &gdata);
        GPSOutputPos = -1;
        return;
      }

      GPSOutput[GPSOutputPos] = inChar;
      GPSOutputPos++;
    }
  }
  }
*/


void loop() {
  
  /* IMU read process */
  readFloatIMU(imuData);
  
  /* Barometer read process *
  if (millis() - lastBaroRead > 10){
    if (baroCount){
      readPressreMS5611(&barometer);
      if baroCount < 255{
        primePressureMS5611();
      };
      else{
        primeTempMS5611();
      };
    };
    else{
      readTempMS5611(&barometer);
      primePressureMS5611();
    };
    baroCount++;
  };

  buildPacketMS5611(&barometer);
  
  /* GPS Read Process */

  /* Pressure Tap Read Process */
  
  /* Send out the data */
  
  
  write_packet(IMU_ID, (byte *)imuData, sizeof(imuData));
  //write_packet((byte *)&barometer, sizeof(barometer));

    
}

