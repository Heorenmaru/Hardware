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



/////////////////////////////////////////////////////////////////////////////////
// USB Receive logic
/////////////////////////////////////////////////////////////////////////////////


//не используется

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




/////////////////////////////////////////////////////////////////////////////////
// INIT
/////////////////////////////////////////////////////////////////////////////////


void kernel_init() {

    //////////////////////////////////
    // Hardware Check
	HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
	HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, 1);
	HAL_Delay(500);
	HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, 1);
	HAL_Delay(500);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 1);


	HAL_Delay(1000);
	HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, 0);
	HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, 0);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 0);

}


// Простая задержка в микросекундах для STM32
void delay_us(uint32_t us) {
    uint32_t count = us * (SystemCoreClock / 10000000); // Грубый расчет под частоту MCU
    while(count--) { __NOP(); }
}

#define ADNS_DELAY 	2

void adns_w (uint8_t addr, uint8_t dat){
	///addr
	HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
	delay_us(ADNS_DELAY);
	HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 1);
	delay_us(ADNS_DELAY);
	HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
	delay_us(ADNS_DELAY);
	for ( uint8_t i= 0; i<=6; i++ ){
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
		delay_us(ADNS_DELAY);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, (addr>>(6-i) & 0b1) );
		delay_us(ADNS_DELAY);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
		delay_us(ADNS_DELAY);
	}
	///data
	for ( uint8_t i= 0; i<=7; i++ ){
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
		delay_us(ADNS_DELAY);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, (dat>>(7-i) & 0b1) );
		delay_us(ADNS_DELAY);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
		delay_us(ADNS_DELAY);
	}

}
uint8_t adns_r (uint8_t addr){
	uint8_t dat = 0;
	HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
	delay_us(ADNS_DELAY);
	HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 0);
	delay_us(ADNS_DELAY);
	HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
	delay_us(ADNS_DELAY);
	for ( uint8_t i= 0; i<=6; i++ ){
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
		delay_us(ADNS_DELAY);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, (addr>>(6-i) & 0b1) );
		delay_us(ADNS_DELAY);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
		delay_us(ADNS_DELAY);
	}
	///data
	HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 1);
	for ( uint8_t i= 0; i<=7; i++ ){
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
		delay_us(ADNS_DELAY);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
		dat |= ( HAL_GPIO_ReadPin(D_GPIO_Port, D_Pin))<<(7-i) ;
		delay_us(ADNS_DELAY);

	}
	return dat;
}

void read_red(){

	HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 0);

	HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, 1);
	HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, 0);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 0);

	uint8_t y = 0;
	uint8_t x = 0;
	adns_w(0x08, 0x00);
	for (uint16_t i = 0; i<324; i++){

		delay_us(1);
		usb_rst_cursor();
		usb_add_uint8(0xFF);
		y = adns_r(0x02);
		x = adns_r(0x03);
		usb_add_uint8(y);
		usb_add_uint8(x);
		usb_add_uint16(i);
		usb_add_uint8(adns_r(0x08));
		if(usb_send_buff()!=0){
			HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 1);
			}
		}
}

void read_green(){
	HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, 0);
	HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, 1);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 0);

	uint8_t y = 0;
	uint8_t x = 0;
	adns_w(0x08, 0x00);
	for (uint16_t i = 0; i<(18*18); i++){
		y = adns_r(0x02);
		x = adns_r(0x03);

		delay_us(1);
		usb_rst_cursor();
		usb_add_uint8(0xFE);
		usb_add_uint8(y);
		usb_add_uint8(x);
		usb_add_uint16(i);
		usb_add_uint8(adns_r(0x08));
		usb_send_buff();
		}
}

void read_blue(){
	HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, 0);
	HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, 0);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 1);

	uint8_t y = 0;
	uint8_t x = 0;
	adns_w(0x08, 0x00);
	for (uint16_t i = 0; i<(18*18); i++){
		y = adns_r(0x02);
		x = adns_r(0x03);

		delay_us(1);
		usb_rst_cursor();
		usb_add_uint8(0xFD);
		usb_add_uint8(y);
		usb_add_uint8(x);
		usb_add_uint16(i);
		usb_add_uint8(adns_r(0x08));
		usb_send_buff();
		}
}


/////////////////////////////////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////////////////////////////////



void kernel_main() {
	usb_set_callback(&usb_callback);
	kernel_init();

	adns_w(0x00, 0b10000000);

	uint8_t id = 0;
	id = adns_r(0x01);
	if(id == 0xFF) {
		while (1){
			HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, 1);
			HAL_Delay(500);
			HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, 0);
			HAL_Delay(500);
		}
	}else{
		HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, 1);
		HAL_Delay(1000);
	}
	adns_w(0x00, 0b00000001);
    while(1)
    {

    	read_red();
    	read_green();
     	read_blue();


    }
}



