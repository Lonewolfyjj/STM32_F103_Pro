#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "stm32f10x.h"

#define CT_MAX_TOUCH  5    		//������֧�ֵĵ���,�̶�Ϊ5��
//������������
typedef struct 
{
	uint8_t (*init)(void);			//��ʼ��������������
	uint8_t (*scan)(uint8_t);				//ɨ�败����.0,��Ļɨ��;1,��������;	 
	void (*adjust)(uint8_t);		//������У׼ 
	uint16_t x[CT_MAX_TOUCH]; 		//��ǰ����
	uint16_t y[CT_MAX_TOUCH];		//�����������5������,����������x[0],y[0]����:�˴�ɨ��ʱ,����������,��
								//x[4],y[4]�洢��һ�ΰ���ʱ������. 
	uint8_t  sta;					//�ʵ�״̬ 
								//b7:����1/�ɿ�0; 
	                            //b6:0,û�а�������;1,�а�������. 
								//b5:����
								//b4~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����)
/////////////////////������У׼����(����������ҪУ׼)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//�����Ĳ���,��������������������ȫ�ߵ�ʱ��Ҫ�õ�.
//b0:0,����(�ʺ�����ΪX����,����ΪY�����TP)
//   1,����(�ʺ�����ΪY����,����ΪX�����TP) 
//b1~6:����.
//b7:0,������
//   1,������ 
	uint8_t touchtype;
} TP_DEV;

extern TP_DEV tp_dev;

#define VERTICAL 1
#define HORIZONTAL  0

#define             XPT2046_PENIRQ_GPIO_CLK                        RCC_APB2Periph_GPIOF   
#define             XPT2046_PENIRQ_GPIO_PORT                       GPIOF
#define             XPT2046_PENIRQ_GPIO_PIN                        GPIO_Pin_10


/******************************* XPT2046 ������ģ��SPI���Ŷ��� ***************************/
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


/******************************* XPT2046 �����������ź�ָʾ���Ŷ���(��ʹ���ж�) ***************************/
#define             XPT2046_PENIRQ_GPIO_CLK                        RCC_APB2Periph_GPIOF   
#define             XPT2046_PENIRQ_GPIO_PORT                       GPIOF
#define             XPT2046_PENIRQ_GPIO_PIN                        GPIO_Pin_10

//�����ź���Ч��ƽ
#define             XPT2046_PENIRQ_ActiveLevel                     0
#define             XPT2046_PENIRQ_Read()                          GPIO_ReadInputDataBit ( XPT2046_PENIRQ_GPIO_PORT, XPT2046_PENIRQ_GPIO_PIN )

//Ĭ��Ϊtouchtype=0������.

#define     TP_PRES_DOWN    0x80  		//����������	  
#define     SAVE_ADDR_BASE  1024        // ������������������flash��ַ

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
