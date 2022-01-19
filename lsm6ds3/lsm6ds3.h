/*
 * lsm6ds3.h
 *
 *  Created on: Jan 13, 2022
 *      Author: Heorenmaru
 */

#ifndef INC_LSM6DS3_H_
#define INC_LSM6DS3_H_
#include "main.h"

//Embedded functions configuration register
#define FUNC_CFG_ACCESS				0x01

//Sensor sync configuration register
#define SENSOR_SYNC_TIME_FRAME		0x04

//FIFO configuration registers
#define FIFO_CTRL1					0x06
#define FIFO_CTRL2					0x07
#define FIFO_CTRL3					0x08
#define FIFO_CTRL4					0x09
#define FIFO_CTRL5					0x0A

#define ORIENT_CFG_G				0x0B

//INTx pin control
#define INT1_CTRL					0x0D
#define INT2_CTRL					0x0E

#define WHO_AM_I					0x0F

//Accelerometer and gyroscope control registers
#define CTRL1_XL					0x10
#define CTRL2_G						0x11
#define CTRL3_C						0x12
#define CTRL4_C						0x13
#define CTRL5_C						0x14
#define CTRL6_C						0x15
#define CTRL7_G						0x16
#define CTRL8_XL					0x17
#define CTRL9_XL					0x18
#define CTRL10_C					0x19

//I2C master configuration register
#define MASTER_CONFIG				0x1A

//Interrupts registers
#define WAKE_UP_SRC					0x1B
#define TAP_SRC						0x1C
#define D6D_SRC						0x1D

//Status data register
#define STATUS_REG					0x1E

//Temperature output data register
#define OUT_TEMP_L					0x20
#define OUT_TEMP_H					0x21

//Gyroscope output register
#define OUTX_L_G					0x22
#define OUTX_H_G					0x23
#define OUTY_L_G					0x24
#define OUTY_H_G					0x25
#define OUTZ_L_G					0x26
#define OUTZ_H_G					0x27

//Accelerometer output register
#define OUTX_L_XL					0x28
#define OUTX_H_XL					0x29
#define OUTY_L_XL					0x2A
#define OUTY_H_XL					0x2B
#define OUTZ_L_XL					0x2C
#define OUTZ_H_XL					0x2D

//Sensor hub output registers
#define SENSORHUB1_REG 		 		0x2E
#define SENSORHUB2_REG 		 		0x2F
#define SENSORHUB3_REG 		 		0x30
#define SENSORHUB4_REG 		 		0x31
#define SENSORHUB5_REG 		 		0x32
#define SENSORHUB6_REG 		 		0x33
#define SENSORHUB7_REG 		 		0x34
#define SENSORHUB8_REG 		 		0x35
#define SENSORHUB9_REG 		 		0x36
#define SENSORHUB10_REG 	 		0x37
#define SENSORHUB11_REG 	 		0x38
#define SENSORHUB12_REG 	 		0x39


//FIFO status registers
#define FIFO_STATUS1 				0x3A
#define FIFO_STATUS2 				0x3B
#define FIFO_STATUS3 				0x3C
#define FIFO_STATUS4 				0x3D

//FIFO data output registers
#define FIFO_DATA_OUT_L 			0x3E
#define FIFO_DATA_OUT_H 			0x3F

//Timestamp output registers
#define TIMESTAMP0_REG 				0x40
#define TIMESTAMP1_REG 				0x41
#define TIMESTAMP2_REG 				0x42

//Step counter timestamp registers
#define STEP_TIMESTAMP_L 			0x49
#define STEP_TIMESTAMP_H 			0x4A

//Step counter output registers
#define STEP_COUNTER_L 				0x4B
#define STEP_COUNTER_H 				0x4C

//Sensor hub output registers
#define SENSORHUB13_REG 			0x4D
#define SENSORHUB14_REG 			0x4E
#define SENSORHUB15_REG 			0x4F
#define SENSORHUB16_REG 			0x50
#define SENSORHUB17_REG 			0x51
#define SENSORHUB18_REG 			0x52

//Interrupt register
#define FUNC_SRC 					0x53
#define TAP_CFG 					0x58
#define TAP_THS_6D 					0x59
#define INT_DUR2 					0x5A
#define WAKE_UP_THS 				0x5B
#define WAKE_UP_DUR 				0x5C
#define FREE_FALL 					0x5D
#define MD1_CFG 					0x5E
#define MD2_CFG 					0x5F

//External magnetometer raw data output registers
#define OUT_MAG_RAW_X_L 			0x66
#define OUT_MAG_RAW_X_H 			0x67
#define OUT_MAG_RAW_Y_L 			0x68
#define OUT_MAG_RAW_Y_H 			0x69
#define OUT_MAG_RAW_Z_L 			0x6A
#define OUT_MAG_RAW_Z_H 			0x6B


/////////// Parameters

//Accelerometer params
//ODR_XL3 ODR_XL2 ODR_XL1 ODR_XL0 FS_XL1 FS_XL0 BW_XL1 BW_XL0
//https://content.arduino.cc/assets/st_imu_lsm6ds3_datasheet.pdf  page 53  //da ebal ia etu arduinu, no datasheet est datasheet
#define ODR_0		0b00000000
#define ODR_12_5	0b00010000
#define ODR_26		0b00100000
#define ODR_52		0b00110000
#define ODR_104		0b01000000
#define ODR_208		0b01010000
#define ODR_416		0b01100000
#define ODR_833		0b01110000
#define ODR_1660	0b10000000
#define ODR_3330	0b10010000
#define ODR_6660	0b10100000

//full-scale selection
#define FS_XL_2		0b00000000
#define FS_XL_16	0b00000100
#define FS_XL_4		0b00001000
#define FS_XL_8		0b00001100

//Anti-aliasing filter bandwidth selection
//(!) Filter not used when accelerometer is in normal and low-power modes
#define BW_XL_400	0b00000000
#define BW_XL_200	0b00000001
#define BW_XL_100	0b00000010
#define BW_XL_50	0b00000011




void lsm_write(uint8_t regaddr, uint8_t regdat);
void lsm_read(uint8_t regaddr, uint8_t *regdat);
uint8_t lsm_read8(uint8_t regaddr);
void lsm_gyro(int16_t *gx, int16_t *gy, int16_t *gz);
void lsm_accel(int16_t *ax, int16_t *ay, int16_t *az);
void lsm_init(SPI_HandleTypeDef *spi_);

#endif /* INC_LSM6DS3_H_ */
