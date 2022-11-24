#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "stm32f10x.h"

#define CT_MAX_TOUCH  5    		//电容屏支持的点数,固定为5点
//触摸屏控制器
typedef struct 
{
	uint8_t (*init)(void);			//初始化触摸屏控制器
	uint8_t (*scan)(uint8_t);				//扫描触摸屏.0,屏幕扫描;1,物理坐标;	 
	void (*adjust)(uint8_t);		//触摸屏校准 
	uint16_t x[CT_MAX_TOUCH]; 		//当前坐标
	uint16_t y[CT_MAX_TOUCH];		//电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
								//x[4],y[4]存储第一次按下时的坐标. 
	uint8_t  sta;					//笔的状态 
								//b7:按下1/松开0; 
	                            //b6:0,没有按键按下;1,有按键按下. 
								//b5:保留
								//b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
/////////////////////触摸屏校准参数(电容屏不需要校准)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
//b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
//   1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
//b1~6:保留.
//b7:0,电阻屏
//   1,电容屏 
	uint8_t touchtype;
} TP_DEV;

extern TP_DEV tp_dev;

#define VERTICAL 1
#define HORIZONTAL  0

#define             XPT2046_PENIRQ_GPIO_CLK                        RCC_APB2Periph_GPIOF   
#define             XPT2046_PENIRQ_GPIO_PORT                       GPIOF
#define             XPT2046_PENIRQ_GPIO_PIN                        GPIO_Pin_10


/******************************* XPT2046 触摸屏模拟SPI引脚定义 ***************************/
#define             XPT2046_SPI_GPIO_CLK                          RCC_APB2Periph_GPIOF| RCC_APB2Periph_GPIOB

#define             XPT2046_SPI_CS_PIN		                      GPIO_Pin_11
#define             XPT2046_SPI_CS_PORT		                      GPIOF

#define	            XPT2046_SPI_CLK_PIN	                          GPIO_Pin_1
#define             XPT2046_SPI_CLK_PORT	                      GPIOB

#define	            XPT2046_SPI_MOSI_PIN	                      GPIO_Pin_9
#define	            XPT2046_SPI_MOSI_PORT	                      GPIOF

#define	            XPT2046_SPI_MISO_PIN	                      GPIO_Pin_2
#define	            XPT2046_SPI_MISO_PORT	                      GPIOB



#define             XPT2046_CS_LOW()                              GPIO_ResetBits ( XPT2046_SPI_CS_PORT, XPT2046_SPI_CS_PIN )    
#define             XPT2046_CS_HIGH()                             GPIO_SetBits ( XPT2046_SPI_CS_PORT, XPT2046_SPI_CS_PIN )  

#define             XPT2046_CLK_HIGH()                            GPIO_SetBits ( XPT2046_SPI_CLK_PORT, XPT2046_SPI_CLK_PIN )    
#define             XPT2046_CLK_LOW()                             GPIO_ResetBits ( XPT2046_SPI_CLK_PORT, XPT2046_SPI_CLK_PIN ) 

#define             XPT2046_MOSI_1()                              GPIO_SetBits ( XPT2046_SPI_MOSI_PORT, XPT2046_SPI_MOSI_PIN ) 
#define             XPT2046_MOSI_0()                              GPIO_ResetBits ( XPT2046_SPI_MOSI_PORT, XPT2046_SPI_MOSI_PIN )

#define             XPT2046_MISO()                               GPIO_ReadInputDataBit ( XPT2046_SPI_MISO_PORT, XPT2046_SPI_MISO_PIN )


/******************************* XPT2046 触摸屏触摸信号指示引脚定义(不使用中断) ***************************/
#define             XPT2046_PENIRQ_GPIO_CLK                        RCC_APB2Periph_GPIOF   
#define             XPT2046_PENIRQ_GPIO_PORT                       GPIOF
#define             XPT2046_PENIRQ_GPIO_PIN                        GPIO_Pin_10

//触屏信号有效电平
#define             XPT2046_PENIRQ_ActiveLevel                     0
#define             XPT2046_PENIRQ_Read()                          GPIO_ReadInputDataBit ( XPT2046_PENIRQ_GPIO_PORT, XPT2046_PENIRQ_GPIO_PIN )

//默认为touchtype=0的数据.

#define     TP_PRES_DOWN    0x80  		//触屏被按下	  
#define     SAVE_ADDR_BASE  1024        // 电阻屏触摸参数储存flash地址

void rtp_test(void);
void  XPT2046_Init(void) ;
void GPIO_Config(void) ;
void XPT2046_SPI_SendData(uint8_t data) ;
uint16_t XPT2046_SPI_RecvData(void) ;
uint16_t XPT2046_ReadAdc(uint8_t channelCmd) ;
uint8_t XPT2046_ReadAdc_XY (uint16_t * x, uint16_t * y)  ;
uint8_t TP_Init(void) ;
void ILI9341_DrawCross ( uint16_t usX, uint16_t usY );
void drowCrossInfo(uint8_t i,uint16_t x, uint16_t y ); 
uint8_t TP_Scan(uint8_t tp);
void TP_Adjust( uint8_t LCD_Mode);
uint8_t TP_Read_XY2(uint16_t *x, uint16_t *y) ;
uint8_t XPT2046_ReadAdc_XY ( uint16_t * x, uint16_t * y )  ;
u8 TP_Get_Adjdata(void);
void TP_Save_Adjdata(void);
#endif
