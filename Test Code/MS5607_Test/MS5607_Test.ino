#include <Wire.h>

#include "barometer.h"
#include "digital_io.h"

// Functionality switches - Comment out to disable
//#define SD_CARD
//#define USB_TELEMETRY
#define XBEE_TELEMETRY

// Define digial pins
#define BUZZER 9
#define LED LED_BUILTIN
//#define SDSELECT 8
#define AD7606_CS 23
#define AD7606_CONVST 24
#define AD7606_RESET 25

// Give sane names to serial outputs
#define SerialXbee Serial

// Data packet structs
struct BAROMETER_packet barometer;

uint8_t baroCount = 0;
int lastBaroRead = 0;

void setup() {

  // Serial2.begin(19200);
  SerialXbee.begin(19200);

  pinMode(LED,OUTPUT);
  pinMode(12,OUTPUT);
  digitalWrite(LED, HIGH);
  beep(BUZZER, 660, 400);
  beep(BUZZER, 770, 400);

  Wire.begin();

  // Initialize barometer and get initial values
  initMS5607();
  
}

void loop() {
  
  primePressureMS5607();
  delay(50);
  readPressureMS5607(&barometer);
  delay(50);

  primeTempMS5607();
  delay(50);
  readTempMS5607(&barometer);
  delay(50);
  calcAltitudeMS5607(&barometer);

  digitalWrite(12, HIGH);

  String datastring = MS5607ToString(&barometer);
  SerialXbee.println(datastring);
  
  delay(1000);

  digitalWrite(LED, LOW);
  
}

