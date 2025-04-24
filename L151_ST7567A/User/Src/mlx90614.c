/*
 * mlx90614.c
 *
 *  Created on: 29 mag 2019
 *      Author: Michele Gazzarri
 */

#include "mlx90614.h"

char temp_buff[128] = {};


uint8_t MObjectTemperature;
uint8_t MAmbientTemperature;


MLX90614_STATUS MLX90614_Read16(MLXHandler* handle, uint8_t Register, uint16_t *Result)
{
	uint8_t tmp[2];

	if(HAL_OK != HAL_I2C_Mem_Read(handle->i2c , MLX90614_DEFAULT_ADDRESS, Register, 1, tmp, 2, 10))
	{
		return MLX90614_ERROR;
	}

	*Result = (uint16_t)tmp[1]<<8 | tmp[0];

	return MLX90614_OK;
}

MLX90614_STATUS MLX90614_Read16Eeprom(MLXHandler* handle, uint8_t Address, uint16_t *Result)
{
	uint8_t tmp[2];

	Address &= 0x1F;
	Address |= 0x20;

	if(HAL_OK != HAL_I2C_Mem_Read(handle->i2c , MLX90614_DEFAULT_ADDRESS, Address, 1, tmp, 2, 10))
	{
		return MLX90614_ERROR;
	}

	*Result = (uint16_t)tmp[1]<<8 | tmp[0];

	return MLX90614_OK;
}

uint8_t crc8(uint8_t InCrc, uint8_t InData)
{
	uint8_t i;
	uint8_t Data;
	Data = InCrc ^= InData;
	for ( i = 0; i < 8; i++ )
	{
		if (( Data & 0x80 ) != 0 )
		{
			Data <<= 1;
			Data ^= 0x07;
		}
		else
		{
			Data <<= 1;
		}
	}
	return Data;
}

MLX90614_STATUS MLX90614_Write16Crc(MLXHandler* handle, uint8_t Address, uint16_t Value)
{
	uint8_t Crc;
	uint8_t tmp[3];

	Address &= 0x1F;
	Address |= 0x20;

	Crc = crc8(0, MLX90614_DEFAULT_ADDRESS);
	Crc = crc8(Crc, Address);
	Crc = crc8(Crc, Value & 0xFF);
	Crc = crc8(Crc, Value>>8);

	tmp[0] = Value & 0xFF;
	tmp[1] = Value>>8;
	tmp[2] = Crc;

	if(HAL_OK != HAL_I2C_Mem_Write(handle->i2c, MLX90614_DEFAULT_ADDRESS, Address, 1, tmp, 3, 10))
	{
		return MLX90614_ERROR;
	}

	return MLX90614_OK;
}

MLX90614_STATUS MLX90614_Write16Eeprom(MLXHandler* handle, uint8_t Address, uint16_t Value)
{

	uint16_t Verify;

	if(MLX90614_OK != MLX90614_Write16Crc(handle, Address, 0))
	{
		return MLX90614_ERROR;
	}

	HAL_Delay(10); // Erasing time ~5ms;

	if(MLX90614_OK != MLX90614_Write16Crc(handle, Address, Value))
	{
		return MLX90614_ERROR;
	}

	HAL_Delay(10); // Writing time ~5ms;

	MLX90614_Read16Eeprom(handle, Address, &Verify);
	if(Verify != Value)
	{
		return MLX90614_ERROR;
	}

	return MLX90614_OK;
}

float MLX90614_KelvinToCelsius(uint16_t Temperature)
{
	float Result;

	Result = (float)Temperature * 0.02;
	Result = Result - 273.15;

	return Result;
}

MLX90614_STATUS MLX90614_ReadAmbientTemperature(MLXHandler* handle, float *Temperature)
{
	uint16_t tmp;

	if(MLX90614_OK != MLX90614_Read16(handle, MLX90614_TA, &tmp))
	{
		return MLX90614_ERROR;
	}

	*Temperature = MLX90614_KelvinToCelsius(tmp);

	return MLX90614_OK;
}

MLX90614_STATUS MLX90614_ReadObjectTemperature(MLXHandler* handle, float *Temperature)
{
	uint16_t tmp;

	if(MLX90614_OK != MLX90614_Read16(handle, MLX90614_TOBJ1, &tmp))
	{
		return MLX90614_ERROR;
	}

	*Temperature = MLX90614_KelvinToCelsius(tmp);

	return MLX90614_OK;
}

MLX90614_STATUS MLX90614_GetId(MLXHandler* handle, uint32_t *Id)
{
	uint16_t id[2];

	if(MLX90614_OK != MLX90614_Read16Eeprom(handle, MLX90614_ID1, id))
	{
		return MLX90614_ERROR;
	}

	if(MLX90614_OK != MLX90614_Read16Eeprom(handle, MLX90614_ID3, id+1))
	{
		return MLX90614_ERROR;
	}

	*Id = (uint32_t)id[0]<<16 | id[1];

	return MLX90614_OK;
}

MLX90614_STATUS MLX90614_GetEmissivity(MLXHandler* handle, float *Emissivity)
{
	uint16_t tmp;

	if(MLX90614_OK != MLX90614_Read16Eeprom(handle, MLX90614_EMISS, &tmp))
	{
		return MLX90614_ERROR;
	}

	*Emissivity = (float)tmp/65535.0;

	return MLX90614_OK;
}

MLX90614_STATUS MLX90614_SetEmissivity(MLXHandler* handle, float Emissivity)
{
	if (Emissivity < 0.1 || Emissivity > 1)
		return MLX90614_ERROR;

	uint16_t current_emissivity;
	uint16_t current_calibration;
	uint16_t new_emissivity;
	uint16_t new_calibration;

	MLX90614_Read16Eeprom(handle, MLX90614_EMISS, &current_emissivity);
	MLX90614_Read16Eeprom(handle, MLX90614_EMISS_CALIBRATION, &current_calibration);

	new_emissivity = (uint16_t)round(65535.0 * Emissivity);

	if(new_emissivity == current_emissivity)
	{
		return MLX90614_OK;
	}

	new_calibration = (uint16_t)(((float)current_emissivity / new_emissivity) * current_calibration);

	if(MLX90614_OK != MLX90614_Write16Eeprom(handle, MLX90614_EMISS, new_emissivity))
	{
		return MLX90614_ERROR;
	}

	if(MLX90614_OK != MLX90614_Write16Eeprom(handle, MLX90614_EMISS_CALIBRATION, new_calibration))
	{
		return MLX90614_ERROR;
	}

	return MLX90614_OK;
}

MLX90614_STATUS MLX90614_ResetEmissivity(MLXHandler* handle, float DefaultCalibrationy)
{

	if(MLX90614_OK != MLX90614_Write16Eeprom(handle, MLX90614_EMISS, 0xFFFF))
	{
		return MLX90614_ERROR;
	}

	if(MLX90614_OK != MLX90614_Write16Eeprom(handle, MLX90614_EMISS_CALIBRATION, DefaultCalibrationy))
	{
		return MLX90614_ERROR;
	}

	return MLX90614_OK;
}

MLX90614_STATUS MLX90614_SetAddress(MLXHandler* handle, uint8_t Address)
{
	if(Address > 0x7F) return MLX90614_ERROR;

	if(MLX90614_OK != MLX90614_Write16Eeprom(handle, MLX90614_ADDR, Address))
	{
		return MLX90614_ERROR;
	}

	return MLX90614_OK;
}


