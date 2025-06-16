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
extern SPI_HandleTypeDef hspi2;
//uint16_t adc_data[9 * 8] = { 0 };

#define T_TIMEOUT  1
#define T_OK 0


uint32_t cmd_counter = 0;
uint32_t cmd_counterlast = 0;

st7567Handler disp = {
				.spi = &hspi2,
				.nss.port = DCS_GPIO_Port,
				.nss.pin = DCS_Pin,
				.dc.port = DDC_GPIO_Port,
				.dc.pin = DDC_Pin
};
/////////////////////////////////////////////////////////////////////////////////
// USB Receive logic
/////////////////////////////////////////////////////////////////////////////////



void usb_callback(uint8_t *arr, uint16_t len){

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





uint16_t read_temp(){
	uint16_t t = 0;
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, 1);
	HAL_Delay(220);
	HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, 0);

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, 0);

	for(uint8_t i = 0; i<16; i++){
		HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, 0);
		HAL_Delay(1);
		HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, 1);
		HAL_Delay(1);
		t = t<<1 | HAL_GPIO_ReadPin(DAT_GPIO_Port, DAT_Pin);
	}
	HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, 0);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, 1);

	if((t & 0b100) > 0){
		return 0xFFFF;
	}
	return (t>>3);

}


/////////////////////////////////////////////////////////////////////////////////
// INIT
/////////////////////////////////////////////////////////////////////////////////


void kernel_init() {

    //////////////////////////////////
    // Hardware Check
	D_Init(&disp);
	HAL_Delay(2000);
	HAL_GPIO_WritePin(L13_GPIO_Port, L13_Pin, 0);
	Dcls(&disp);
}


/////////////////////////////////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////////////////////////////////
uint8_t buf[32] = {0};


void kernel_main() {
	usb_set_callback(&usb_callback);
	kernel_init();

	uint16_t tmp = 0;
    while(1)
    {
    	tmp = read_temp();
    	sprintf(buf, "temp: %.3f   ", (float)tmp/4);
    	ShowString(&disp, 10,1,(uint8_t*)buf);
//    	HAL_Delay(200);
    }
}



