/*
 * crclib.h
 *
 *  Created on: Feb 28, 2022
 *      Author: Heorenmaru
 */

#ifndef INC_CRCLIB_H_
#define INC_CRCLIB_H_

#include "main.h"

uint8_t crc8Maxim(uint8_t *data, uint16_t len);
uint8_t crc8MaximARTS(uint8_t *data, uint16_t len); // deprecated

#endif /* INC_CRCLIB_H_ */
