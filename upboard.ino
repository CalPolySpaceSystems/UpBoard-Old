
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include "lsm9ds1.h"
#include "barometer.h"
#include "telemetry.h"
//#include "control.h"
#include "digital_io.h"

// Define digial pins
#define BUZZER 3
#define LED 4

// Give sane names to serial outputs
#define SerialXbee Serial1
#define SerialGPS Serial

// Control System Constants
#define NUM_READINGS 50
#define KP 0.367063690673387
#define KD 0.344612096976194

// Sensor State
uint8_t GPSOutput[48];
int GPSOutputPos = -2;
struct GPSData gdata;
struct MS5611data mdata;
struct LSMData offset;

// Control System State
struct LSMData readings[NUM_READINGS]; // Used for the moving average function
float prevAngle[3];

Servo pitch1, pitch2, yaw1, yaw2;

File flog;

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
  
  // see if the card is present and can be initialized:
  if (!SD.begin(8)) {
    SerialUSB.println("SD ERROR");
    SerialXbee.println("SD ERROR");
    beep(BUZZER, 400, 2000);
  }
  
  File flog = SD.open("uplog.txt", FILE_WRITE);
  
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

  setupControls();
}

void setupControls() {
  // Set all the values in the moving average to zero
  memset(readings, 0, NUM_READINGS * sizeof(struct LSMData));

  pitch1.attach(6);
  pitch2.attach(7);
  yaw1.attach(10);
  yaw2.attach(12);

  initServo(pitch1);
  initServo(pitch2);
  initServo(yaw1);
  initServo(yaw2);

  // Notify successful initialization of servo positions
  beep(BUZZER, 1000, 1000);
}

void loop() {
  static float pitchArr[10], yawArr[10];
  static int controlCounter = 0;
  static struct LSMData ldata, prevData;
  static float timer0, timer1, timerServo;
  float dt, pitch, yaw;
  static struct state state;

  // Start by priming the barometer because it requires a delay between priming and reading
  primePressureMS5611();

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

  readLSM(&ldata);

  state.launched = state.launched || detectedLaunch(&ldata);
  timer0 = millis(); //time right before data processing
  if (!state.launched)
  {
    // Do nothing
    sensorMovingAverage();
    for (int j = 0; j < 3; j++)
    {
      prevAngle[j] = state.angle[j];
    }
    prevData = ldata;
  }
  else
  {
    applyGyroOffsets(&ldata, &offset);
    dt = (timer0 - timer1) / 1000;
    state.dt = dt;
    for (int j = 0; j < 3; j++)
    {
      state.angle[j] = compFilter(prevAngle[j], ldata.gyr[j], 0.0, dt); // Try passing 0 into the code
    }
    for (int j = 0; j < 3; j++)
    {
      prevAngle[j] = state.angle[j];
    }
    prevData = ldata; //set to keep previous data on next loop

  }


  float pitchRaw = KD * ldata.gyr[0] + KP * state.angle[0];
  float yawRaw = KD * ldata.gyr[1] + KP * state.angle[1];
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
  if (i >= NUM_READINGS)
  {
    i = 0;
  }
  /* Take the average of all the readings */
  for (int j = 0; j < 3; j++)
  {
    offset.acc[j] = total.acc[j] / NUM_READINGS;
    offset.gyr[j] = total.gyr[j] / NUM_READINGS;
  }
  offset.acc[2] -= 1; //We should read 1 G in the z axis
}
