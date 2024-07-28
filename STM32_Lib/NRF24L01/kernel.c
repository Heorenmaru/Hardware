
/*
 * au_logic.c
 *
 *  Created on: Mar 16, 2022
 *      Author: Heorenmaru
 */

#include "kernel.h"
//#include "adc.h"
#include "nrf24l01.h"

#define DEVICE_CODE 0x0000U
#define DEVICE_VER 	0x00U

/////////////////////////////////////////////////////////////////////////////////
// HANDLERS and VARS
/////////////////////////////////////////////////////////////////////////////////
extern SPI_HandleTypeDef hspi1;
nrf24l01Handler rf={
				.spi = &hspi1,
				.nss.port = NRF_CS_GPIO_Port,
				.nss.pin = NRF_CS_Pin,
				.ce.port = NRF_CE_GPIO_Port,
				.ce.pin = NRF_CE_Pin,
				.irq.port = NRF_IRQ_GPIO_Port,
				.irq.pin = NRF_IRQ_Pin,
};

#define T_TIMEOUT  1
#define T_OK 0

// data array to be sent
uint8_t tx_data[NRF24L01P_PAYLOAD_LENGTH] = {0, 1, 2, 3, 4, 5, 6, 7};
uint8_t rx_data[NRF24L01P_PAYLOAD_LENGTH] = { 0, };
// for rx interrupt
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);



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



	////////////////Start Convert Single mode
	if(arr[0] == 1 ){


		usb_rst_cursor();
		usb_add_uint8(0x01);
		usb_add_uint8(0);

		usb_send_buff();
	}
}






/////////////////////////////////////////////////////////////////////////////////
// INIT
/////////////////////////////////////////////////////////////////////////////////


void kernel_init() {

    //////////////////////////////////
    // Hardware Check
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
	HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, 0);

	HAL_GPIO_WritePin(LED01_GPIO_Port, LED01_Pin, 0);
	HAL_GPIO_WritePin(LED02_GPIO_Port, LED02_Pin, 1);
	HAL_GPIO_WritePin(LED03_GPIO_Port, LED03_Pin, 0);
	HAL_GPIO_WritePin(LED04_GPIO_Port, LED04_Pin, 0);

//	uint8_t buf = 0;
//	for(uint8_t addr = 0; addr < 128; addr++){
//
//		if( HAL_I2C_Mem_Read(&hi2c1, (addr << 1), 0x12, 1, &buf, 1, 100) == HAL_OK){
//			break;
//		}
//
//	}

	HAL_Delay(2000);
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
	HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, 1);

	HAL_GPIO_WritePin(LED01_GPIO_Port, LED01_Pin, 0);
	HAL_GPIO_WritePin(LED02_GPIO_Port, LED02_Pin, 0);
	HAL_GPIO_WritePin(LED03_GPIO_Port, LED03_Pin, 0);
	HAL_GPIO_WritePin(LED04_GPIO_Port, LED04_Pin, 1);

}


/////////////////////////////////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////////////////////////////////

uint8_t regs[0x1d] = { 0, };

void kernel_main() {
	usb_set_callback(&usb_callback);
	kernel_init();

	for(uint8_t i = 0; i<= 0x1d; i++){
		regs[i] = read_register(&rf, i);
	}
	//nrf24l01p_tx_init(&rf, 2500, _1Mbps);
	nrf24l01p_rx_init(&rf, 2500, _1Mbps);

	for(uint8_t i = 0; i<= 0x1d; i++){
		regs[i] = read_register(&rf, i);
	}
    while(1)
    {
    	// change tx datas
    	for(int i= 0; i < 8; i++)
    		tx_data[i]++;

    	// transmit
//    	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
//    	nrf24l01p_tx_transmit(&rf, tx_data);
//    	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
//    	HAL_Delay(1000);


    	if (((rf.irq.port->IDR) & (rf.irq.pin)) == 0){
    		HAL_GPIO_WritePin(LED02_GPIO_Port, LED02_Pin, 1);
    		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
    		nrf24l01p_rx_receive(&rf, rx_data);
    		//nrf24l01p_tx_irq(&rf);
    		HAL_GPIO_WritePin(LED02_GPIO_Port, LED02_Pin, 0);
    		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
    	}





    }
}




