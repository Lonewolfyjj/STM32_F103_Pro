#ifndef _SPI_H_
#define _SPI_H_
#include "stm32f10x.h"  	
			  	    													  
void SPI2_Init(void);			 //��ʼ��SPI2��

u8 SPI2_ReadWriteByte(u8 data);//SPI2���߶�дһ���ֽ�

void gpio_config(void) ;

void SPI2_Config(void) ;

#endif
