/*
 * au_logic.c
 *
 *  Created on: Mar 16, 2022
 *      Author: Heorenmaru
 */

#include "kernel.h"

#define DEVICE_CODE 0x0000U
#define DEVICE_VER 	0x00U

/////////////////////////////////////////////////////////////////////////////////
// HANDLERS and VARS
/////////////////////////////////////////////////////////////////////////////////
//extern ADC_HandleTypeDef hadc;
//extern TIM_HandleTypeDef htim3;
//extern TIM_HandleTypeDef htim2;
extern I2C_HandleTypeDef hi2c1;
//extern I2C_HandleTypeDef hi2c2;


uint8_t devaddr = 0x00;
//TCSHandler tcs1 = {
//		.i2c = &hi2c1,
//		.adress = 0x29,
//
//};

//uint16_t adc_data[9 * 8] = { 0 };

#define T_TIMEOUT  1
#define T_OK 0


//uint32_t cmd_counter = 0;
//uint32_t cmd_counterlast = 0;



//void PWMOFF(){
//	 TIM2->CCR1 = 0;
//	 TIM3->CCR1 = 0;
//	 TIM3->CCR2 = 0;
//	 TIM3->CCR3 = 0;
//	 TIM3->CCR4 = 0;
//	 HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 0);
//}

/////////////////////////////////////////////////////////////////////////////////
// USB Receive logic
/////////////////////////////////////////////////////////////////////////////////



void usb_callback(uint8_t *arr, uint16_t len){


	HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 0);
	////////////
	// DEV INFO
	// (STANDART COMMAND)
	if(arr[0] == 0 ){

		uint16_t *idBase0 = (uint16_t*)(UID_BASE);
		uint16_t *idBase1 = (uint16_t*)(UID_BASE + 0x02);
		uint32_t *idBase2 = (uint32_t*)(UID_BASE + 0x04);
		uint32_t *idBase3 = (uint32_t*)(UID_BASE + 0x08);

		usb_rst_cursor();
		usb_add_uint8(0x00);
		usb_add_uint16(idBase0);
		usb_add_uint16(idBase1);
		usb_add_uint32(idBase2);
		usb_add_uint32(idBase3);

		usb_add_uint16((uint8_t)DEVICE_CODE);
		usb_add_uint8((uint8_t)DEVICE_VER);

		usb_send_buff();

	}



}



//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
//
//    if(hadc->Instance == ADC1)
//    {
//        HAL_ADC_Stop_DMA(hadc); // это необязательно
//        calc_voltage();
//
//    }
//}






/////////////////////////////////////////////////////////////////////////////////
// INIT
/////////////////////////////////////////////////////////////////////////////////


void kernel_init() {

    //////////////////////////////////
    // Hardware Check
	HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 0);
	HAL_Delay(2000);
	HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 1);

}


/////////////////////////////////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////////////////////////////////



void kernel_main() {
	HAL_StatusTypeDef stat ;
	usb_set_callback(&usb_callback);
	kernel_init();
	uint8_t buffer[6];
	uint8_t data;
//	uint8_t addr;
//	addr = 1;
	data = 0b11010001;

//	data = 0xA0;
//	stat = HAL_I2C_Mem_Write(&hi2c1, (0x0d<<1)|0, 0x01, 1, &data, 1, HAL_MAX_DELAY);
//	data = 0x00;
//	stat = HAL_I2C_Mem_Write(&hi2c1, (0x0d<<1)|0, 0x02, 1, &data, 1, HAL_MAX_DELAY);


//	float scale = 2.56;


	for(devaddr = 0; devaddr<127; devaddr++){
		if(HAL_I2C_Mem_Read(&hi2c1,(devaddr<<1)|1, 0x0D, 1, &data, 1, 1000) == HAL_OK){
			HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 0);
			break;
		}
	}
	data = 0b10000000;
	stat = HAL_I2C_Mem_Write(&hi2c1, (devaddr<<1)|0, 0x0A, 1, &data, 1, 1000);
	HAL_Delay(10);
	data = 0b01000000;

	stat = HAL_I2C_Mem_Write(&hi2c1, (devaddr<<1)|0, 0x0A, 1, &data, 1, 1000);

	data = 0b00010001;
	stat = HAL_I2C_Mem_Write(&hi2c1, (devaddr<<1)|0, 0x09, 1, &data, 1, 1000);
	data = 0x01;
	stat = HAL_I2C_Mem_Write(&hi2c1, (devaddr<<1)|0, 0x0b, 1, &data, 1, 1000);
	data = 0;
	stat = HAL_I2C_Mem_Read(&hi2c1, (devaddr<<1)|1, 0x09, 1, &data, 1, 1000);
	 //laser(5);


	 HAL_Delay(2000);
//	 HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 1);
	 int16_t x = 0 ;
	 int16_t y = 0 ;
	 int16_t z = 0 ;
    while(1)
    {


//    	stat = HAL_I2C_Mem_Read(&hi2c1, (devaddr<<1)|1, 0x00, 1, &data, 1, 1000);
//    	x = data;
//    	stat = HAL_I2C_Mem_Read(&hi2c1, (devaddr<<1)|1, 0x01, 1, &data, 1, 1000);
//    	x |= data<<8 ;
////    	y = ;
////    	z = ;
    	HAL_I2C_Mem_Read(&hi2c1, (devaddr<<1)|1, 0x00, 1, (uint8_t *)&buffer, 6, 1000);
    	x = ((buffer[1] << 8) | buffer[0]);
    	y = ((buffer[3] << 8) | buffer[2]) ;
    	z = ((buffer[5] << 8) | buffer[4]) ;



    	usb_rst_cursor();
    	usb_add_uint8(0xFF);
    	usb_add_uint16(x);
    	usb_add_uint16(y);
    	usb_add_uint16(z);
    	usb_send_buff();
    	HAL_Delay(100);
    }
}



