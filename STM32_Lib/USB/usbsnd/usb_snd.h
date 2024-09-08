/*
 *   Author: Heorenmaru
 *   License: CC-BY-SA
 */

#ifndef INC_USB_SND_H_
#define INC_USB_SND_H_
#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"


uint8_t usb_add_uint8(uint8_t data);
uint8_t usb_add_uint16(uint16_t data);
uint8_t usb_add_uint32(uint32_t data);
uint8_t usb_add_uint64(uint64_t data);

//uint8_t usb_add_float(float data);
//uint8_t usb_add_double(double data);


void usb_rst_cursor();
USBD_StatusTypeDef usb_last_status();
USBD_StatusTypeDef usb_send_buff();

//usb_recv(uint8_t b);
/////////////////////////////////////////////////
// OUTDATED
//USBD_StatusTypeDef usb_send(uint8_t *data, uint8_t len);

#endif /* INC_USB_SND_H_ */
