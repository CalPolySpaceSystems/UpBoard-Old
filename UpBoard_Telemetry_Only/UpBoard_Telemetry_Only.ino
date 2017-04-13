/*
 * UpBoard_Telemetry_Only
 * Contributions by:
 * Harrison King, Ste
 * 
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

uint8_t GPSOutput[48];
int GPSOutputPos = -2;
struct GPSData gdata;
struct MS5611data mdata;

// Variables
const int numReadings = 50; // Memory restricted, can make new readings struct if we don't need mag and temperature data
struct LSMData offset;
static struct LSMData readings[numReadings]; // Used for the moving average function
float prevAngle[3];

void beep(float hz, float dur) { //beep frequency in Hz for duration in ms (not us)
  int halfT = 500000 / hz;
  for (int i = 0; i < (500 * dur / halfT) ; i++) {
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(halfT);
    digitalWrite(BUZZER, LOW);
    delayMicroseconds(halfT);
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
  // XBee module
  Serial1.begin(9600);
  // GPS
  Serial.begin(115200);
  Wire.begin();
  // see if the card is present and can be initialized:
  
  if (!SD.begin(8)) {
    SerialUSB.println("SD ERROR");
    Serial1.println("SD ERROR");
    beep(220, 400);
  }
  File ourlog = SD.open("uplog.txt", FILE_WRITE);

  beep(220, 400);
  
  initLSM();
  initMS5611();
  // Set all the values in the moving average to zero
  /*
  for (int i = 0; i < numReadings; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      readings[i].acc[j] = 0;
      readings[i].gyr[j] = 0;
    }
  }
  */
  beep(880, 400);
  primeTempMS5611();
  delay(10);
  readTempMS5611(&mdata);

  //blink(100);
  //delay(50);
  //blink(100);
  //delay(50);
  //digitalWrite(LED, HIGH);
  
  beep(440, 400);
  
  delay(100);
  beep(660, 900);
  
}

void loop() {
  double kp = 0.367063690673387;
  double kd = 0.344612096976194;
  static float pitchArr[10], yawArr[10];
  static int controlCounter = 0;
  static struct LSMData ldata, prevData;
  static bool launched;
  static float timer0, timer1, timerServo;
  float dt, pitch, yaw;
  //float angle[3] = {0, 0, 0};
  struct state state;
  primePressureMS5611();
  //SerialUSB.println("Loop");
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
  readLSM(&ldata);
  //SerialUSB.println("LSM");
  
  timer0 = millis(); //time right before data processing
  
  timer1 = timer0; //set to keep previous time on next loop

  while (timer0 + 10 < millis());
  readPressureMS5611(&mdata);
  calcAltitudeMS5611(&mdata);

  char out[255] = "";
  createPacket(out, &ldata, &gdata, &mdata, &state);

  Serial1.print(out);
  SerialUSB.println(out);

  SerialUSB.println();
  
  /*
  ourlog = SD.open("uplog.txt", FILE_WRITE);
    if(!ourlog) {
    SerialUSB.println("FILE ERROR");
    Serial1.println("FILE ERROR");
    }

    ourlog.println(out);
    //log.close();
  */
  delay(1);
}
