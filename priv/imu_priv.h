
#ifndef _IMU_PRIV_H_
#define _IMU_PRIV_H_

// Device addresses
#define LSM_AG_ADD  (0b1101010)
#define LSM_MAG_ADD (0b0011100)

// CTRL registers
#define LSM_CTRL_REG4    (0b00011110)
#define LSM_CTRL_REG5_XL (0b00011111)
#define LSM_CTRL_REG6_XL (0b00100000)
#define LSM_CTRL_REG1_G  (0b00010000)
#define LSM_CTRL_REG2_M  (0b00100001)
#define LSM_CTRL_REG1_M  (0b00100000)
#define LSM_CTRL_REG2_M  (0b00100001)
#define LSM_CTRL_REG3_M  (0b00100010)
#define LSM_CTRL_REG4_M  (0b00100011)
#define LSM_WHO_AM_I     (0b00001111)

// Data registers
#define LSM_ACC_START  (0b00101000)
#define LSM_GYRO_START (0b00011000)
#define LSM_MAG_START  (0b00101000)
#define LSM_TS_START   (0b00010101)

// Converion factors
#define LSM_GYRO_FACT (0.00875f)
#define LSM_ACC_FACT (0.000738f)
#define LSM_MAG_FACT (0.00029f)
#define LSM_TS_FACT		(0.001907f)

/* A3G4250D */

//Device Address
#define A3G_DEVICE_ADD (0b1101001)

//CTRL Reg Addresses
#define A3G_CTRL_REG1		0x20 // Toggle axes, data rates
#define A3G_CTRL_REG2		0x21 // HP filter settings
#define A3G_CTRL_REG3		0x22 // Interrupts,DRDY
#define A3G_CTRL_REG4		0x23 // Big/Little Endian,Self_test,SPI Mode
#define A3G_CTRL_REG5		0x24 // HP Filter Enable, FIFO enable, Reboot
#define FIFO_CTRL_REG		0x2E // FIFO Settings

// Status Register
#define A3G_STATUS_REG	0x27

// Output Registers
#define A3G_OUT_START		0x28
#define A3G_OUT_TEMP    0x26

// Registers to find
#define STAT_ZYXOR			0x80
#define STAT_ZYXDA			0x08

// Conversion Factor
# define A3G_GYRO_FACT (0.007477f)

#endif