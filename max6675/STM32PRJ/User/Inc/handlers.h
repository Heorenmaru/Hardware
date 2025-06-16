/*
 * handlers.h
 *
 *  Created on: 15 июн. 2023 г.
 *      Author: kuznetsovdp
 */

#ifndef INC_HANDLERS_H_
#define INC_HANDLERS_H_
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "main.h"


typedef enum {
    eStatusError = 0,
    eStatusOk = 1,
} Status;

typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin;
} Gpio;






#endif /* INC_HANDLERS_H_ */
