/*
 *   Author: Heorenmaru
 *   License: CC-BY-SA
 */

#include "usb_snd.h"

uint8_t usb_cursor = 0;
uint16_t usb_len = 0;
uint8_t usb_buff[256] = {0};
USBD_StatusTypeDef last_statius = USBD_FAIL;

//////////////////////////////////////////////
// int val
//////////////////////////////////////////////
uint8_t usb_add_uint8(uint8_t data){
	usb_buff[usb_cursor] = data;
	usb_cursor++;
	usb_len++;
	return usb_cursor; //return next byte number
}
uint8_t usb_add_uint16(uint16_t data){
	usb_buff[usb_cursor] = data & 0xFF;
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = data >> 8;
	usb_cursor++;
	usb_len++;
	return usb_cursor;
}
uint8_t usb_add_uint32(uint32_t data){
	usb_buff[usb_cursor] = data 		& 0xFF;
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = (data >> 8 )	& 0xFF;
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = (data >> 16)	& 0xFF;
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = data >> 24;
	usb_cursor++;
	usb_len++;
	return usb_cursor;
}
uint8_t usb_add_uint64(uint64_t data){
	usb_buff[usb_cursor] = data 		& 0xFF;
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = (data >> 8 )	& 0xFF;
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = (data >> 16)	& 0xFF;
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = (data >> 24)	& 0xFF;
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = (data >> 32)	& 0xFF;
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = (data >> 40)	& 0xFF;
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = (data >> 48)	& 0xFF;
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = data >> 56;
	usb_cursor++;
	usb_len++;
	return usb_cursor;
}

//////////////////////////////////////////////
// float/double val
//////////////////////////////////////////////
uint8_t usb_add_float(float data){
	union {
		float fVal;
		uint8_t fArr[4];
	} uF_arr;

	uF_arr.fVal = data;

	usb_buff[usb_cursor] = uF_arr.fArr[0];
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = uF_arr.fArr[1];
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = uF_arr.fArr[2];
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = uF_arr.fArr[3];
	usb_cursor++;
	usb_len++;

	return usb_cursor;
}

uint8_t usb_add_double(double data){
	union {
		double dVal;
		uint8_t dArr[8];
	} uD_arr;

	uD_arr.dVal = data;

	usb_buff[usb_cursor] = uD_arr.dArr[0];
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = uD_arr.dArr[1];
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = uD_arr.dArr[2];
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = uD_arr.dArr[3];
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = uD_arr.dArr[4];
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = uD_arr.dArr[5];
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = uD_arr.dArr[6];
	usb_cursor++;
	usb_len++;
	usb_buff[usb_cursor] = uD_arr.dArr[7];
	usb_cursor++;
	usb_len++;
	return usb_cursor;
}

//////////////////////////////////////////////
// byte array
//////////////////////////////////////////////
uint8_t usb_add_data(uint8_t *data, uint8_t len){
	//TODO: допилить!
	for (uint16_t i = 0; i < len; i++) {
		usb_buff[usb_cursor] = data[i];
		usb_cursor++;
		usb_len++;
	}
	return usb_cursor;
}

void usb_rst_cursor(){
	usb_cursor = 0;
}

USBD_StatusTypeDef usb_last_status(){
	usb_cursor = 0;
}
//////////////////////////////////////////////
// send buffer
//////////////////////////////////////////////
USBD_StatusTypeDef usb_send_buff(){
	//////////////////////////////////////////////
	// generate log packet
		uint16_t data_len = 0;
		uint8_t data_packet[260];
		uint16_t len = 0;
		if(usb_cursor == 0){
			len = 256;
		}else{
			len = usb_cursor;
		}
		//uint8_t	data_crc = 0;
		data_len = 0;

		//preamp
		data_packet[0] = 0xAA;
		data_len++;
		//lenH;
		data_len++;
		//lenL;
		data_len++;

		/////////////fill packet
		for (uint16_t i = 0; i < len; i++) {
			data_packet[data_len] = usb_buff[i];
			data_len++;
		}

		data_packet[1] = data_len>>8;
		data_packet[2] = data_len & 0xFF;
		//data_packet[data_len] = crc8Maxim(data_packet, data_len-1);  //uart sync crc
		data_len++;

		// [0xAA][len][rssi][snr][data...][crc]
		// в длину пакета crc не входит
		last_statius = CDC_Transmit_FS(data_packet, data_len);
		return last_statius;
}








/////////////////////////////////////////////
//
// !!!!!OUTDATED!!!!!
//
/////////////////////////////////////////////
USBD_StatusTypeDef usb_send(uint8_t *data, uint8_t len){
	//////////////////////////////////////////////
	// generate log packet
		uint16_t data_len = 0;
		uint8_t data_packet[260];
		//uint8_t	data_crc = 0;
		data_len = 0;

		//preamp
		data_packet[0] = 0xAA;
		data_len++; //1
		//lenH;
		data_len++; //2
		//lenL;
		data_len++; //3

		/////////////fill packet
		for (uint8_t i = 0; i < len; i++) {
			data_packet[data_len] = data[i];
			data_len++;
		}

		data_packet[1] = data_len>>8;
		data_packet[2] = data_len & 0xFF;
		//data_packet[data_len] = crc8Maxim(data_packet, data_len-1);  //uart sync crc
		//data_len++;

		// [0xAA][len][rssi][snr][data...][crc]
		// в длину пакета crc не входит
		return CDC_Transmit_FS(data_packet, data_len);
}


