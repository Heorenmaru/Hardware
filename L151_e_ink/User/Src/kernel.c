/*
 * au_logic.c
 *
 *  Created on: Mar 16, 2022
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
extern I2C_HandleTypeDef hi2c1;
//extern I2C_HandleTypeDef hi2c2;
extern SPI_HandleTypeDef hspi2;

#define EPD_W21_CS_0	HAL_GPIO_WritePin(DISP_CS_GPIO_Port,DISP_CS_Pin,GPIO_PIN_RESET)
#define EPD_W21_CS_1	HAL_GPIO_WritePin(DISP_CS_GPIO_Port,DISP_CS_Pin,GPIO_PIN_SET)

#define EPD_W21_DC_0	HAL_GPIO_WritePin(DISP_DC_GPIO_Port,DISP_DC_Pin,GPIO_PIN_RESET)
#define EPD_W21_DC_1	HAL_GPIO_WritePin(DISP_DC_GPIO_Port,DISP_DC_Pin,GPIO_PIN_SET)

#define EPD_W21_RST_0	HAL_GPIO_WritePin(DISP_RES_GPIO_Port,DISP_RES_Pin,GPIO_PIN_RESET)
#define EPD_W21_RST_1	HAL_GPIO_WritePin(DISP_RES_GPIO_Port,DISP_RES_Pin,GPIO_PIN_SET)

#define isEPD_W21_BUSY HAL_GPIO_ReadPin(DISP_BUSY_GPIO_Port, DISP_BUSY_Pin) // for solomen solutions

#define EPD_WIDTH   152
#define EPD_HEIGHT  152

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


uint8_t disp_frame[151*19] = {0};

void delay_xms(uint16_t xms)
{
	uint16_t i;
	while(xms--)
	{
		i=12000;
		while(i--);
	}
}
void SPI_Write(unsigned char value)
{
    HAL_SPI_Transmit(&hspi2,&value,1,150);
}


void EPD_Display(uint8_t *Image)
{
    uint16_t Width, Height,i,j;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

    EPD_W21_WriteCMD(0x24);
    for ( j = 0; j < Height; j++) {
        for ( i = 0; i < Width; i++) {
           EPD_W21_WriteDATA(Image[i + j * Width]);
        }
    }
    EPD_Update();
}

void Epaper_Spi_WriteByte(uint8_t TxData)
{
    SPI_Write(TxData);
}

void Epaper_READBUSY(void)
{
  while(1)
  {	 //=1 BUSY
     if(isEPD_W21_BUSY==0) break;
  }
}

void Epaper_Write_Command(uint8_t cmd)
{
	EPD_W21_CS_1;
	EPD_W21_CS_0;
	EPD_W21_DC_0;  // D/C#   0:command  1:data
	Epaper_Spi_WriteByte(cmd);
	EPD_W21_CS_1;
}

void Epaper_Write_Data(uint8_t data)
{
	EPD_W21_CS_1;
	EPD_W21_CS_0;
	EPD_W21_DC_1;  // D/C#   0:command  1:data
	Epaper_Spi_WriteByte(data);
	EPD_W21_CS_1;
}

void EPD_Update(void)
{
  Epaper_Write_Command(0x22); //Display Update Control
  Epaper_Write_Data(0xF7); //0xF7
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();

}

void EPD_WriteScreen(uint8_t* datas)
{

//	uint16_t x,y;
	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(0);
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;
	Epaper_Write_Data(0);
	Epaper_Write_Data(0);
	Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
	for(uint16_t i=0;i<(19*151);i++)
	{
		Epaper_Write_Data(datas[i]);
	}
	EPD_Update();

}

void EPD_ClearRam00(void)
{
	{
		uint16_t x,y;
		Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
		Epaper_Write_Data(0);
		Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;
		Epaper_Write_Data(0);
		Epaper_Write_Data(0);
		Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
		for(x=0;x<19;x++)
	  {
			for(y=0;y<151;y++)
			{
				Epaper_Write_Data(0xFF);
			}
	  }

	}

}


void EPD_WhiteScreen_White(void)

{
	uint16_t i,k;
	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(0);
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;
	Epaper_Write_Data(10);
	Epaper_Write_Data(0);

	Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
	for(k=0;k<20;k++)
  {
		for(i=0;i<150;i++)
		{
			Epaper_Write_Data(0xff);
			}
  }
	EPD_Update();
}

void EPD_test(void)

{
//	uint8_t cnt = 0 ;
	uint16_t x,y;
	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(0);
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;
	Epaper_Write_Data(0);
	Epaper_Write_Data(0);
	Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
	for(x=0;x<19;x++)
  {
		for(y=0;y<151;y++)
		{
			//Epaper_Write_Data(0b01100010);
			Epaper_Write_Data(~y);
//			cnt++;
		}
  }
	EPD_Update();
}

/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//SSD1681
void EPD_HW_Init(void)
{

	EPD_W21_RST_0;  // Module reset
	delay_xms(10);//At least 10ms delay
	EPD_W21_RST_1;
	delay_xms(10); //At least 10ms delay

	Epaper_READBUSY();
	Epaper_Write_Command(0x12);  //SWRESET
	Epaper_READBUSY();

	Epaper_Write_Command(0x01); //Driver output control
	Epaper_Write_Data(151);
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(0b000);

	Epaper_Write_Command(0x11); //data entry mode
	Epaper_Write_Data(0b111);


	Epaper_Write_Command(0x0f); //Gate Scan Start Position
	Epaper_Write_Data(0);



	///// clear ram
	Epaper_Write_Command(0x44); //set Ram-X address start/end position
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(0b11111);

	Epaper_Write_Command(0x45); //set Ram-Y address start/end position
	Epaper_Write_Data(0);
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(0b11111111);
	Epaper_Write_Data(0b1);
	Epaper_Write_Command(0x24);
	for(uint8_t x=0;x<=0b11111;x++)
	  {
			for(uint16_t y=0;y<0b111111111;y++)
			{
				Epaper_Write_Data(0xff);
			}
	 }

	///// conf ram
	Epaper_Write_Command(0x44); //set Ram-X address start/end position
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(18);    //0x0C-->(18+1)*8=200

	Epaper_Write_Command(0x45); //set Ram-Y address start/end position
	Epaper_Write_Data(0);    //0xC7-->(199+1)=200
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(150);
	Epaper_Write_Data(0x00);

	Epaper_Write_Command(0x3C); //BorderWavefrom
//	Epaper_Write_Data(0x05);
	Epaper_Write_Data(0b10001010);

  Epaper_Write_Command(0x18); //Read built-in temperature sensor
	Epaper_Write_Data(0x80);

	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(0);
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;
	Epaper_Write_Data(0);
	Epaper_Write_Data(0);
  Epaper_READBUSY();

}

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
	/// write img
	if(arr[0] == 1 ){

		uint16_t n = arr[1]<<8 | arr[2];
		uint8_t cnt = arr[3];

		for(uint8_t i = 0; i < cnt; i++){
			if((n + i)< 151*19){
				disp_frame[n + i] = arr[i + 4];
			}
		}

		usb_rst_cursor();
		usb_add_uint8(0x01);
		usb_add_uint8(1);
		usb_send_buff();

	}
	/// update
	if(arr[0] == 2 ){

		EPD_WriteScreen(disp_frame);

		usb_rst_cursor();
		usb_add_uint8(0x02);
		usb_add_uint8(1);
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


	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 0);
	EPD_HW_Init();

//	EPD_ClearRam00();

	EPD_test();
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 1);

}


/////////////////////////////////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////////////////////////////////



void kernel_main() {

	usb_set_callback(&usb_callback);
	kernel_init();
    while(1)
    {

    }
}



