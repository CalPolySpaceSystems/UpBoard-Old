/*
 * UpBoardX Legacy Code
 */

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "lsm9ds1.h"
#include "A3G4250D.h"
#include "AD7606Pressure.h"
#include "barometer.h"
#include "telemetry.h"
#include "digital_io.h"


// Functionality switches - Comment out to disable
#define SD_CARD
#define USB_TELEMETRY
#define XBEE_TELEMETRY

// Define digial pins
#define BUZZER 3
#define LED LED_BUILTIN
#define SDSELECT 8
#define AD7606_CS 4
#define AD7606_CONVST 11
#define AD7606_RESET 9


#define FILEPREFIX "updata"

// Give sane names to serial outputs
#define SerialXbee Serial1
#define SerialGPS Serial

// Sensor State
char GPSOutput[64];
int GPSOutputPos = -1;
struct GPSData gdata;
struct MS5611data mdata;
struct LSMData offset;

char fileName[48];

void setup() {
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  
  blink(LED, 1, 500);
  
  // USB Communication
  SerialUSB.begin(9600);
  // XBee module
  SerialXbee.begin(9600);
  // GPS
  SerialGPS.begin(9600);
  setupGPSNMEAStrings(&SerialGPS,  &SerialUSB);
  flushGPS(&SerialGPS);
  
  Wire.begin();

#ifdef SD_CARD
  if(!SD.begin(SDSELECT)) {
    SerialUSB.println("SD ERROR");
    SerialXbee.println("SD ERROR");
    beep(BUZZER, 220, 400);
  }

  for(int i = 1; i < 100; i++){
    fileName[0] = '\0';
    strcat(fileName,FILEPREFIX);

    if(i < 10) {
      strcat(fileName,"0");
    }
    
    char logNum[3] = "";
    itoa(i,logNum,10);
    strcat(fileName,logNum);

    strcat(fileName,".txt");
  
    if (!SD.exists(fileName)){
      break;
    } 
  }
#endif
  
  initLSM();
  initA3G();
  initMS5611();
  
  // Read temperature requires piriming, delaying, and then reading.
  // Since the temperature does not significantly change throughout flight,
  // reading it at initalization is sufficent for altitude calculation.
  primeTempMS5611();
  delay(10);
  readTempMS5611(&mdata);

  initAD7606(AD7606_CS, AD7606_CONVST,AD7606_RESET);

  // Notify successful initialization of sensors
  blink(LED, 3, 100);
  digitalWrite(LED, HIGH);
  beep(BUZZER,660, 400);
  beep(BUZZER, 770, 400);
}

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

void loop() {
  struct LSMData ldata;
  float A3GData[3];
  static uint32_t lastBaroRead;

  // Start by priming the barometer because it requires a delay between priming and reading
  primePressureMS5611();
  readGPS();
  readLSM(&ldata);
  readA3G(A3GData);

  if ((millis() - lastBaroRead) > 10) {
    readPressureMS5611(&mdata); // Take a baro reading 
    mdata.temperature = ldata.temp; // Update temperature
      
    calcAltitudeMS5611(&mdata);
    primePressureMS5611(); // Prime baro for next reading
    // Last 
    lastBaroRead = millis();
  }

  char out[255] = "";
  createPacket(out, &ldata, &gdata, &mdata);

#ifdef USB_TELEMETRY
  SerialUSB.println(out);
#endif


#ifdef XBEE_TELEMETRY
  SerialXbee.println(out);
#endif

#ifdef SD_CARD
  File fLog = SD.open(fileName, FILE_WRITE);
  if(fLog) {
    fLog.println(out);
    fLog.close();
  } else {
    SerialUSB.println("NO SD");
    SerialXbee.println("NO SD");
  }
#endif
}

