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
extern I2C_HandleTypeDef hi2c1;
//extern I2C_HandleTypeDef hi2c2;
extern SPI_HandleTypeDef hspi2;


#define D_WIDTH   152
#define D_HEIGHT  152

uint8_t devaddr = 0x00;

#define T_TIMEOUT  1
#define T_OK 0


void SPI_Write(uint8_t value){
    HAL_SPI_Transmit(&hspi2,&value,1,150);
}


void Dcmd(uint8_t c){
	HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, 0);
	HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, 0);
	SPI_Write(c);
	HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, 1);
	HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, 1);
}

void Ddat(uint8_t c){
	HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, 0);
	HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, 1);
	SPI_Write(c);
	HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, 1);
	HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, 1);
}





void D_WriteByte(uint8_t TxData){
    SPI_Write(TxData);
}



void D_WriteScreen(uint8_t* datas)
{

////	uint16_t x,y;
//	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
//	Epaper_Write_Data(0);
//	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;
//	Epaper_Write_Data(0);
//	Epaper_Write_Data(0);
//	Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
//	for(uint16_t i=0;i<(19*151);i++)
//	{
//		Epaper_Write_Data(datas[i]);
//	}
//	EPD_Update();

}








//----------------------------------------------------------------------
// Module    : ERC13232-1
// Create    : JAVEN LIU
// Date      : Jan-14-2021
// Drive IC  : ST7567A/AIP31567A
// INTERFACE : 4 Wire SPI
// MCU 		 : STC89LE52RC
// VDD		 : 3.3V
//----------------------------------------------------------------------





//KEY Adjust contrast

uint8_t Contrast = 27;

//Resolution

#define Max_Column	132
#define Max_Row		32



//常用ASCII表
//ASCII字符集
//大小:6*8
/************************************6*8的点阵************************************/
const uint8_t F6x8[92*6] =
{
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,// sp
0x00, 0x00, 0x00, 0x2f, 0x00, 0x00,// !
0x00, 0x00, 0x07, 0x00, 0x07, 0x00,// "
0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14,// #
0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12,// $
0x00, 0x62, 0x64, 0x08, 0x13, 0x23,// %
0x00, 0x36, 0x49, 0x55, 0x22, 0x50,// &
0x00, 0x00, 0x05, 0x03, 0x00, 0x00,// '
0x00, 0x00, 0x1c, 0x22, 0x41, 0x00,// (
0x00, 0x00, 0x41, 0x22, 0x1c, 0x00,// )
0x00, 0x14, 0x08, 0x3E, 0x08, 0x14,// *
0x00, 0x08, 0x08, 0x3E, 0x08, 0x08,// +
0x00, 0x00, 0x00, 0xA0, 0x60, 0x00,// ,
0x00, 0x08, 0x08, 0x08, 0x08, 0x08,// -
0x00, 0x00, 0x60, 0x60, 0x00, 0x00,// .
0x00, 0x20, 0x10, 0x08, 0x04, 0x02,// /
0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
0x00, 0x00, 0x42, 0x7F, 0x40, 0x00,// 1
0x00, 0x42, 0x61, 0x51, 0x49, 0x46,// 2
0x00, 0x21, 0x41, 0x45, 0x4B, 0x31,// 3
0x00, 0x18, 0x14, 0x12, 0x7F, 0x10,// 4
0x00, 0x27, 0x45, 0x45, 0x45, 0x39,// 5
0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
0x00, 0x01, 0x71, 0x09, 0x05, 0x03,// 7
0x00, 0x36, 0x49, 0x49, 0x49, 0x36,// 8
0x00, 0x06, 0x49, 0x49, 0x29, 0x1E,// 9
0x00, 0x00, 0x36, 0x36, 0x00, 0x00,// :
0x00, 0x00, 0x56, 0x36, 0x00, 0x00,// ;
0x00, 0x08, 0x14, 0x22, 0x41, 0x00,// <
0x00, 0x14, 0x14, 0x14, 0x14, 0x14,// =
0x00, 0x00, 0x41, 0x22, 0x14, 0x08,// >
0x00, 0x02, 0x01, 0x51, 0x09, 0x06,// ?
0x00, 0x32, 0x49, 0x59, 0x51, 0x3E,// @
0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C,// A
0x00, 0x7F, 0x49, 0x49, 0x49, 0x36,// B
0x00, 0x3E, 0x41, 0x41, 0x41, 0x22,// C
0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C,// D
0x00, 0x7F, 0x49, 0x49, 0x49, 0x41,// E
0x00, 0x7F, 0x09, 0x09, 0x09, 0x01,// F
0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A,// G
0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F,// H
0x00, 0x00, 0x41, 0x7F, 0x41, 0x00,// I
0x00, 0x20, 0x40, 0x41, 0x3F, 0x01,// J
0x00, 0x7F, 0x08, 0x14, 0x22, 0x41,// K
0x00, 0x7F, 0x40, 0x40, 0x40, 0x40,// L
0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F,// M
0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F,// N
0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E,// O
0x00, 0x7F, 0x09, 0x09, 0x09, 0x06,// P
0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
0x00, 0x7F, 0x09, 0x19, 0x29, 0x46,// R
0x00, 0x46, 0x49, 0x49, 0x49, 0x31,// S
0x00, 0x01, 0x01, 0x7F, 0x01, 0x01,// T
0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F,// U
0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F,// V
0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F,// W
0x00, 0x63, 0x14, 0x08, 0x14, 0x63,// X
0x00, 0x07, 0x08, 0x70, 0x08, 0x07,// Y
0x00, 0x61, 0x51, 0x49, 0x45, 0x43,// Z
0x00, 0x00, 0x7F, 0x41, 0x41, 0x00,// [
0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55,// 55
0x00, 0x00, 0x41, 0x41, 0x7F, 0x00,// ]
0x00, 0x04, 0x02, 0x01, 0x02, 0x04,// ^
0x00, 0x40, 0x40, 0x40, 0x40, 0x40,// _
0x00, 0x00, 0x01, 0x02, 0x04, 0x00,// '
0x00, 0x20, 0x54, 0x54, 0x54, 0x78,// a
0x00, 0x7F, 0x48, 0x44, 0x44, 0x38,// b
0x00, 0x38, 0x44, 0x44, 0x44, 0x20,// c
0x00, 0x38, 0x44, 0x44, 0x48, 0x7F,// d
0x00, 0x38, 0x54, 0x54, 0x54, 0x18,// e
0x00, 0x08, 0x7E, 0x09, 0x01, 0x02,// f
0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C,// g
0x00, 0x7F, 0x08, 0x04, 0x04, 0x78,// h
0x00, 0x00, 0x44, 0x7D, 0x40, 0x00,// i
0x00, 0x40, 0x80, 0x84, 0x7D, 0x00,// j
0x00, 0x7F, 0x10, 0x28, 0x44, 0x00,// k
0x00, 0x00, 0x41, 0x7F, 0x40, 0x00,// l
0x00, 0x7C, 0x04, 0x18, 0x04, 0x78,// m
0x00, 0x7C, 0x08, 0x04, 0x04, 0x78,// n
0x00, 0x38, 0x44, 0x44, 0x44, 0x38,// o
0x00, 0xFC, 0x24, 0x24, 0x24, 0x18,// p
0x00, 0x18, 0x24, 0x24, 0x18, 0xFC,// q
0x00, 0x7C, 0x08, 0x04, 0x04, 0x08,// r
0x00, 0x48, 0x54, 0x54, 0x54, 0x20,// s
0x00, 0x04, 0x3F, 0x44, 0x40, 0x20,// t
0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C,// u
0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C,// v
0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C,// w
0x00, 0x44, 0x28, 0x10, 0x28, 0x44,// x
0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C,// y
0x00, 0x44, 0x64, 0x54, 0x4C, 0x44,// z
0x14, 0x14, 0x14, 0x14, 0x14, 0x14,// horiz lines
};







void delay_ms(uint Delay){
HAL_Delay(Delay);

}





//void display_Contrast(uchar number)
//{ 	uint8_t number1,number2;	uint8_t value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
//	SIZE=8;
//	number1=number/10;number2=number%10;
//	ShowChar(0,0,value[number1]) ;
//	ShowChar(6,0,value[number2]) ;
//
//}


//
//void adj_Contrast(void)
//{
//	Set_Contrast_Control_Register(Contrast);
//	display_Contrast(Contrast);
//
//}












void D_Initial(void)
{


//	RST=1;
	delay_ms(10);
//	RST=0;
	delay_ms(10);
//	RST=1;
	delay_ms(10);



	Dcmd(0xe2);//soft reset

	delay_ms(10);


	Dcmd(0x2c);  	//Control built-in power circuit ON	    VB
	Dcmd(0x2e);  	//Control built-in power circuit ON	    VR
	Dcmd(0x2f);  	//Control built-in power circuit ON	    VF

	delay_ms(10);


	Dcmd(0xf8);  //Set booster level
	Dcmd(0x00);  //Set  4X booster level

	Dcmd(0x25);  //Select regulation resistor ratio
	Dcmd(0x81);  	 //Setelectronic volume (EV) level
	Dcmd(Contrast);  //Setelectronic volume (EV) level
	Dcmd(0xa2);  //1/9bias
	Dcmd(0xc8);  //Set output direction of COM
	Dcmd(0xa0);  //Set scan direction of SEG
	Dcmd(0xa4);  //Set normal display

	Dcmd(0xa6);  //Set normal display

	Dcmd(0x40);  //start line
	Dcmd(0xaf);  //  display ON


}


void Set_Contrast_Control_Register(uint8_t mod)
{
	Dcmd(0x81);
	Dcmd(mod);
}


// Set page address 0~8
void Set_Page_Address(uint8_t add)
{
    add=0xb0|add;
    Dcmd(add);
}

void Set_Column_Address(uint8_t add)
{
	Dcmd((0x10|(add>>4)));
	Dcmd((0x0f&add));
}



void Set_Pos(uint8_t x, uint8_t y)
{

	Dcmd(0xb0+y);
	Dcmd(((x&0xf0)>>4)|0x10);
	Dcmd((x&0x0f));

}



//// ShowChar   size: 16/8
void ShowChar(uint8_t x,uint8_t y,uint8_t chr)
{

	uint8_t c=0,i=0;
		c=chr-' ';
		if(x>Max_Column-1){
			x=0;
		}

		Set_Pos(x,y);
		for(i=0;i<6;i++)
			Ddat(F6x8[c*6+i]);

//		Ddat(F6x8[c]);
//		Ddat(F6x8[c+1]);
//		Ddat(F6x8[c+2]);
//		Ddat(F6x8[c+3]);
//		Ddat(F6x8[c+4]);
//		Ddat(F6x8[c+5]);
//		Ddat(F6x8[c+6]);

}



////m^n函数
//unsigned int pow(uint8_t m,uint8_t n)
//{
//	unsigned int result=1;
//	while(n--)result*=m;
//	return result;
//}
////Show  number
//void ShowNum(uint8_t x,uint8_t y,uint16_t num,uint8_t len,uint8_t size2)
//{
//	uint8_t t,temp;
//	uint8_t enshow=0;
//	for(t=0;t<len;t++)
//	{
//		temp=(num/pow(10,len-t-1))%10;
//		if(enshow==0&&t<(len-1))
//		{
//			if(temp==0)
//			{
//				ShowChar(x+(size2/2)*t,y,' ');
//				continue;
//			}else enshow=1;
//
//		}
//	 	ShowChar(x+(size2/2)*t,y,temp+'0');
//	}
//}
//ShowString
void ShowString(uint8_t x,uint8_t y,uint8_t *chr)
{
	uint8_t j=0;
	while (chr[j]!='\0')
	{		ShowChar(x,y,chr[j]);
			x+=8;
		if(x>124)
		{
			x=0;
			y+=1;
		}
			j++;
	}
}
////ShowCHinese
//void ShowCHinese(uint8_t x,uint8_t y,uint8_t no)
//{
//	uint8_t t;
//	Set_Pos(x,y);
//    for(t=0;t<16;t++)
//		{
//				Write_Data(Hzk[32*no+t]);
//
//        }
//		Set_Pos(x,y+1);
//    for(t=16;t<32;t++)
//		{
//				Write_Data(Hzk[32*no+t]);
//
//        }
//}




void Dcls()
{
	uint8_t i,j;

	for(i=0;i<8;i++)
	{
		Set_Page_Address(i);
		Set_Column_Address(0x00);
        for(j=0;j<132;j++)
		{
		    Ddat(0x00);
		}

	}

}

//void Show(uint8_t d1,uint8_t d2)
//{
//    uint8_t i,j;
//	for(i=0;i<4;i++)
//	{
//	Set_Page_Address(i);
//    Set_Column_Address(0x00);
//        for(j=0;j<66;j++)
//		{
//		    Write_Data(d1);
//			Write_Data(d2);
//		}
//	}
//    return;
//}


//void Show_line_dot()
//{
//   Show(0x55,0xaa);
// 	delay_ms(1000);
//   Show(0xaa,0x55);
// 	delay_ms(1000);
//
//   Show(0x55,0x55);
// 	delay_ms(1000);
//   Show(0xaa,0xaa);
// 	delay_ms(1000);
//
//   Show(0xff,0x00);
// 	delay_ms(1000);
//   Show(0x00,0xff);
// 	delay_ms(1000);
//
//    return;
//}



//void main(void)
//{
//	P0=0xff; P1=0xff;P2=0xff;P3=0xff;
//
//	IE=0x81;
//	IP=0x01;
//	TCON=0x01;
//
//	delay_ms(100);
//
//
//	while(1)
//	{
//		 	ClearScreen();
//
//
//
//
//
//
//		SIZE=16;
//		ShowString(10,2,"Buydisplay.com");
//		delay_ms(1000);
//
//
//		Show_line_dot();
//
//	   	ClearScreen();
//
//		DrawBMP(0,0,132,4,pic1); //x:0,y:0,col:132,line:4*8=64
//		delay_ms(1000);
//	    Write_Instruction(0xa7);  //Set  inverse  display
//		delay_ms(1000);
//	    Write_Instruction(0xa6);  //Set normal display
//
//
//		DrawBMP(0,0,132,4,pic2);  //x:0,y:0,col:132,line:4*8=64
//		delay_ms(1000);
// 	    Write_Instruction(0xa7);  //Set  inverse  display
//		delay_ms(1000);
//	    Write_Instruction(0xa6);  //Set normal display
//
//
//
//	}
//}







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

//		for(uint8_t i = 0; i < cnt; i++){
//			if((n + i)< 151*19){
//				disp_frame[n + i] = arr[i + 4];
//			}
//		}

		usb_rst_cursor();
		usb_add_uint8(0x01);
		usb_add_uint8(1);
		usb_send_buff();

	}
	/// update
	if(arr[0] == 2 ){

//		EPD_WriteScreen(disp_frame);

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

	D_Initial();
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 0);


	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, 1);

}


/////////////////////////////////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////////////////////////////////

MLXHandler MLX = {
		.i2c = &hi2c1,
		.adress = 0x5A
};

float ObjectTemperature;
float AmbientTemperature;
float Emissivity;
char buf[30] = {' '};

void kernel_main() {

	usb_set_callback(&usb_callback);
	kernel_init();

	HAL_Delay(2000);
	Dcls();

	MLX90614_SetEmissivity(0.985); // Human skin

	ShowString(10,0,"           ");
	ShowString(10,1,"           ");

    while(1)
    {
    	MLX90614_ReadAmbientTemperature(&MLX, &AmbientTemperature);
    	MLX90614_ReadObjectTemperature(&MLX, &ObjectTemperature);
    	MLX90614_GetEmissivity(&MLX, &Emissivity);
//    	ShowString(10,0,"           ");
//    	ShowString(10,1,"           ");
//    	ShowString(10,2,"           ");
    	sprintf(buf, "Amb: %.2f   ", AmbientTemperature);
    	ShowString(10,0,(uint8_t*)buf);

    	sprintf(buf, "Obj: %.2f   ", ObjectTemperature);
    	ShowString(10,1,(uint8_t*)buf);

    	sprintf(buf, "Emi: %.3f   ", Emissivity);
    	ShowString(10,2,(uint8_t*)buf);

    	HAL_Delay(100);




    }
}



