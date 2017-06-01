#include "A3G4250D.h"
#include <Wire.h>

//Device Address
#define A3G_DEVICE_ADD (0b1101001)

//CTRL Reg Addresses
#define A3G_CTRL_REG1    0x20
#define A3G_CTRL_REG2   0x21
#define A3G_CTRL_REG3   0x22
#define A3G_CTRL_REG4   0x23
#define A3G_CTRL_REG5   0x24
#define FIFO_CTRL_REG   0x2E

// Other registers
#define A3G_OUT_TEMP    0x26
#define A3G_STATUS_REG    0x27
#define A3G_OUT_START   0x28

// Registers to find
#define STAT_ZYXOR      0x80
#define STAT_ZYXDA      0x08

float A3GData[3];
String dataString;

void setup() {
  // put your setup code here, to run once:
  SerialUSB.begin(9600);

  Wire.begin();
  writeReg(A3G_CTRL_REG1,0b00001111,A3G_DEVICE_ADD); //Enable Gyro, Set Bandwidth
  writeReg(A3G_CTRL_REG2,0b00000011,A3G_DEVICE_ADD);


  
}

void loop() {
  // put your main code here, to run repeatedly:
  readA3G(A3GData);

  dataString = (String(A3GData[0]) + ", " + String(A3GData[1]) + ", " + String(A3GData[2]) + ';');
  SerialUSB.println(dataString);
  delay(10);
}
