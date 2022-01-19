/*
 * lsm6ds3.c
 *
 *  Created on: Jan 13, 2022
 *      Author: Heorenmaru
 */


#include "lsm6ds3.h"



/**********************************************************************
 *
 *	SPI
 *
 **********************************************************************/

SPI_HandleTypeDef lsm_spi_iface;

void lsm_write(uint8_t regaddr, uint8_t regdat){
	HAL_GPIO_WritePin(LSM_CS_GPIO_Port, LSM_CS_Pin, 0);			//CS = 0
	HAL_SPI_Transmit(&lsm_spi_iface, &regaddr, 1, 100);	//send byte
	HAL_SPI_Transmit(&lsm_spi_iface, &regdat, 1, 100);				 	//send byte
	HAL_GPIO_WritePin(LSM_CS_GPIO_Port, LSM_CS_Pin, 1);			//CS = 1
}

void lsm_read(uint8_t regaddr, uint8_t *regdat){
	uint8_t addr = regaddr | 0b10000000;
	HAL_GPIO_WritePin(LSM_CS_GPIO_Port, LSM_CS_Pin, 0);		//CS = 0
	HAL_SPI_Transmit(&lsm_spi_iface, &addr, 1, 100);				//send byte
	HAL_SPI_Receive(&lsm_spi_iface, regdat, 1, 100);				//read byte
	HAL_GPIO_WritePin(LSM_CS_GPIO_Port, LSM_CS_Pin, 1);		//CS = 1
}

uint8_t lsm_read8(uint8_t regaddr){
	uint8_t addr = regaddr | 0b10000000;
	uint8_t dat;
	HAL_GPIO_WritePin(LSM_CS_GPIO_Port, LSM_CS_Pin, 0);		//CS = 0
	HAL_SPI_Transmit(&lsm_spi_iface, &addr, 1, 100);				//send byte
	HAL_SPI_Receive(&lsm_spi_iface, &dat, 1, 100);				//read byte
	HAL_GPIO_WritePin(LSM_CS_GPIO_Port, LSM_CS_Pin, 1);		//CS = 1
	return dat;
}

void lsm_gyro(int16_t *gx, int16_t *gy, int16_t *gz){
	*gx = lsm_read8(OUTX_L_G) | lsm_read8(OUTX_H_G) << 8;
	*gy = lsm_read8(OUTY_L_G) | lsm_read8(OUTY_H_G) << 8;
	*gz = lsm_read8(OUTZ_L_G) | lsm_read8(OUTZ_H_G) << 8;
}

void lsm_accel(int16_t *ax, int16_t *ay, int16_t *az){
	*ax = lsm_read8(OUTX_L_XL) | lsm_read8(OUTX_H_XL) << 8;
	*ay = lsm_read8(OUTY_L_XL) | lsm_read8(OUTY_H_XL) << 8;
	*az = lsm_read8(OUTZ_L_XL) | lsm_read8(OUTZ_H_XL) << 8;
}

void lsm_init(SPI_HandleTypeDef *spi_){
	//init spi
	lsm_spi_iface = *spi_;

	lsm_write(CTRL1_XL, 0b01010000);
	lsm_write(CTRL2_G, 0b01010000);
}
