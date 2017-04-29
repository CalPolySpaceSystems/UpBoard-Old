
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include "lsm9ds1.h"
#include "barometer.h"
#include "telemetry.h"
#include "digital_io.h"

// Define digial pins
#define BUZZER 3
#define LED 4
#define SDSELECT 8

#define FILEPREFIX "updata"

// Give sane names to serial outputs
#define SerialXbee Serial1
#define SerialGPS Serial

// Sensor State
uint8_t GPSOutput[48];
int GPSOutputPos = -2;
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
  SerialGPS.begin(115200);
  
  Wire.begin();
  
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
  
  initLSM();
  initMS5611();
  
  // Read temperature requires piriming, delaying, and then reading.
  // Since the temperature does not significantly change throughout flight,
  // reading it at initalization is sufficent for altitude calculation.
  primeTempMS5611();
  delay(10);
  readTempMS5611(&mdata);

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
    char inChar = (char)SerialGPS.read();

    if (inChar == '$') {
      GPSOutputPos = -1;
    } else if (GPSOutputPos > -2) {
      GPSOutputPos++;
      GPSOutput[GPSOutputPos] = inChar;
      // End of message, process and reset to start
      if (GPSOutputPos > 46) {
        processGPS(GPSOutput, &gdata);
        GPSOutputPos = -2;
      }
    }
  }
}

void loop() {
  struct LSMData ldata;
  static uint32_t lastBaroRead;

  // Start by priming the barometer because it requires a delay between priming and reading
  primePressureMS5611();
  readGPS();
  readLSM(&ldata);

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
  
  SerialUSB.println(out);

  File fLog = SD.open(fileName, FILE_WRITE);
  if(fLog) {
    fLog.println(out);
    fLog.close();
  } else {
    SerialUSB.println("NO SD");
    SerialXbee.println("NO SD");
  }
}

