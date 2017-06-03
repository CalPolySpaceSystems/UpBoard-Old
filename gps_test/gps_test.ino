#include <SPI.h>
#include "gps.h"

// Define digial pins
#define BUZZER 3
#define LED 4

// Give sane names to serial outputs
#define SerialXbee Serial1
#define SerialGPS Serial

void setup() {
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  
  blink(LED, 1, 500);
  
  // USB Communication
  SerialUSB.begin(9600);
  // GPS
  SerialGPS.begin(9600);

  // Notify successful initialization of sensors
  blink(LED, 3, 100);
  digitalWrite(LED, HIGH);
  beep(BUZZER,660, 400);
  beep(BUZZER, 770, 400);
  
  setupGPSNMEAStrings(SerialGPS, SerialUSB);
  flushGPS(SerialGPS);
}

void loop() {
  if (SerialGPS.available()) {
    SeraiUSB.print(SerialGPS.read());
  }
}

