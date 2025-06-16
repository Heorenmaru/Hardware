/*
 * st7567a.h
 *
 *  Created on: 10 мая 2025 г.
 *      Author: Heorenmaru
 */

#ifndef INC_ST7567A_H_
#define INC_ST7567A_H_


#include "handlers.h"


#define Max_Column	  132
#define Max_Row		  32

#define dispContrast  27

typedef struct {
	SPI_HandleTypeDef* spi;
	Gpio nss;
	Gpio dc;
} st7567Handler;

#endif /* INC_ST7567A_H_ */
