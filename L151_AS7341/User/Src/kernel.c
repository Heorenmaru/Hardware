/*
 *      Author: Heorenmaru
 */

#include "kernel.h"


#define DEVICE_CODE 0x0000U
#define DEVICE_VER 	0x00U

uint16_t *idBase0 = (uint16_t*)(UID_BASE);
uint16_t *idBase1 = (uint16_t*)(UID_BASE + 0x02);
uint32_t *idBase2 = (uint32_t*)(UID_BASE + 0x04);
uint32_t *idBase3 = (uint32_t*)(UID_BASE + 0x08);
/////////////////////////////////////////////////////////////////////////////////
// HANDLERS and VARS
/////////////////////////////////////////////////////////////////////////////////
//extern ADC_HandleTypeDef hadc;
//extern TIM_HandleTypeDef htim3;
//extern TIM_HandleTypeDef htim2;
//extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
//extern SPI_HandleTypeDef hspi2;







/////////////////////////////////////////////////////////////////////////////////
// USB Receive logic
/////////////////////////////////////////////////////////////////////////////////



void usb_callback(uint8_t *arr, uint16_t len){


	HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 0);
	////////////
	// DEV INFO
	// (STANDART COMMAND)
	if(arr[0] == 0 ){
		uint16_t uid0 = *idBase0;
		uint16_t uid1 = *idBase1;
		uint32_t uid2 = *idBase2;
		uint32_t uid3 = *idBase3;


		usb_rst_cursor();
		usb_add_uint8(0x00);
		usb_add_uint16(uid0);
		usb_add_uint16(uid1);
		usb_add_uint32(uid2);
		usb_add_uint32(uid3);

		usb_add_uint16((uint8_t)DEVICE_CODE);
		usb_add_uint8((uint8_t)DEVICE_VER);

		usb_send_buff();

	}

	HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 1);
}








/////////////////////////////////////////////////////////////////////////////////
// INIT
/////////////////////////////////////////////////////////////////////////////////


void kernel_init() {

    //////////////////////////////////
    // Hardware Check

	HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 0);
	HAL_Delay(2000);
	HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 1);

//
//	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 0);
//
//
//	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 1);
	for(uint8_t i = 0; i<128; i++){
		uint8_t dat = 0;
		if(HAL_I2C_Mem_Read(&hi2c2, (i<<1 | 0b1) , 0xAF, 1, &dat, 1, 1000) == HAL_OK){
			break;
		}
	}

}

uint8_t read_dat(uint8_t reg){
	uint8_t dat = 0;
	HAL_I2C_Mem_Read(&hi2c2, (0x39<<1 | 0b1) , reg, 1, &dat, 1, 1000);
	return dat;
}

void write_dat(uint8_t reg, uint8_t dat){
	uint8_t arr[2] ={reg,dat};
	HAL_I2C_Master_Transmit(&hi2c2,(0x39<<1 | 0b0), arr, 2, 1000);

}
/////////////////////////////////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////////////////////////////////



void kernel_main() {

	usb_set_callback(&usb_callback);
	kernel_init();

	HAL_Delay(2000);

	write_dat(0x80, 0b00000001);
	write_dat(0x70, 0b00001000);
	write_dat(0x70, 0b00000100); //led
	write_dat(0x70, 0b00000100);

	uint8_t astatus = read_dat(0x60);
	uint16_t ch0 = read_dat(0x61)|(read_dat(0x62)<<8);
	uint32_t itime = read_dat(0x63)|(read_dat(0x64)<<8)|(read_dat(0x65)<<8);
	uint16_t ch1 = read_dat(0x66)|(read_dat(0x67)<<8);
	uint16_t ch2 = read_dat(0x68)|(read_dat(0x69)<<8);
	uint16_t ch3 = read_dat(0x6a)|(read_dat(0x6b)<<8);
	uint16_t ch4 = read_dat(0x6c)|(read_dat(0x6d)<<8);
	uint16_t ch5 = read_dat(0x6e)|(read_dat(0x6f)<<8);
	uint8_t FD_TIME_2 = read_dat(0xDA);
	uint8_t stat = read_dat(0x71);
	uint8_t edge = read_dat(0x72);
	uint8_t gpio = read_dat(0x73);
	uint8_t led = read_dat(0x74);
	uint8_t en = read_dat(0x80);
	uint8_t atime = read_dat(0x81);
	uint8_t wtime = read_dat(0x83);

	uint8_t status = read_dat(0x93);


    while(1)
    {
    	write_dat(0x80, 0b00000011);

    	HAL_Delay(100);
    	 ch0 = read_dat(0x95)|(read_dat(0x96)<<8);
    	 ch1 = read_dat(0x97)|(read_dat(0x98)<<8);
    	 ch2 = read_dat(0x99)|(read_dat(0x9a)<<8);
    	 ch3 = read_dat(0x9b)|(read_dat(0x9c)<<8);
    	 ch4 = read_dat(0x9d)|(read_dat(0x9e)<<8);
    	 ch5 = read_dat(0x9f)|(read_dat(0xa0)<<8);

    	HAL_Delay(100);




    }
}



