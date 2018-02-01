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

/* Write the packet data out to the Xbee */
void write_packet(byte * data, size_t data_size) {
  SerialXbee.write(data, data_size);
  SerialXbee.write(ENDING >> 8);
  SerialXbee.write(ENDING); 
}


void setup() {
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);

  blink(LED, 1, 500);

  Wire.begin();

  SerialXbee.begin(9600);
  SerialXbee.write("MEME");
  // Initialize both IMUs
  initIMU();
  SerialXbee.write("MEME2");
  digitalWrite(LED, HIGH);
  beep(BUZZER, 660, 400);
  beep(BUZZER, 770, 400);

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

struct BAROMETER_packet barometer;
struct IMU_packet imu;

void loop() {
  /* IMU read process */
  readFloatIMU(&imu);
  
  /* Barometer read process */
  primeTempMS5611();
  delay(10);
  readTempMS5611(&barometer);
  primePressureMS5611();
  delay(10);
  readPressureMS5611(&barometer);
  buildPacketMS5611(&barometer);

  /* Send out the data */
  write_packet((byte *)&imu, sizeof(imu));
  write_packet((byte *)&barometer, sizeof(barometer));

  /* Create packet
  SerialXbee.write((byte *)&data, sizeof(data));
  SerialXbee.write(ENDING >> 8);
  SerialXbee.write(ENDING);
  */

    
}

