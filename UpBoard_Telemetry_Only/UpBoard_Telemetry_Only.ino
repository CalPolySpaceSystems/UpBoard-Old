/*
 * UpBoard_Telemetry_Only
 * Contributions by:
 * Harrison King, Steven MacLean, Patrick Chizek 
 * Josh Anderson, and Brett Glidden
 * 
 */
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include "lsm9ds1.h"
#include "barometer.h"
#include "telemetry.h"

#define BUZZER 3
#define LED 4
#define SDSELECT 8

uint8_t GPSOutput[48];
int GPSOutputPos = -2;
struct GPSData gdata;
struct MS5611data mdata;

// Variables
const int numReadings = 50; // Memory restricted, can make new readings struct if we don't need mag and temperature data
struct LSMData offset;

// File Naming
char logNum[2];
char fileName[11];
File ourlog;

//Buffer 255 byte arrays
char out[255] = "";

//static struct LSMData readings[numReadings]; // Used for the moving average function
//float prevAngle[3];

void beep(float hz, float dur) { //beep frequency in Hz for duration in ms (not us)
  int halfT = 500000 / hz;
  for (int i = 0; i < (500 * dur / halfT) ; i++) {
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(halfT);
    digitalWrite(BUZZER, LOW);
    delayMicroseconds(halfT);
  }
}


void errorScream(){
 while(1){
    for (int j = 440; j < 660; j++){
      beep(j,8);
    }
  }
}

void blink(int time) {
  digitalWrite(LED, HIGH);
  delay(time);
  digitalWrite(LED, LOW);
}

//File ourlog;
void setup() {

  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  blink(500);
  // USB Communication
  
  SerialUSB.begin(9600);
  //while(!SerialUSB){;}
  // XBee module
  Serial1.begin(9600);
  // GPS
  Serial.begin(115200);
  Wire.begin();
  // see if the card is present and can be initialized:
  
  if (!SD.begin(SDSELECT)) {
    SerialUSB.println("SD ERROR");
    Serial1.println("SD ERROR");
    beep(220, 400);
  }
  
  //File ourlog = SD.open("uplog.txt", FILE_WRITE);

  for (int i = 1; i < 100; i++){ 
    fileName[0] = '\0'; // clear array
    logNum[0] = '\0';
    if (i < 10) {
      strcpy(fileName,"uplog");
      //Serial.println(String(i));
      strcat(fileName,"0");
      itoa(i,logNum,10);
      strcat(fileName,logNum);
      strcat(fileName,".txt");
    } 
    else {
      //Serial.println(String(i));
      if (i > 99) {
       //SerialUSB.println("Card full or file error.");
       errorScream();
      }
      strcpy(fileName,"data");
      //Serial.println(String(i));
      itoa(i,logNum,10);
      strcat(fileName,logNum);
      strcat(fileName,".txt");
      //Serial.println(fileName);
    }
  
    if (!SD.exists(fileName)){// If file does not exist
      //File ourlog = SD.open(fileName, FILE_WRITE);
      //delay(100);
      //ourlog.close();
      break; // This will be the fileName
    } 

  }


  beep(220, 400);
  
  initLSM();
  initMS5611();

  beep(880, 400);
  //primeTempMS5611();
  delay(10);
  primePressureMS5611();
  //readTempMS5611(&mdata);

  digitalWrite(LED, HIGH);
  
  beep(440, 400);
  
  delay(100);
  beep(660, 900);
  
}

void loop() {
  
  static struct LSMData ldata, prevData;

  long lastBaroRead;

  //SerialUSB.println("Loop");

  // Get GPS
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();

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

  //SerialUSB.println("GPS");

  // Get Gyro
  readLSM(&ldata);
  //SerialUSB.println("LSM");
  
  //timer0 = millis(); //time right before data processing
  
  //timer1 = timer0; //set to keep previous time on next loop

  if ((millis() - lastBaroRead) > 10) {
      readPressureMS5611(&mdata); // Take a baro reading
     
      mdata.temperature = ldata.temp; // Update temperature
      
      calcAltitudeMS5611(&mdata);

      primePressureMS5611(); // Prime baro for next reading
      
      lastBaroRead = millis();
    }
    
  createPacket(out, &ldata, &gdata, &mdata);

   /* Temp Debugging*/
  SerialUSB.println(out);
  
  ourlog = SD.open(fileName, FILE_WRITE);
  if(!ourlog) {
        SerialUSB.println("NO SD");
        Serial1.println("FILE ERROR");
        //errorScream();
  } else {
      ourlog.println(out);
      ourlog.close();
  }
  
  // Save data to SD
  //ourlog = SD.open(fileName, FILE_WRITE);
    
  //if(!ourlog) {
    //SerialUSB.println("FILE ERROR");
    //Serial1.println("FILE ERROR");
    //errorScream();
  //}

  //delay(1);
}
