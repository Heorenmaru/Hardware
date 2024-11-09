
#ifndef __KERNEL_H___
#define __KERNEL_H__


//#include "protocol.h"

//#include "radioconf.h"



#include "crclib.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"

#include "handlers.h"



void kernel_init();
void kernel_main();







#endif /* INC_KERNEL_H_ */
