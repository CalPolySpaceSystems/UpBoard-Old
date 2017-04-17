
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include "lsm9ds1.h"
#include "barometer.h"
#include "telemetry.h"
#include "control.h"
#include "digital_io.h"

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

Servo pitch1, pitch2, yaw1, yaw2;

void InitServo(Servo myServo)
{
  myServo.writeMicroseconds(1500);
  delay(500);
  myServo.writeMicroseconds(1500-45*10);
  delay(500);
  myServo.writeMicroseconds(1500+45*10);
  delay(1000);
  myServo.writeMicroseconds(1500);
  delay(500);
}

File ourlog;
void setup() {

  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  
  blink(LED, 1, 500);
  
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
    beep(BUZZER, 400, 2000);
  }
  
  File ourlog = SD.open("uplog.txt", FILE_WRITE);
  
  initLSM();
  initMS5611();
  
  // Set all the values in the moving average to zero
  for (int i = 0; i < numReadings; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      readings[i].acc[j] = 0;
      readings[i].gyr[j] = 0;
    }
  }
  
  primeTempMS5611();
  delay(10);
  
  readTempMS5611(&mdata);
  
  pitch1.attach(6);
  pitch2.attach(7);
  
  yaw1.attach(10);
  yaw2.attach(12);
  
  blink(LED, 3, 100);
  digitalWrite(LED, HIGH);
  beep(BUZZER,660, 400);
  beep(BUZZER, 770, 400);

  InitServo(pitch1);
  InitServo(pitch2);
  InitServo(yaw1);
  InitServo(yaw2);

  beep(BUZZER, 1000, 1000);

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
  launched = launchDetect(&ldata, launched);
  //if (millis() > 20000)
  //{
    //launched = true;
  //}
  state.launched = launched;
  timer0 = millis(); //time right before data processing
  if (!launched)
  {
    //SerialUSB.println("Not Launched");
    // Do nothing
    sensorMovingAverage();
    for (int j = 0; j < 3; j++)
    {
      prevAngle[j] = state.angle[j];
    }//memcpy(prevAngle, state.angle, 12);
    prevData = ldata;
  }
  else
  {
    ApplyOffsets(&ldata);
    dt = (timer0 - timer1) / 1000;
    //SerialUSB.println(dt);
    state.dt = dt;
    for (int j = 0; j < 3; j++)
    {
      state.angle[j] = compFilter(prevAngle[j], ldata.gyr[j], 0.0, dt); // Try passing 0 into the code
    }
    for (int j = 0; j < 3; j++)
    {
      prevAngle[j] = state.angle[j];
    }//memcpy(prevAngle, state.angle, 12);
    prevData = ldata; //set to keep previous data on next loop

  }


  float pitchRaw = kd * ldata.gyr[0] + kp * state.angle[0];
  float yawRaw = kd * ldata.gyr[1] + kp * state.angle[1];
  if (pitchRaw > 15) {
    pitchRaw = 15;
  }
  if (pitchRaw < -15) {
    pitchRaw = -15;
  }
  if (yawRaw > 15) {
    yawRaw = 15;
  }
  if (yawRaw < -15) {
    yawRaw = -15;
  }

  pitchArr[controlCounter] = pitchRaw;
  yawArr[controlCounter] = yawRaw;
  controlCounter++;
  if (controlCounter >= 10)
  {
    controlCounter = 0;
  }
if (timer1 - timerServo > 100)
{
  //SerialUSB.println("Move");
  for (int ii = 0; ii < 10; ii++)
  {
    pitch =+ pitchArr[ii];
    yaw =+ yawArr[ii];
  }
  float pitchMicro1 = 1500 + 60*.707*pitch + 30*.707*yaw;
  float pitchMicro2 = 1500 - 30*.707*pitch + 30*.707*yaw;
  float yawMicro1 = 1500 + 60*.707*yaw + 30*.707*pitch;
  float yawMicro2 = 1500 - 30*.707*yaw + 30*.707*pitch;
//  pitch1.writeMicroseconds(pitchMicro1);
//  pitch2.writeMicroseconds(pitchMicro2);
//  yaw1.writeMicroseconds(yawMicro1);
//  yaw2.writeMicroseconds(yawMicro2);
  //  pitch1.write(pitch);
  //  pitch2.write(-pitch);
  //  yaw1.write(yaw);
  //  yaw2.write(-yaw);
  //SerialUSB.println(timer0-timer1, 4);
  timerServo = millis();
}
  timer1 = timer0; //set to keep previous time on next loop

  while (timer0 + 10 < millis());
  readPressureMS5611(&mdata);
  calcAltitudeMS5611(&mdata);

  char out[255] = "";
  createPacket(out, &ldata, &gdata, &mdata, &state);

  Serial1.print(out);
  SerialUSB.println(out);
  SerialUSB.print(pitch);
  SerialUSB.print("\t");
  SerialUSB.print(yaw);
  SerialUSB.print("\t");
  SerialUSB.print(ldata.acc[0]);
  SerialUSB.print("\t");
  SerialUSB.print(ldata.acc[1]);
  SerialUSB.print("\t");
  SerialUSB.print(ldata.acc[2]);
  SerialUSB.print("\t");
  SerialUSB.print(ldata.gyr[0]);
  SerialUSB.print("\t");
  SerialUSB.print(ldata.gyr[1]);
  SerialUSB.print("\t");
  SerialUSB.print(ldata.gyr[2]);
  SerialUSB.print("\t");
  SerialUSB.print(state.angle[0]);
  SerialUSB.print("\t");
  SerialUSB.print(state.angle[1]);
  SerialUSB.print("\t");
  SerialUSB.print(state.angle[2]);
  SerialUSB.print("\t");
  SerialUSB.println(state.dt, 4);
  /*ourlog = SD.open("uplog.txt", FILE_WRITE);
    if(!ourlog) {
    SerialUSB.println("FILE ERROR");
    Serial1.println("FILE ERROR");
    }

    ourlog.println(out);
    //log.close();
  */
  delay(1);
}
float compFilter(float oldAngle, float gyroData, float accAng, float dt)
{
  return 0.99 * (oldAngle + gyroData * dt) + 0.01 * accAng;
}

void ApplyOffsets(struct LSMData *ldata)
{
  ldata->acc[0] = ldata->acc[0] - offset.acc[0];
  ldata->acc[1] = ldata->acc[1] - offset.acc[1];
  ldata->acc[2] = ldata->acc[2] - offset.acc[2];
  ldata->gyr[0] = ldata->gyr[0] - offset.gyr[0];
  ldata->gyr[1] = ldata->gyr[1] - offset.gyr[1];
  ldata->gyr[2] = ldata->gyr[2] - offset.gyr[2];
}

bool launchDetect(struct LSMData *data, bool launched)
{
  if (data->acc[2] > 1.5 || launched) //TODO: Determine what is a reasonable value for this
  {
    //SerialUSB.println("Launch Detected");
    return true;
  }
  else
  {
    return false;
  }

}

void sensorMovingAverage()
{
  static struct LSMData total;
  static int i;
  /* Subtract the last reading of the current index*/
  for (int j = 0; j < 3; j++)
  {
    total.acc[j] -= readings[i].acc[j];
    total.gyr[j] -= readings[i].gyr[j];
  }
  /* Get new reading at current index */
  readLSM(&readings[i]);
  for (int j = 0; j < 3; j++)
  {
    total.acc[j] += readings[i].acc[j];
    total.gyr[j] += readings[i].gyr[j];
  }
  i++;
  /* Reset array count if array is full */
  if (i >= numReadings)
  {
    i = 0;
  }
  /* Take the average of all the readings */
  for (int j = 0; j < 3; j++)
  {
    offset.acc[j] = total.acc[j] / numReadings;
    offset.gyr[j] = total.gyr[j] / numReadings;
  }
  offset.acc[2] -= 1; //We should read 1 G in the z axis
}
