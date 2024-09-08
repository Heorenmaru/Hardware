/*
 *   Author: Heorenmaru
 *   License: CC-BY-SA
 */

#include "kernel.h"

#define DEVICE_CODE 0x0000U
#define DEVICE_VER 	0x00U

/////////////////////////////////////////////////////////////////////////////////
// HANDLERS and VARS
/////////////////////////////////////////////////////////////////////////////////


#define T_TIMEOUT  1
#define T_OK 0


extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;


TCSHandler tcs1 = {
		.i2c = &hi2c1,
		.adress = 0x29,

};

uint16_t c, r,g,b;


/////////////////////////////////////////////////////////////////////////////////
// USB Receive logic
/////////////////////////////////////////////////////////////////////////////////

// see file usb_snd.c line 213 and 231

void usb_callback(uint8_t *arr, uint16_t len){

	//GET DATA FROM PC

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
	HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, 0);
	HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, 0);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 1);

//	uint8_t buf = 0;
//	for(uint8_t addr = 0; addr < 128; addr++){
//
//		if( HAL_I2C_Mem_Read(&hi2c1, (addr << 1), 0x12, 1, &buf, 1, 100) == HAL_OK){
//			break;
//		}
//
//	}

	HAL_Delay(2000);
	HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, 1);
	HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, 1);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 0);

}





/////////////////////////////////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////////////////////////////////



void kernel_main() {
	
	
	usb_set_callback(&usb_callback);
	kernel_init();

	if(tcs_init(&tcs1) == 0){
		HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, 0);
		HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, 1);
		HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 1);
		while(1){}
	}

	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 1);


    while(1)
    {
    	HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, 1);
    	if(tcs_status(&tcs1) == 0){
    		HAL_Delay(2);
    	}
    	HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, 0);

    	c=tcs_GetC(&tcs1);
    	r=tcs_GetR(&tcs1);
    	g=tcs_GetG(&tcs1);
    	b=tcs_GetB(&tcs1);

		//SEND DATA TO PC
    	usb_rst_cursor();
    	usb_add_uint8(0xFF);
    	usb_add_uint16(c);
    	usb_add_uint16(r);
    	usb_add_uint16(g);
    	usb_add_uint16(b);
    	usb_send_buff();


    }
}



