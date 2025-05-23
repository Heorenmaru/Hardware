/*
 * mlx90614.h
 *
 *  Created on: 29 mag 2019
 *      Author: Michele Gazzarri
 */

#ifndef MLX90614_H_
#define MLX90614_H_
#include "main.h"


typedef struct {
	I2C_HandleTypeDef* i2c;
	uint8_t adress;
} MLXHandler;

// C Standard Libraries

typedef enum
{
	MLX90614_OK 	= 0,
	MLX90614_ERROR	= 1
}MLX90614_STATUS;

/* Private defines -----------------------------------------------------------*/
/* DEFAULT SLAVE ADDRESS */
#define MLX90614_DEFAULT_ADDRESS ((0x5A)<<1)
/* OPCODE DEFINES */
#define MLX90614_OP_RAM		0x00
#define MLX90614_OP_EEPROM	0x20
#define MLX90614_OP_SLEEP	0xFF

/* RAM offsets with 16-bit data, MSB first */
#define MLX90614_RAWIR1 0x04
#define MLX90614_RAWIR2 0x05
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07
#define MLX90614_TOBJ2 0x08
/* EEPROM offsets with 16-bit data, MSB first */
#define MLX90614_TOMAX 0x00
#define MLX90614_TOMIN 0x01
#define MLX90614_PWMCTRL 0x02
#define MLX90614_TARANGE 0x03
#define MLX90614_EMISS 0x04
#define MLX90614_CONFIG 0x05
#define MLX90614_ADDR 0x0E
#define MLX90614_EMISS_CALIBRATION 0x0F
#define MLX90614_ID1 0x1C
#define MLX90614_ID2 0x1D
#define MLX90614_ID3 0x1E
#define MLX90614_ID4 0x1F

#define MLX90614CMD_UNLOCK_EMISS_CALIBRATION 0x60
#define MLX90614_SLEEP_MODE 0xFF
/* Exported functions prototypes ---------------------------------------------*/


#endif /* MLX90614_H_ */
