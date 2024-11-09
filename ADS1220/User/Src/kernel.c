/*
 * au_logic.c
 *
 *  Created on: Mar 16, 2022
 *      Author: Heorenmaru
 */

#include "kernel.h"

#define DEVICE_CODE 0x0001U
#define DEVICE_VER 	0x00U

/////////////////////////////////////////////////////////////////////////////////
// HANDLERS and VARS
/////////////////////////////////////////////////////////////////////////////////
//extern ADC_HandleTypeDef hadc;
//extern TIM_HandleTypeDef htim3;
extern SPI_HandleTypeDef hspi2;


//uint16_t adc_data[9 * 8] = { 0 };

#define T_TIMEOUT  1
#define T_OK 0


/////////////////////////////////////////////////////////////////////////////////
// USB Receive logic
/////////////////////////////////////////////////////////////////////////////////



void usb_callback(uint8_t *arr, uint16_t len){



	////////////
	// DEV INFO
	// (STANDART COMMAND)
	if(arr[0] == 0 ){
		uint8_t cmd_count = arr[1];
		uint16_t *idBase0 = (uint16_t*)(UID_BASE);
		uint16_t *idBase1 = (uint16_t*)(UID_BASE + 0x02);
		uint32_t *idBase2 = (uint32_t*)(UID_BASE + 0x04);
		uint32_t *idBase3 = (uint32_t*)(UID_BASE + 0x08);

		usb_rst_cursor();
		usb_add_uint8(0x00);
		usb_add_uint8(cmd_count);
		usb_add_uint16(idBase0);
		usb_add_uint16(idBase1);
		usb_add_uint32(idBase2);
		usb_add_uint32(idBase3);

		usb_add_uint16((uint8_t)DEVICE_CODE);
		usb_add_uint8((uint8_t)DEVICE_VER);

		usb_send_buff();

	}


	////////////////cmd
	if(arr[0] == 1 ){
		uint8_t cmd_count = arr[1];
//		if(arr[2] == 0){
//			if(arr[3] == 1){
//				HAL_GPIO_WritePin(B7_GPIO_Port, B7_Pin, 1);
//			}else{
//				HAL_GPIO_WritePin(B7_GPIO_Port, B7_Pin, 0);
//			}
//		}
//		if(arr[2] == 1){
//			if(arr[3] == 1){
//				HAL_GPIO_WritePin(B6_GPIO_Port, B6_Pin, 1);
//			}else{
//				HAL_GPIO_WritePin(B6_GPIO_Port, B6_Pin, 0);
//			}
//		}
//		if(arr[2] == 2){
//			if(arr[3] == 1){
//				HAL_GPIO_WritePin(B5_GPIO_Port, B5_Pin, 1);
//			}else{
//				HAL_GPIO_WritePin(B5_GPIO_Port, B5_Pin, 0);
//			}
//		}
//		if(arr[2] == 3){
//			if(arr[3] == 1){
//				HAL_GPIO_WritePin(B4_GPIO_Port, B4_Pin, 1);
//			}else{
//				HAL_GPIO_WritePin(B4_GPIO_Port, B4_Pin, 0);
//			}
//		}
//		if(arr[2] == 4){
//			if(arr[3] == 1){
//				HAL_GPIO_WritePin(B3_GPIO_Port, B3_Pin, 1);
//			}else{
//				HAL_GPIO_WritePin(B3_GPIO_Port, B3_Pin, 0);
//			}
//		}
//
//		HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 0);
//		HAL_ADC_Start_DMA(&hadc, (uint32_t*)&adc_data, 9*8);
//
		usb_rst_cursor();
		usb_add_uint8(0x01);
		usb_add_uint8(cmd_count);
		usb_add_uint8(0);
//
		usb_send_buff();
	}


}



/////////////////////////////////////////////////////////////////////////////////
// ADC
/////////////////////////////////////////////////////////////////////////////////

//#define ADC_REFERENCE_VOLTAGE					1.224f
//#define ADC_MAX									0x1000//0xFFF
//
//void calc_voltage()
//{
//	float vref_adc 	= 0 ;
//	float adc_0 	= 0 ;
//	float adc_1 	= 0 ;
//	float adc_2 	= 0 ;
//	float adc_3 	= 0 ;
//	float adc_4 	= 0 ;
//	float adc_5 	= 0 ;
//	float adc_6 	= 0 ;
//	float adc_7 	= 0 ;
//
//	for (uint8_t i = 0; i < 8*9; i = i + 9) {
//		vref_adc 	+= adc_data[i];
//		adc_0 		+= adc_data[i + 1];
//		adc_1 		+= adc_data[i + 2];
//		adc_2 		+= adc_data[i + 3];
//		adc_3 		+= adc_data[i + 4];
//		adc_4 		+= adc_data[i + 5];
//		adc_5 		+= adc_data[i + 6];
//		adc_6 		+= adc_data[i + 7];
//		adc_7 		+= adc_data[i + 8];
//
//
//	}
//
//	vref_adc 	= vref_adc 	/ 8;
//	adc_0 		= adc_0 	/ 8;
//	adc_1 		= adc_1 	/ 8;
//	adc_2 		= adc_2 	/ 8;
//	adc_3 		= adc_3 	/ 8;
//	adc_4 		= adc_4 	/ 8;
//	adc_5 		= adc_5 	/ 8;
//	adc_6		= adc_6 	/ 8;
//	adc_7 		= adc_7 	/ 8;
//
//
//	vref_adc 	=  ADC_MAX * ADC_REFERENCE_VOLTAGE / vref_adc;
//	adc_0 		=  vref_adc / ADC_MAX * adc_0;
//	adc_1 		=  vref_adc / ADC_MAX * adc_1;
//	adc_2 		=  vref_adc / ADC_MAX * adc_2;
//	adc_3 		=  vref_adc / ADC_MAX * adc_3;
//	adc_4 		=  vref_adc / ADC_MAX * adc_4;
//	adc_5 		=  vref_adc / ADC_MAX * adc_5;
//	adc_6 		=  vref_adc / ADC_MAX * adc_6;
//	adc_7 		=  vref_adc / ADC_MAX * adc_7;
//
//	//voltage = (float) adc_2 * ( (RV_HI + RV_LO) / RV_LO );
//	//current = (float) ( adc_3 / (1 + (RA_HI/RA_LO)) ) / R_SHUNT;   //(float)3/3*adc_3
//	//power = voltage*current;
//	usb_rst_cursor();
//	usb_add_uint8(0xFD);
//	usb_add_float(&vref_adc);
//	usb_add_float(&adc_0);
//	usb_add_float(&adc_1);
//	usb_add_float(&adc_2);
//	usb_add_float(&adc_3);
//	usb_add_float(&adc_4);
//	usb_add_float(&adc_5);
//	usb_add_float(&adc_6);
//	usb_add_float(&adc_7);
//	usb_send_buff();
//	HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 1);
//}
//

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



//void setR(uint8_t val){
//	TIM3->CCR3 = TIM3->ARR - val;
//}
//
//void setG(uint8_t val){
//	TIM3->CCR1 = TIM3->ARR - val;
//}
//
//void setB(uint8_t val){
//	TIM3->CCR2 = TIM3->ARR - val;
//}
//
//void setY(uint8_t val){
//	TIM3->CCR4 = val;
//}

/////////////////////////////////////////////////////////////////////////////////
// INIT
/////////////////////////////////////////////////////////////////////////////////


void kernel_init() {

    //////////////////////////////////
    // Hardware Check

	HAL_Delay(2000);
	HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 1);

}


/////////////////////////////////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////////////////////////////////



void kernel_main() {
	usb_set_callback(&usb_callback);
	kernel_init();


//	 HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
//	 HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
//	 HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
//	 HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

	//	page 58
	//00h 3Eh  0b00111110 // 0b0011 111 0
	//01h 04h  0b00000100 // 0b00000000
	//02h 98h  0b10011000 // 0b10 01 1 000
	//03h 00h


	//page 35
	//RESET (0000 011x)
	//START/SYNC (0000 100x)
	//RDATA (0001 xxxx)
	//RREG (0010 rrnn)
	//WREG (0100 rrnn)
	uint8_t RESET = 0b00000110;
	uint8_t START = 0b00001000;
	uint8_t RDATA = 0b00010000;
	uint8_t RREG  = 0b00100000;
	uint8_t WREG  = 0b01000000;

	uint8_t cmd = 0b00000110;
	//reset
	HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, 0);
	HAL_SPI_Transmit(&hspi2, &cmd, 1, 1000);
	HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, 1);

	HAL_Delay(2000);


	//conf


	uint8_t MUX = 0b0011;
	uint8_t GAIN = 0b111;

	uint8_t param[4] ={(MUX<<4) | (GAIN<<1) | (0b0), 0b00000000, 0b10011000, 0b00000000};

	cmd = WREG | 0b00<<2 | 0b01;
	HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, 0);
	HAL_SPI_Transmit(&hspi2, &cmd, 1, 1000);
	HAL_SPI_Transmit(&hspi2, &param[0], 1, 1000);
	HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, 1);

	cmd = WREG | 0b01<<2 | 0b01;
	HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, 0);
	HAL_SPI_Transmit(&hspi2, &cmd, 1, 1000);
	HAL_SPI_Transmit(&hspi2, &param[1], 1, 1000);
	HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, 1);

	cmd = WREG | 0b10<<2 | 0b01;
	HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, 0);
	HAL_SPI_Transmit(&hspi2, &cmd, 1, 1000);
	HAL_SPI_Transmit(&hspi2, &param[2], 1, 1000);
	HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, 1);

	cmd = WREG | 0b11<<2 | 0b01;
	HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, 0);
	HAL_SPI_Transmit(&hspi2, &cmd, 1, 1000);
	HAL_SPI_Transmit(&hspi2, &param[3], 1, 1000);
	HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, 1);



	HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 0);
	uint8_t data[64] = {0};
	int32_t val = 0;

	uint32_t tme = HAL_GetTick();
    while(1)
    {
    	//cs
    	HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, 0);

    	//START
    	cmd = 0b00001000;
    	HAL_SPI_Transmit(&hspi2, &cmd, 1, 1000);
    	//read DRDY
    	tme = HAL_GetTick();
    	while(1){
    		if(HAL_GPIO_ReadPin(ADS_DRDY_GPIO_Port, ADS_DRDY_Pin) < 1){
    			break;
    		}
    		if(HAL_GetTick()> (tme + 1000)){
    			break;
    		}
    	}
    	//RDATA
    	cmd = 0b00000000;
    	//HAL_SPI_Transmit(&hspi2, &cmd, 1, 1000);
    	for (uint8_t i=0;i<3;i++){
    		HAL_SPI_TransmitReceive(&hspi2, &cmd, &data[i], 1, 100);
//    		HAL_SPI_Receive(&hspi2, &data[i], 1, 1000);
    	}
    	val = ((int32_t)(data[0]<<24 | (data[1]<<16) | (data[2]<<8)| (data[3])));

		usb_rst_cursor();
		usb_add_uint8(0xff);
		usb_add_uint32(val);
		usb_send_buff();


    }
}



